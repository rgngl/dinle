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
#include "config.h"

G_DEFINE_TYPE (DinleSession, dinle_session, G_TYPE_OBJECT)

#define SESSION_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), DINLE_TYPE_SESSION, DinleSessionPrivate))

#define BUF_LEN 1000

enum {
    DINLE_SESSION_STATE_UNKNOWN,

    DINLE_SESSION_STATE_NEW,
    DINLE_SESSION_STATE_AUTHORIZE,
    DINLE_SESSION_STATE_READY
} _DinleSesssionState;
typedef enum _DinleSessionState DinleSessionState;

struct _DinleSessionPrivate
{
    GSocketConnection *conn;
    guint state;
};

typedef enum {
    DONE_SIGNAL,

    NUM_SIGNALS
} DinleSessionSignals;
static guint signals[NUM_SIGNALS] = {0};

static void _init (DinleSession *self, GSocketConnection *conn);
static gboolean _network_read(GIOChannel *source, GIOCondition cond, gpointer data);

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
                g_cclosure_marshal_VOID__INT,
                G_TYPE_NONE, 1, G_TYPE_INT);

}

static void
dinle_session_init (DinleSession *self)
{
    self->priv = SESSION_PRIVATE (self);
}

static void
_init (DinleSession *self, GSocketConnection *conn)
{
    g_return_if_fail (DINLE_IS_SESSION (self));
    DinleSessionPrivate *priv = SESSION_PRIVATE (self);

    GError *error = NULL;

    priv->state = DINLE_SESSION_STATE_NEW;

    priv->conn = conn;
    GSocketAddress *sockaddr = g_socket_connection_get_remote_address (conn, NULL);
    GInetAddress *addr = g_inet_socket_address_get_address (G_INET_SOCKET_ADDRESS (sockaddr));
    guint16 port = g_inet_socket_address_get_port (G_INET_SOCKET_ADDRESS (sockaddr));

    g_print ("New Connection from %s:%d\n", g_inet_address_to_string(addr), port);

    g_object_ref (conn);
    GSocket *socket = g_socket_connection_get_socket (conn);

    gint fd = g_socket_get_fd (socket);
    GIOChannel *channel = g_io_channel_unix_new (fd);
    gsize written = 0;
    g_io_channel_write_chars (channel, PACKAGE_NAME PACKAGE_VERSION"\n", -1, &written, &error);
    g_io_channel_flush (channel, &error);
    g_io_add_watch (channel, G_IO_IN, (GIOFunc) _network_read, conn);
}

static gboolean
_network_read (GIOChannel *source,
               GIOCondition cond,
               gpointer data)
{
    GString *str = g_string_new ("");
    gchar buf[BUF_LEN+1];
    gsize len = 0;
    GError *error = NULL;
    GIOStatus ret = g_io_channel_read_chars (source, buf, BUF_LEN, &len, &error);
    buf[BUF_LEN] = '\0';

    if (ret == G_IO_STATUS_EOF) {
        g_object_unref (data);
        g_print ("EOF\n");
        return TRUE;
    } else  if (ret == G_IO_STATUS_ERROR) {
        g_warning ("Error reading: %s\n", error->message);
        // Drop last reference on connection
        g_object_unref (data);
        // Remove the event source
    } else
        g_print("Got: %s %d\n", buf, (guint)len);

    g_string_free (str, TRUE);

    if (ret == G_IO_STATUS_ERROR)
        return FALSE;

    g_io_channel_write_chars (source, "oi oi oi\n", -1, &len, &error);
    g_io_channel_flush (source, &error);

    return TRUE;
}

DinleSession *
dinle_session_new (GSocketConnection *conn)
{
    GObject *obj = g_object_new (DINLE_TYPE_SESSION, NULL);
    _init (DINLE_SESSION (obj), conn);

    return DINLE_SESSION (obj);
}
