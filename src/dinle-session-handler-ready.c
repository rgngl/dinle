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
/* dinle-session-handler-ready.c */

#include "dinle-session-handler-ready.h"

G_DEFINE_TYPE (DinleSessionHandlerReady, dinle_session_handler_ready, DINLE_TYPE_SESSION_HANDLER)

#define SESSION_HANDLER_READY_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), DINLE_TYPE_SESSION_HANDLER_READY, DinleSessionHandlerReadyPrivate))

typedef enum {
    DINLE_READY_STATE_UNKNOWN,

    DINLE_READY_STATE_NEW,

    DINLE_READY_STATE_NUM
} DinleReadyState;

struct _DinleSessionHandlerReadyPrivate
{
    GMarkupParseContext *parse_context;
    DinleReadyState state;
};


static gboolean _process (DinleSessionHandler *handler, gchar *data, gsize len);

static void _start_element (GMarkupParseContext *context,
                            const gchar *element_name,
                            const gchar **attribute_names,
                            const gchar **attribute_values,
                            gpointer user_data, GError **error);
static void _text (GMarkupParseContext *context, const gchar *text, gsize text_len,
                   gpointer user_data, GError **error);
static void _end_element (GMarkupParseContext *context, const gchar *element_name,
                          gpointer user_data, GError **error);

static GMarkupParser parser = {
    _start_element,
    _end_element,
    _text,
    NULL,
    NULL
};


static void
dinle_session_handler_ready_get_property (GObject    *object,
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
dinle_session_handler_ready_set_property (GObject      *object,
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
dinle_session_handler_ready_dispose (GObject *object)
{
    G_OBJECT_CLASS (dinle_session_handler_ready_parent_class)->dispose (object);
}

static void
dinle_session_handler_ready_finalize (GObject *object)
{
    g_return_if_fail (DINLE_IS_SESSION_HANDLER_READY (object));
    DinleSessionHandlerReadyPrivate *priv = SESSION_HANDLER_READY_PRIVATE (object);

    if (priv->parse_context)
        g_markup_parse_context_free (priv->parse_context);

    G_OBJECT_CLASS (dinle_session_handler_ready_parent_class)->finalize (object);
}

static void
dinle_session_handler_ready_class_init (DinleSessionHandlerReadyClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    DinleSessionHandlerClass *parent_class = DINLE_SESSION_HANDLER_CLASS (klass);

    g_type_class_add_private (klass, sizeof (DinleSessionHandlerReadyPrivate));

    object_class->get_property = dinle_session_handler_ready_get_property;
    object_class->set_property = dinle_session_handler_ready_set_property;
    object_class->dispose = dinle_session_handler_ready_dispose;
    object_class->finalize = dinle_session_handler_ready_finalize;
    parent_class->process = _process;
}

static void
dinle_session_handler_ready_init (DinleSessionHandlerReady *self)
{
    self->priv = SESSION_HANDLER_READY_PRIVATE (self);

    self->priv->parse_context = g_markup_parse_context_new (&parser, 0, self, NULL);
    self->priv->state = DINLE_READY_STATE_NEW;
}

static gboolean
_process (DinleSessionHandler *handler, gchar *data, gsize len)
{
    g_return_val_if_fail (DINLE_IS_SESSION_HANDLER_READY (handler), FALSE);
    DinleSessionHandlerReadyPrivate *priv = SESSION_HANDLER_READY_PRIVATE (handler);

    if (g_markup_parse_context_parse (priv->parse_context,
                                      data, len, NULL) == FALSE) {
        g_warning ("failed parsing commands.\n");
        return FALSE;
    }
    return TRUE;
}

static void
_start_element (GMarkupParseContext *context, const gchar *element_name,
                const gchar **attribute_names,const gchar **attribute_values,
                gpointer user_data, GError **error)
{
    DinleSessionHandlerReady *self = DINLE_SESSION_HANDLER_READY (user_data);
    g_return_if_fail (DINLE_IS_SESSION_HANDLER_READY (self));
    DinleSessionHandlerReadyPrivate *priv = SESSION_HANDLER_READY_PRIVATE (self);
}

static void
_text (GMarkupParseContext *context, const gchar *text, gsize text_len,
       gpointer user_data, GError **error)
{
}

static void
_end_element (GMarkupParseContext *context, const gchar *element_name,
              gpointer user_data, GError **error)
{
    DinleSessionHandlerReady *self = DINLE_SESSION_HANDLER_READY (user_data);
    g_return_if_fail (DINLE_IS_SESSION_HANDLER_READY (self));
    DinleSessionHandlerReadyPrivate *priv = SESSION_HANDLER_READY_PRIVATE (self);
}

DinleSessionHandler *
dinle_session_handler_ready_new (void)
{
    return g_object_new (DINLE_TYPE_SESSION_HANDLER_READY, NULL);
}
