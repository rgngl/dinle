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
#include "dinle-media-file.h"
#include "dinle-media-metadata.h"
#include "dinle-commands.h"
#include "dinle-db.h"
#include "dinle-archive-manager.h"

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
    GSList *search_tags;
    GSList *search_values;
    gchar *search_type;
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

    if (priv->search_tags) {
        gint i;
        for (i = 0; i < g_slist_length (priv->search_tags); i++)
            g_free (g_slist_nth_data (priv->search_tags, i));
    }

    if (priv->search_values) {
        gint i;
        for (i = 0; i < g_slist_length (priv->search_values); i++)
            g_free (g_slist_nth_data (priv->search_values, i));
    }

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
    self->priv->search_tags = NULL;
    self->priv->search_values = NULL;
    self->priv->search_type = NULL;
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

    if (!g_strcmp0 (element_name, DINLE_COMMAND_SEARCH)) {
        g_print ("search command started\n");
        gchar **name = attribute_names;
        gchar **value = attribute_values;
        while (*name) {
            if (!g_strcmp0 (*name, DINLE_PARAM_SEARCH_TYPE)) {
                if (!g_strcmp0 (*value, DINLE_PVALUE_SEARCH_TAG)) {
                    priv->search_type = DINLE_PVALUE_SEARCH_TAG;
                    g_print ("search type: %s\n", *value);
                } else if (!g_strcmp0 (*value, DINLE_PVALUE_SEARCH_FREE)) {
                    priv->search_type = DINLE_PVALUE_SEARCH_FREE;
                    g_print ("search type: %s\n", *value);
                }
            }
            name ++;
            value ++;
        }
    } else if (!g_strcmp0 (element_name, DINLE_COMMAND_TAG) &&
               !g_strcmp0 (priv->search_type, DINLE_PVALUE_SEARCH_TAG)) {
        g_print ("tag ");
        gchar **name = attribute_names;
        gchar **value = attribute_values;
        gchar *field = NULL;
        gchar *pvalue = NULL;
        while (*name) {
            if (!g_strcmp0 (*name, DINLE_PARAM_FIELD)) {
                field = *value;
                g_print ("field %s ", *value);
            } if (!g_strcmp0 (*name, DINLE_PARAM_VALUE)) {
                pvalue = *value;
                g_print ("value %s ", *value);
            }
            name ++;
            value ++;
        }
        g_print ("\n");
        if (field && pvalue) {
            g_print ("adding search terms %s %s\n", field, pvalue);
            priv->search_tags = g_slist_append (priv->search_tags, g_strdup (field));
            priv->search_values = g_slist_append (priv->search_values, g_strdup (pvalue));
        }
    }
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

    if (!g_strcmp0 (element_name, DINLE_COMMAND_SEARCH)) {
        /*g_print ("search close %s %d\n", priv->search_type, g_slist_length (priv->search_tags));*/
        if (!g_strcmp0 (priv->search_type, DINLE_PVALUE_SEARCH_TAG) &&
             g_slist_length (priv->search_tags) > 0) {
            /*g_print ("building taglist\n");*/
            gchar **tag_pairs = g_malloc0 (sizeof(gchar*) * (g_slist_length (priv->search_tags)*2 + 1));
            guint i = 0;
            guint index = 0;
            for (i = 0; i < g_slist_length (priv->search_tags); i++) {
                tag_pairs[index++] = g_slist_nth_data (priv->search_tags, i);
                g_print (g_slist_nth_data (priv->search_tags, i));
                tag_pairs[index++] = g_slist_nth_data (priv->search_values, i);
                g_print (g_slist_nth_data (priv->search_values, i));
            }

            DinleDb *db = dinle_archive_manager_get_db ();
            DinleMediaFile** mflist = dinle_db_search_by_tags (db, tag_pairs);
            if (mflist) {
                g_signal_emit_by_name (self, "reply", DINLE_TAG_START (DINLE_REPLY_SEARCHRESULT, ""));
            }
            DinleMediaFile** mfiter = mflist;
            while (mfiter && *mfiter) {
                /*g_print ("%s\n", dinle_media_file_get_path (*mfiter));*/
                GString *filetag = g_string_new ("");
                gchar *esc = g_markup_printf_escaped (DINLE_TAG_START (DINLE_REPLY_TRACK, DINLE_PARAM_TRACKID"='%s'")"\n",
                                              dinle_media_file_get_hash (*mfiter));
                g_string_append (filetag, esc);
                g_free (esc);
                const DinleMediaMetadata *md = dinle_media_file_get_metadata (*mfiter);
                gchar **fvlist = dinle_media_metadata_get_field_value_list (md);
                gchar **fviter = fvlist;
                while (fviter && *fviter) {
                    esc = g_markup_printf_escaped (DINLE_TAG_ALONE (DINLE_REPLY_TAG, "%s='%s'"), *fviter, *(fviter+1));
                    g_string_append (filetag, esc);
                    g_free (esc);
                    fviter+=2;
                }
                g_string_append (filetag, DINLE_TAG_END (DINLE_REPLY_TRACK)"\n");
                g_signal_emit_by_name (self, "reply", filetag->str);
                mfiter++;
            }
            if (mflist) {
                g_signal_emit_by_name (self, "reply", DINLE_TAG_END ("search-result")"\n");
            }
            g_strfreev (tag_pairs);
            g_slist_free (priv->search_tags);
            priv->search_tags = NULL;
            g_slist_free (priv->search_values);
            priv->search_values = NULL;
        }
    }
}

DinleSessionHandler *
dinle_session_handler_ready_new (void)
{
    return g_object_new (DINLE_TYPE_SESSION_HANDLER_READY, NULL);
}
