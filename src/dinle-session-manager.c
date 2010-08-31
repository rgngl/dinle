/* dinle-session-manager.c */

#include <glib.h>
#include <gio/gio.h>
#include <stdlib.h>

#include "dinle-session-manager.h"
#include "dinle-config-manager.h"

G_DEFINE_TYPE (DinleSessionManager, dinle_session_manager, G_TYPE_OBJECT)

#define SESSION_MANAGER_PRIVATE(o) \
        (G_TYPE_INSTANCE_GET_PRIVATE ((o), DINLE_TYPE_SESSION_MANAGER, DinleSessionManagerPrivate))

struct _DinleSessionManagerPrivate
{
    GSocketService *service;
    GInetAddress *address;
    GSocketAddress *socket_address;
};

static DinleSessionManager *instance = NULL;


static gboolean _connection_received(GThreadedSocketService *service, GSocketConnection *connection,
                                     GObject *source_object, gpointer user_data);
static gboolean _network_read(GIOChannel *source, GIOCondition cond, gpointer data);
static DinleSessionManager* dinle_session_manager_new (void);

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
    g_object_get (dinle_config_manager_get (),
                  "server-port", &dinle_server_port,
                  NULL);

    GError *err = NULL;
    self->priv->service = g_threaded_socket_service_new (16);
    self->priv->address = g_inet_address_new_any (G_SOCKET_FAMILY_IPV6);
    self->priv->socket_address = g_inet_socket_address_new (self->priv->address, (guint16)dinle_server_port);
    gboolean success = g_socket_listener_add_address(G_SOCKET_LISTENER(self->priv->service),
                                                     self->priv->socket_address, G_SOCKET_TYPE_STREAM,
                                                     G_SOCKET_PROTOCOL_TCP, NULL, NULL, NULL);

    if (!success || !dinle_server_port) {
        g_print ("cannot bind on port, aborting.\n");
        abort ();
    }
    g_object_unref(self->priv->socket_address);
    g_object_unref(self->priv->address);
    g_socket_service_start(self->priv->service);

    g_signal_connect(self->priv->service, "run", G_CALLBACK(_connection_received), self);
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
    GSocketAddress *sockaddr = g_socket_connection_get_remote_address(connection, NULL);
    GInetAddress *addr = g_inet_socket_address_get_address(G_INET_SOCKET_ADDRESS(sockaddr));
    guint16 port = g_inet_socket_address_get_port(G_INET_SOCKET_ADDRESS(sockaddr));

    g_print ("New Connection from %s:%d\n", g_inet_address_to_string(addr), port);

    g_object_ref (connection);
    GSocket *socket = g_socket_connection_get_socket(connection);

    gint fd = g_socket_get_fd (socket);
    GIOChannel *channel = g_io_channel_unix_new(fd);
    g_io_add_watch(channel, G_IO_IN, (GIOFunc) _network_read, connection);
    /*GInputStream *input = g_unix_input_stream_new (fd, TRUE);*/
    /*GDataInputStream *data_input = g_data_input_stream_new (input);*/
    /*GOutputStream *output = g_unix_output_stream_new (fd, TRUE);*/

    return TRUE;
}

static gboolean
_network_read (GIOChannel *source,
               GIOCondition cond,
               gpointer data)
{
    gchar *str = NULL;
    gsize len = 0;
    GError *error = NULL;
    GIOStatus ret = g_io_channel_read_line(source, &str, &len, NULL, &error);

    if (ret == G_IO_STATUS_ERROR) {
        g_warning ("Error reading: %s\n", error->message);
        // Drop last reference on connection
        g_object_unref (data);
        // Remove the event source
    } else
        g_print("Got: %s\n", str);

    g_free (str);

    if (ret == G_IO_STATUS_EOF || ret == G_IO_STATUS_ERROR) {
        return FALSE;
    }

    g_io_channel_write_chars (source, "oi oi oi\n", -1, &len, &error);
    g_io_channel_flush(source, &error);

    return TRUE;
}
