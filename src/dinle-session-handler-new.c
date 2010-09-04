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
/* dinle-session-handler-new.c */

#include "dinle-session-handler-new.h"

#include "dinle-commands.h"

G_DEFINE_TYPE (DinleSessionHandlerNew, dinle_session_handler_new, DINLE_TYPE_SESSION_HANDLER)

#define SESSION_HANDLER_NEW_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), DINLE_TYPE_SESSION_HANDLER_NEW, DinleSessionHandlerNewPrivate))

struct _DinleSessionHandlerNewPrivate
{
    int foo; /*to avoid warnings.*/
};


static gboolean _process (DinleSessionHandler *handler, gchar *data, gsize len);

static void
dinle_session_handler_new_get_property (GObject    *object,
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
dinle_session_handler_new_set_property (GObject      *object,
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
dinle_session_handler_new_dispose (GObject *object)
{
    G_OBJECT_CLASS (dinle_session_handler_new_parent_class)->dispose (object);
}

static void
dinle_session_handler_new_finalize (GObject *object)
{
    g_return_if_fail (DINLE_IS_SESSION_HANDLER_NEW (object));
    DinleSessionHandlerNewPrivate *priv = SESSION_HANDLER_NEW_PRIVATE (object);

    G_OBJECT_CLASS (dinle_session_handler_new_parent_class)->finalize (object);
}

static void
dinle_session_handler_new_class_init (DinleSessionHandlerNewClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    DinleSessionHandlerClass *parent_class = DINLE_SESSION_HANDLER_CLASS (klass);

    g_type_class_add_private (klass, sizeof (DinleSessionHandlerNewPrivate));

    object_class->get_property = dinle_session_handler_new_get_property;
    object_class->set_property = dinle_session_handler_new_set_property;
    object_class->dispose = dinle_session_handler_new_dispose;
    object_class->finalize = dinle_session_handler_new_finalize;
    parent_class->process = _process;
}

static void
dinle_session_handler_new_init (DinleSessionHandlerNew *self)
{
    self->priv = SESSION_HANDLER_NEW_PRIVATE (self);
}

static gboolean
_process (DinleSessionHandler *handler, gchar *data, gsize len)
{
    g_return_val_if_fail (DINLE_IS_SESSION_HANDLER_NEW (handler), FALSE);
    DinleSessionHandlerNewPrivate *priv = SESSION_HANDLER_NEW_PRIVATE (handler);

    if (!g_str_has_prefix (data, DINLE_CLIENT_TAG))
        return FALSE;

    g_signal_emit_by_name (handler, "done", TRUE);

    return TRUE;
}

DinleSessionHandler *
dinle_session_handler_new_new (void)
{
    return g_object_new (DINLE_TYPE_SESSION_HANDLER_NEW, NULL);
}
