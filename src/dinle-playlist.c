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
/* dinle-playlist.c */

#include "dinle-playlist.h"
#include "dinle-archive-manager.h"
#include "dinle-media-metadata.h"
#include "dinle-commands.h"

G_DEFINE_TYPE (DinlePlaylist, dinle_playlist, G_TYPE_OBJECT)

#define PLAYLIST_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), DINLE_TYPE_PLAYLIST, DinlePlaylistPrivate))

struct _DinlePlaylistPrivate
{
    GList *tracks;
    DinlePlaylistType type;
};

static void _tracks_for_each_delete (gpointer data, gpointer user_data);

static void
dinle_playlist_get_property (GObject    *object,
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
dinle_playlist_set_property (GObject      *object,
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
dinle_playlist_dispose (GObject *object)
{
    G_OBJECT_CLASS (dinle_playlist_parent_class)->dispose (object);
}

static void
dinle_playlist_finalize (GObject *object)
{
    g_list_foreach (DINLE_PLAYLIST (object)->priv->tracks, _tracks_for_each_delete, NULL);
    g_list_free (DINLE_PLAYLIST (object)->priv->tracks);
    DINLE_PLAYLIST (object)->priv->tracks = NULL;

    G_OBJECT_CLASS (dinle_playlist_parent_class)->finalize (object);
}

static void
dinle_playlist_class_init (DinlePlaylistClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    g_type_class_add_private (klass, sizeof (DinlePlaylistPrivate));

    object_class->get_property = dinle_playlist_get_property;
    object_class->set_property = dinle_playlist_set_property;
    object_class->dispose = dinle_playlist_dispose;
    object_class->finalize = dinle_playlist_finalize;
}

static void
dinle_playlist_init (DinlePlaylist *self)
{
    self->priv = PLAYLIST_PRIVATE (self);
    self->priv->tracks = NULL;
    self->priv->type = DINLE_PLAYLIST_TYPE_UNKNOWN;
}

static void _tracks_for_each_delete (gpointer data, gpointer user_data)
{
    g_object_unref (G_OBJECT (data));
}

DinlePlaylist *
dinle_playlist_new (DinlePlaylistType type)
{
    DinlePlaylist *obj = g_object_new (DINLE_TYPE_PLAYLIST, NULL);
    obj->priv->type = type;
    return obj;
}

void
dinle_playlist_add (DinlePlaylist *playlist, DinleMediaFile *file)
{
    g_return_if_fail (DINLE_IS_PLAYLIST (playlist));
    DinlePlaylistPrivate *priv = PLAYLIST_PRIVATE (playlist);
    g_return_if_fail (DINLE_IS_MEDIA_FILE (file));

    priv->tracks = g_list_append (priv->tracks, (gpointer)file);
    g_object_ref (file);
}

gboolean
dinle_playlist_remove (DinlePlaylist *playlist, DinleMediaFile *file)
{
    g_return_val_if_fail (DINLE_IS_PLAYLIST (playlist), FALSE);
    DinlePlaylistPrivate *priv = PLAYLIST_PRIVATE (playlist);

    if (g_list_position (priv->tracks, (gpointer)file) == -1)
        return FALSE;

    priv->tracks = g_list_remove (priv->tracks, (gpointer) file);
    g_object_unref (file);

    return TRUE;
}

gchar *
dinle_playlist_client_reply (DinlePlaylist *playlist)
{
    g_return_val_if_fail (DINLE_IS_PLAYLIST (playlist), NULL);
    DinlePlaylistPrivate *priv = PLAYLIST_PRIVATE (playlist);

    const gchar *pltype;
    switch (priv->type) {
        case DINLE_PLAYLIST_TYPE_TAG:
            pltype = DINLE_PVALUE_SEARCH_TAG;
            break;
        case DINLE_PLAYLIST_TYPE_FREE:
            pltype = DINLE_PVALUE_SEARCH_FREE;
            break;
        default:
            g_warning ("Unknown playlist type...\n");
            return NULL;
    }

    GString *str = g_string_new ("");
    g_string_printf (str, DINLE_TAG_START (DINLE_REPLY_SEARCHRESULT, DINLE_PARAM_SEARCH_TYPE"='%s'")"\n", pltype);

    GList *iter = priv->tracks;
    while (iter) {
        DinleMediaFile *mf = DINLE_MEDIA_FILE (iter->data);
        DinleMediaMetadata *md = dinle_media_file_get_metadata (mf);

        GString *filetag = g_string_new ("");
        gchar *esc = g_markup_printf_escaped (DINLE_TAG_START (DINLE_REPLY_TRACK, DINLE_PARAM_TRACKID"='%s'")"\n",
                                              dinle_media_file_get_hash (mf));
        g_string_append (filetag, esc);
        g_free (esc);

        gchar **fvlist = dinle_media_metadata_get_field_value_list (md);
        gchar **fviter = fvlist;
        while (fviter && *fviter) {
            esc = g_markup_printf_escaped (DINLE_TAG_ALONE (DINLE_REPLY_TAG, "%s='%s'"), *fviter, *(fviter+1));
            g_string_append (filetag, esc);
            g_free (esc);
            fviter+=2;
        }
        g_strfreev (fvlist);

        g_string_append (filetag, DINLE_TAG_END (DINLE_REPLY_TRACK)"\n");
        g_string_append (str, filetag->str);
        g_string_free (filetag, TRUE);

        iter = iter->next;
    }

    g_string_append (str, DINLE_TAG_END ("search-result")"\n");

    gchar *s = str->str;
    g_string_free (str, FALSE);

    return s;
}
