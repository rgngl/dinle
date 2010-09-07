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
/* dinle-session-manager.c */

#include <glib.h>
#include <gio/gio.h>
#include <stdlib.h>

#include "config.h"

#include "dinle-session-manager.h"
#include "dinle-config-manager.h"
#include "dinle-session.h"

G_DEFINE_TYPE (DinleSessionManager, dinle_session_manager, G_TYPE_OBJECT)

#define SESSION_MANAGER_PRIVATE(o) \
        (G_TYPE_INSTANCE_GET_PRIVATE ((o), DINLE_TYPE_SESSION_MANAGER, DinleSessionManagerPrivate))

#if HAVE_IPV6
    GSocketFamily dinle_socket_family = G_SOCKET_FAMILY_IPV6;
#else
    GSocketFamily dinle_socket_family = G_SOCKET_FAMILY_IPV4;
#endif

struct _DinleSessionManagerPrivate
{
    GSocketService *service;
    GInetAddress *address;
    GSocketAddress *socket_address;
    GHashTable *sessions;
};

static DinleSessionManager *instance = NULL;


static gboolean _connection_received(GThreadedSocketService *service, GSocketConnection *connection,
                                     GObject *source_object, gpointer user_data);
static DinleSessionManager* dinle_session_manager_new (void);
static void _session_done (DinleSession *session, gpointer user_data);

DinleSessionManager *
dinle_session_manager_get (void)
{
    if (!instance)
        instance = dinle_session_manager_new();

    return instance;
}

static void
dinle_session_manager_get_property (GObject    *object,
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
dinle_session_manager_set_property (GObject      *object,
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
dinle_session_manager_dispose (GObject *object)
{
    G_OBJECT_CLASS (dinle_session_manager_parent_class)->dispose (object);
}

static void
dinle_session_manager_finalize (GObject *object)
{
    G_OBJECT_CLASS (dinle_session_manager_parent_class)->finalize (object);
}

static void
dinle_session_manager_class_init (DinleSessionManagerClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    g_type_class_add_private (klass, sizeof (DinleSessionManagerPrivate));

    object_class->get_property = dinle_session_manager_get_property;
    object_class->set_property = dinle_session_manager_set_property;
    object_class->dispose = dinle_session_manager_dispose;
    object_class->finalize = dinle_session_manager_finalize;
}

static void
dinle_session_manager_init (DinleSessionManager *self)
{
    self->priv = SESSION_MANAGER_PRIVATE (self);
    gint dinle_server_port;
    gint max_sessions;
    g_object_get (dinle_config_manager_get (),
                  "server-port", &dinle_server_port,
                  "max-sessions", &max_sessions,
                  NULL);

    self->priv->sessions = g_hash_table_new (g_direct_hash, g_direct_equal);

    GError *err = NULL;
    self->priv->service = g_threaded_socket_service_new (max_sessions);
    self->priv->address = g_inet_address_new_any (dinle_socket_family);
    self->priv->socket_address = g_inet_socket_address_new (self->priv->address,
                                                            (guint16)dinle_server_port);
    gboolean success = g_socket_listener_add_address(G_SOCKET_LISTENER(self->priv->service),
                                                     self->priv->socket_address, G_SOCKET_TYPE_STREAM,
                                                     G_SOCKET_PROTOCOL_TCP, NULL, NULL, &err);

    if (!success || !dinle_server_port) {
        g_print ("cannot bind on port, aborting. %s\n", err->message);
        abort ();
    }
    g_print ("dinled is now running.\n");

    g_object_unref (self->priv->socket_address);
    g_object_unref (self->priv->address);
    g_socket_service_start (self->priv->service);

    g_signal_connect (self->priv->service, "run", G_CALLBACK(_connection_received), self);
}

static DinleSessionManager *
dinle_session_manager_new (void)
{
    return g_object_new (DINLE_TYPE_SESSION_MANAGER, NULL);
}

static gboolean
_connection_received (GThreadedSocketService *service,
                      GSocketConnection *connection,
                      GObject *source_object,
                      gpointer user_data)
{
    DinleSessionManager *self = DINLE_SESSION_MANAGER (user_data);
    g_return_val_if_fail (DINLE_IS_SESSION_MANAGER (self), FALSE);
    DinleSessionManagerPrivate *priv = SESSION_MANAGER_PRIVATE (self);

    DinleSession *ns = dinle_session_new (connection);
    g_signal_connect (ns, "done", G_CALLBACK (_session_done), self);
    g_hash_table_insert (priv->sessions, ns, connection);

    return TRUE;
}

static void _session_done
(DinleSession *session, gpointer user_data)
{
    DinleSessionManager *self = DINLE_SESSION_MANAGER (user_data);
    g_return_if_fail (DINLE_IS_SESSION_MANAGER (self));
    DinleSessionManagerPrivate *priv = SESSION_MANAGER_PRIVATE (self);

    g_return_if_fail (DINLE_IS_SESSION (session));

    g_object_unref (session);
    g_hash_table_remove (priv->sessions, (gconstpointer) session);
    g_print ("session destroyed.\n");
}
