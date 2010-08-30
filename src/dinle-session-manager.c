/* dinle-session-manager.c */

#include "dinle-session-manager.h"

G_DEFINE_TYPE (DinleSessionManager, dinle_session_manager, G_TYPE_OBJECT)

#define SESSION_MANAGER_PRIVATE(o) \
        (G_TYPE_INSTANCE_GET_PRIVATE ((o), DINLE_TYPE_SESSION_MANAGER, DinleSessionManagerPrivate))

struct _DinleSessionManagerPrivate
{
    int foo;
};


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
}

    DinleSessionManager *
dinle_session_manager_new (void)
{
    return g_object_new (DINLE_TYPE_SESSION_MANAGER, NULL);
}
