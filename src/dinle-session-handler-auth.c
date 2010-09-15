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
/* dinle-session-handler-auth.c */

#include "dinle-session-handler-auth.h"
#include "dinle-commands.h"

G_DEFINE_TYPE (DinleSessionHandlerAuth, dinle_session_handler_auth, DINLE_TYPE_SESSION_HANDLER)

#define SESSION_HANDLER_AUTH_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), DINLE_TYPE_SESSION_HANDLER_AUTH, DinleSessionHandlerAuthPrivate))

typedef enum {
    DINLE_AUTH_STATE_UNKNOWN,

    DINLE_AUTH_STATE_NEW,
    DINLE_AUTH_STATE_DONE,

    DINLE_AUTH_STATE_NUM
} DinleAuthState;

struct _DinleSessionHandlerAuthPrivate
{
    GMarkupParseContext *parse_context;
    DinleAuthState state;
};


static gboolean _process (DinleSessionHandler *handler, gchar *data, gsize len);
static void _start (DinleSessionHandler *handler);

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
dinle_session_handler_auth_get_property (GObject    *object,
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
dinle_session_handler_auth_set_property (GObject      *object,
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
dinle_session_handler_auth_dispose (GObject *object)
{
    G_OBJECT_CLASS (dinle_session_handler_auth_parent_class)->dispose (object);
}

static void
dinle_session_handler_auth_finalize (GObject *object)
{
    g_return_if_fail (DINLE_IS_SESSION_HANDLER_AUTH (object));
    DinleSessionHandlerAuthPrivate *priv = SESSION_HANDLER_AUTH_PRIVATE (object);

    if (priv->parse_context)
        g_markup_parse_context_free (priv->parse_context);
}

static void
dinle_session_handler_auth_class_init (DinleSessionHandlerAuthClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    DinleSessionHandlerClass *parent_class = DINLE_SESSION_HANDLER_CLASS (klass);

    g_type_class_add_private (klass, sizeof (DinleSessionHandlerAuthPrivate));

    object_class->get_property = dinle_session_handler_auth_get_property;
    object_class->set_property = dinle_session_handler_auth_set_property;
    object_class->dispose = dinle_session_handler_auth_dispose;
    object_class->finalize = dinle_session_handler_auth_finalize;
    parent_class->process = _process;
    parent_class->start = _start;
}

static void
dinle_session_handler_auth_init (DinleSessionHandlerAuth *self)
{
    self->priv = SESSION_HANDLER_AUTH_PRIVATE (self);

    self->priv->parse_context = g_markup_parse_context_new (&parser, 0, self, NULL);
    self->priv->state = DINLE_AUTH_STATE_NEW;
}

static gboolean
_process (DinleSessionHandler *handler, gchar *data, gsize len)
{
    g_return_val_if_fail (DINLE_IS_SESSION_HANDLER_AUTH (handler), FALSE);
    DinleSessionHandlerAuthPrivate *priv = SESSION_HANDLER_AUTH_PRIVATE (handler);

    switch (priv->state)
    {
        case DINLE_AUTH_STATE_NEW:
            if (g_markup_parse_context_parse (priv->parse_context,
                        data, len, NULL) == FALSE) {
                g_warning ("failed parsing commands.\n");
                return FALSE;
            }
            break;
        case DINLE_AUTH_STATE_DONE:
            g_signal_emit_by_name (handler, "done", TRUE);
            break;
        default:
            g_warning ("unknown auth state...\n");
            return FALSE;
    }
    return TRUE;
}

static void
_start (DinleSessionHandler *handler)
{
    g_return_if_fail (DINLE_IS_SESSION_HANDLER_AUTH (handler));
    DinleSessionHandlerAuthPrivate *priv = SESSION_HANDLER_AUTH_PRIVATE (handler);

    g_signal_emit_by_name (handler, "reply", DINLE_TAG_ALONE (DINLE_REPLY_AUTHSTART, ""));
}

static gboolean
_process_timeout (gpointer data)
{
    _process (DINLE_SESSION_HANDLER (data), "", 1);
    return FALSE;
}

static void
_start_element (GMarkupParseContext *context, const gchar *element_name,
                const gchar **attribute_names,const gchar **attribute_values,
                gpointer user_data, GError **error)
{
    DinleSessionHandlerAuth *self = DINLE_SESSION_HANDLER_AUTH (user_data);
    g_return_if_fail (DINLE_IS_SESSION_HANDLER_AUTH (self));
    DinleSessionHandlerAuthPrivate *priv = SESSION_HANDLER_AUTH_PRIVATE (self);

    /*g_print ("start: %s\n", element_name);*/
    const gchar **name = attribute_names;
    const gchar **value = attribute_values;

    if (!g_strcmp0 (element_name, "dummy-login")) {
        g_signal_emit_by_name (self, "reply", DINLE_TAG_ALONE (DINLE_REPLY_AUTHOK, ""));
        priv->state = DINLE_AUTH_STATE_DONE;
        /* this is an ugly hack, because we can't send the signal from this
         * handler.*/
        g_timeout_add (10, _process_timeout, user_data);
        return;
    }

    while (*name) {
        /*g_print ("%s = %s\n",*name, *value);*/
        name ++;
        value ++;
    }
}

static void
_text (GMarkupParseContext *context, const gchar *text, gsize text_len,
       gpointer user_data, GError **error)
{
    /*g_print ("text: %*s\n", (gint)text_len, text);*/
}

static void
_end_element (GMarkupParseContext *context, const gchar *element_name,
              gpointer user_data, GError **error)
{
    DinleSessionHandlerAuth *self = DINLE_SESSION_HANDLER_AUTH (user_data);
    g_return_if_fail (DINLE_IS_SESSION_HANDLER_AUTH (self));
    DinleSessionHandlerAuthPrivate *priv = SESSION_HANDLER_AUTH_PRIVATE (self);

    /*g_print ("end: %s\n", element_name);*/
}

DinleSessionHandler *
dinle_session_handler_auth_new (void)
{
    return g_object_new (DINLE_TYPE_SESSION_HANDLER_AUTH, NULL);
}
