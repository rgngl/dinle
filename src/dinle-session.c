/*
    This file is part of Dinle.
    Copyright 2010  Üstün Ergenoglu

    Dinle is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dinle is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dinle.  If not, see <http://www.gnu.org/licenses/>.
*/
/* dinle-session.c */

#include "dinle-session.h"
#include "dinle-commands.h"
#include "dinle-session-handler-auth.h"
#include "dinle-session-handler-new.h"
#include "dinle-session-handler-ready.h"
#include "config.h"

G_DEFINE_TYPE (DinleSession, dinle_session, G_TYPE_OBJECT)

#define SESSION_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), DINLE_TYPE_SESSION, DinleSessionPrivate))

#define BUF_LEN (1024*16)

typedef enum {
    DINLE_SESSION_STATE_UNKNOWN,

    DINLE_SESSION_STATE_NEW,
    DINLE_SESSION_STATE_AUTH,
    DINLE_SESSION_STATE_READY
} DinleSessionState;

struct _DinleSessionPrivate
{
    GSocketConnection *conn;
    GIOChannel *channel;
    DinleSessionHandler *handler;
    DinleSessionState state;
    guint io_source;
};

typedef enum {
    DONE_SIGNAL,
    ACTIVITY_SIGNAL,

    NUM_SIGNALS
} DinleSessionSignals;
static guint signals[NUM_SIGNALS] = {0};

static void _init (DinleSession *self, GSocketConnection *conn);
static gboolean _network_read(GIOChannel *source, GIOCondition cond, gpointer data);

static void _handle_auth_done (DinleSessionHandler *handler,
                               gboolean success, gpointer user_data);
static void _handle_new_done (DinleSessionHandler *handler,
                              gboolean success, gpointer user_data);
static void _handle_ready_done (DinleSessionHandler *handler,
                              gboolean success, gpointer user_data);
static void _handler_reply (DinleSessionHandler *handler,
                            const gchar *reply, gpointer user_data);
static void _clean_session (DinleSession *self);

