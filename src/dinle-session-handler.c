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
/* dinle-session-handler.c */

#include "dinle-session-handler.h"

G_DEFINE_TYPE (DinleSessionHandler, dinle_session_handler, G_TYPE_OBJECT)

#define SESSION_HANDLER_PRIVATE(o) \
        (G_TYPE_INSTANCE_GET_PRIVATE ((o), DINLE_TYPE_SESSION_HANDLER, DinleSessionHandlerPrivate))

typedef enum {
    DONE_SIGNAL,

    NUM_SIGNALS
} DinleSessionSignals;
static guint signals[NUM_SIGNALS] = {0};

struct _DinleSessionHandlerPrivate
{
    int foo;
};


static void
dinle_session_handler_get_property (GObject    *object,
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
dinle_session_handler_set_property (GObject      *object,
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
dinle_session_handler_dispose (GObject *object)
{
    G_OBJECT_CLASS (dinle_session_handler_parent_class)->dispose (object);
}

static void
dinle_session_handler_finalize (GObject *object)
{
    G_OBJECT_CLASS (dinle_session_handler_parent_class)->finalize (object);
}

static void
dinle_session_handler_class_init (DinleSessionHandlerClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    g_type_class_add_private (klass, sizeof (DinleSessionHandlerPrivate));

    object_class->get_property = dinle_session_handler_get_property;
    object_class->set_property = dinle_session_handler_set_property;
    object_class->dispose = dinle_session_handler_dispose;
    object_class->finalize = dinle_session_handler_finalize;
    klass->process = dinle_session_handler_process;

    signals[DONE_SIGNAL] =
        g_signal_new ("done",
                G_TYPE_FROM_CLASS (klass),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (DinleSessionHandlerClass, done),
                NULL, NULL,
                g_cclosure_marshal_VOID__BOOLEAN,
                G_TYPE_NONE, 1, G_TYPE_BOOLEAN);
}

static void
dinle_session_handler_init (DinleSessionHandler *self)
{
    self->priv = SESSION_HANDLER_PRIVATE (self);
}

gboolean
dinle_session_handler_process (DinleSessionHandler *self, gchar *data, gsize len)
{
    g_return_val_if_fail (DINLE_IS_SESSION_HANDLER (self), FALSE);

    gboolean ret = FALSE;
    if (DINLE_SESSION_HANDLER_GET_CLASS (self)->process)
        ret = DINLE_SESSION_HANDLER_GET_CLASS (self)->process (self, data, len);
    else g_critical ("Pure virtual function process called...\n");

    return ret;
}