static void
dinle_session_get_property (GObject    *object,
                            guint       property_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
    switch (property_id)
    {
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
dinle_session_set_property (GObject      *object,
                            guint         property_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
    switch (property_id)
    {
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
dinle_session_dispose (GObject *object)
{
    G_OBJECT_CLASS (dinle_session_parent_class)->dispose (object);
}

static void
dinle_session_finalize (GObject *object)
{
    g_return_if_fail (DINLE_IS_SESSION (object));
    DinleSessionPrivate *priv = SESSION_PRIVATE (object);

    _clean_session (DINLE_SESSION (object));

    G_OBJECT_CLASS (dinle_session_parent_class)->finalize (object);
}

static void
dinle_session_class_init (DinleSessionClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    g_type_class_add_private (klass, sizeof (DinleSessionPrivate));

    object_class->get_property = dinle_session_get_property;
    object_class->set_property = dinle_session_set_property;
    object_class->dispose = dinle_session_dispose;
    object_class->finalize = dinle_session_finalize;

    signals[DONE_SIGNAL] =
        g_signal_new ("done",
                G_TYPE_FROM_CLASS (klass),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (DinleSessionClass, done),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);

    signals[ACTIVITY_SIGNAL] =
        g_signal_new ("activity",
                G_TYPE_FROM_CLASS (klass),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (DinleSessionClass, activity),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
}

static void
dinle_session_init (DinleSession *self)
{
    self->priv = SESSION_PRIVATE (self);

    self->priv->state = DINLE_SESSION_STATE_NEW;

    self->priv->handler = dinle_session_handler_new_new ();
    g_signal_connect (self->priv->handler, "done",
                      G_CALLBACK (_handle_new_done), self);
    g_signal_connect (self->priv->handler, "reply",
                      G_CALLBACK (_handler_reply), self);

    self->priv->conn = NULL;
    self->priv->channel = NULL;
    self->priv->io_source = 0;
}

static void
_init (DinleSession *self, GSocketConnection *conn)
{
    g_return_if_fail (DINLE_IS_SESSION (self));
    DinleSessionPrivate *priv = SESSION_PRIVATE (self);

    GError *error = NULL;

    priv->conn = conn;
    GSocketAddress *sockaddr = g_socket_connection_get_remote_address (conn, NULL);
    GInetAddress *addr = g_inet_socket_address_get_address (G_INET_SOCKET_ADDRESS (sockaddr));
    guint16 port = g_inet_socket_address_get_port (G_INET_SOCKET_ADDRESS (sockaddr));

    g_print ("New Connection from %s:%d\n", g_inet_address_to_string(addr), port);

    g_object_ref (conn);
    GSocket *socket = g_socket_connection_get_socket (conn);

    gint fd = g_socket_get_fd (socket);
    priv->channel = g_io_channel_unix_new (fd);
    g_io_channel_set_encoding (priv->channel, NULL, &error);
    gsize written = 0;
    g_io_channel_write_chars (priv->channel, DINLE_TAG_SERVER "\n",
                              -1, &written, &error);
    g_io_channel_flush (priv->channel, &error);
    priv->io_source = g_io_add_watch (priv->channel, G_IO_IN, (GIOFunc) _network_read, self);
}

static gboolean
_network_read (GIOChannel *source,
               GIOCondition cond,
               gpointer data)
{
    DinleSession *self = DINLE_SESSION (data);
    g_return_val_if_fail (DINLE_IS_SESSION (self), FALSE);
    DinleSessionPrivate *priv = SESSION_PRIVATE (self);

    gchar buf[BUF_LEN+1];
    gsize len = 0;
    GError *error = NULL;
    GIOStatus ret = g_io_channel_read_chars (source, buf, BUF_LEN, &len, &error);
    buf[len] = '\0';

    if (ret == G_IO_STATUS_EOF) {
        dinle_session_close (self);
        return FALSE;
    }

    if (ret == G_IO_STATUS_ERROR) {
        g_warning ("Error reading: %s\n", error->message);
        dinle_session_close (self);
        return FALSE;
    }

    g_return_val_if_fail (DINLE_IS_SESSION_HANDLER (priv->handler), TRUE);

    if (!dinle_session_handler_process (priv->handler, buf, len)) {
        g_io_channel_write_chars (source, "<error>Protocol error.</error>",
                -1, &len, &error);
        g_io_channel_flush (source, &error);
        dinle_session_close (self);
        return FALSE;
    }

    if (error) {
        g_error_free (error);
        error = NULL;
    }

    g_signal_emit_by_name (self, "activity");

    return TRUE;
}

static void
_handle_ready_done (DinleSessionHandler *handler, gboolean success, gpointer user_data)
{
    DinleSession *self = DINLE_SESSION (user_data);
    g_return_if_fail (DINLE_IS_SESSION (self));
    DinleSessionPrivate *priv = SESSION_PRIVATE (self);
}

static void
_handle_auth_done (DinleSessionHandler *handler, gboolean success, gpointer user_data)
{
    DinleSession *self = DINLE_SESSION (user_data);
    g_return_if_fail (DINLE_IS_SESSION (self));
    DinleSessionPrivate *priv = SESSION_PRIVATE (self);

    guint len;
    GError *error = NULL;

    if (success) {
        g_print ("session auth->ready\n");
        g_object_unref (priv->handler);
        priv->state = DINLE_SESSION_STATE_READY;
        priv->handler = dinle_session_handler_ready_new ();
        g_signal_connect (priv->handler,
                          "done", G_CALLBACK (_handle_ready_done), self);
        g_signal_connect (priv->handler, "reply",
                          G_CALLBACK (_handler_reply), self);
    } else {
        g_io_channel_write_chars (priv->channel,
                                  DINLE_TAG_ERROR ("Failed parsing message"),
                                  -1, (gsize*)&len, &error);
        g_io_channel_flush (priv->channel, &error);
        dinle_session_close (self);
    }
}

static void
_handle_new_done (DinleSessionHandler *handler,
                  gboolean success, gpointer user_data)
{
    DinleSession *self = DINLE_SESSION (user_data);
    g_return_if_fail (DINLE_IS_SESSION (self));
    DinleSessionPrivate *priv = SESSION_PRIVATE (self);

    guint len;
    GError *error = NULL;

    if (success) {
        g_print ("session new->auth\n");
        g_object_unref (priv->handler);
        priv->state = DINLE_SESSION_STATE_AUTH;
        priv->handler = dinle_session_handler_auth_new ();
        g_signal_connect (priv->handler,
                          "done", G_CALLBACK (_handle_auth_done), self);
        g_signal_connect (priv->handler, "reply",
                          G_CALLBACK (_handler_reply), self);
    } else {
        g_io_channel_write_chars (priv->channel,
                                  DINLE_TAG_ERROR ("Protocol error."),
                                  -1, (gsize*)&len, &error);
        g_io_channel_flush (priv->channel, &error);
        dinle_session_close (self);
    }
}

static void
_handler_reply (DinleSessionHandler *handler,
                const gchar *reply, gpointer user_data)
{
    g_return_if_fail (DINLE_IS_SESSION_HANDLER (handler));
    DinleSession *self = DINLE_SESSION (user_data);
    g_return_if_fail (DINLE_IS_SESSION (self));
    DinleSessionPrivate *priv = SESSION_PRIVATE (self);

    guint len;
    GError *error = NULL;

    g_io_channel_write_chars (priv->channel, reply, -1, (gsize*)&len, &error);
    g_io_channel_flush (priv->channel, &error);
}

static void
_clean_session (DinleSession *self)
{
    g_return_if_fail (DINLE_IS_SESSION (self));
    DinleSessionPrivate *priv = SESSION_PRIVATE (self);

    if (priv->conn) {
        g_object_unref (priv->conn);
        priv->conn = NULL;
    }

    if (priv->channel) {
        if (!g_source_remove (priv->io_source))
            g_print ("oi..\n");;
        g_io_channel_unref (priv->channel);
        priv->channel = NULL;
    }

    if (priv->handler) {
        g_object_unref (priv->handler);
        priv->handler = NULL;
    }

}

DinleSession *
dinle_session_new (GSocketConnection *conn)
{
    GObject *obj = g_object_new (DINLE_TYPE_SESSION, NULL);
    _init (DINLE_SESSION (obj), conn);

    return DINLE_SESSION (obj);
}

gboolean
dinle_session_close (DinleSession *self)
{
    g_return_val_if_fail (DINLE_IS_SESSION (self), FALSE);
    DinleSessionPrivate *priv = SESSION_PRIVATE (self);

    _clean_session (self);

    g_signal_emit_by_name (self, "done");

    return TRUE;
}
