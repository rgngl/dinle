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
/* dinle-media-metadata.c */

#include "dinle-media-metadata.h"
#include "dinle-tags.h"

G_DEFINE_TYPE (DinleMediaMetadata, dinle_media_metadata, G_TYPE_OBJECT)

#define MEDIA_METADATA_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), DINLE_TYPE_MEDIA_METADATA, DinleMediaMetadataPrivate))

struct _DinleMediaMetadataPrivate
{
    GHashTable *fields;
};

enum {
    PROP_0,

    PROP_ARTIST,
    PROP_ALBUM,
    PROP_TITLE,
    PROP_GENRE,
    PROP_LENGTH,
    PROP_YEAR,
    PROP_TRACK,
    PROP_TRACKS,
    PROP_DISCNO,
    PROP_DISCS,

    PROP_NUMBER
};

static const gchar* prop_names [] = {
    "0",

    DINLE_TAG_ARTIST,
    DINLE_TAG_ALBUM,
    DINLE_TAG_TITLE,
    DINLE_TAG_GENRE,
    DINLE_TAG_LENGTH,
    DINLE_TAG_YEAR,
    DINLE_TAG_TRACK,
    DINLE_TAG_TRACKS,
    DINLE_TAG_DISCNO,
    DINLE_TAG_DISCS,

    NULL
};

static void
dinle_media_metadata_get_property (GObject    *object,
                                   guint       property_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
    g_return_if_fail (DINLE_IS_MEDIA_METADATA (object));

    DinleMediaMetadataPrivate *priv = MEDIA_METADATA_PRIVATE (object);

    gchar *field = NULL;

    if (property_id > PROP_0 && property_id < PROP_NUMBER) {
            field = g_hash_table_lookup (priv->fields, prop_names [property_id]);
            g_value_set_string (value, field);
    } else {
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
dinle_media_metadata_set_property (GObject      *object,
        guint         property_id,
        const GValue *value,
        GParamSpec   *pspec)
{
    g_return_if_fail (DINLE_IS_MEDIA_METADATA (object));

    DinleMediaMetadataPrivate *priv = MEDIA_METADATA_PRIVATE (object);

    gchar *field = NULL;

    if ( property_id > PROP_0 && property_id < PROP_NUMBER) {
        field = g_hash_table_lookup (priv->fields, prop_names [property_id]);
        g_hash_table_remove (priv->fields, prop_names [property_id]);
        g_free (field);
        if (!g_value_get_string (value))
            return;
        g_hash_table_insert (priv->fields, (gpointer) prop_names [property_id],
                g_value_dup_string (value));
    } else {
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
dinle_media_metadata_dispose (GObject *object)
{
    G_OBJECT_CLASS (dinle_media_metadata_parent_class)->dispose (object);
}

static void
dinle_media_metadata_finalize (GObject *object)
{
    g_return_if_fail (DINLE_IS_MEDIA_METADATA (object));

    DinleMediaMetadataPrivate *priv = MEDIA_METADATA_PRIVATE (object);

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init (&iter, priv->fields);

    while (g_hash_table_iter_next (&iter, &key, &value)) {
        g_free (value);
    }
    g_hash_table_destroy (priv->fields);

    G_OBJECT_CLASS (dinle_media_metadata_parent_class)->finalize (object);
}

static void
dinle_media_metadata_class_init (DinleMediaMetadataClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    GParamSpec *pspec = NULL;

    g_type_class_add_private (klass, sizeof (DinleMediaMetadataPrivate));

    object_class->get_property = dinle_media_metadata_get_property;
    object_class->set_property = dinle_media_metadata_set_property;
    object_class->dispose = dinle_media_metadata_dispose;
    object_class->finalize = dinle_media_metadata_finalize;

    pspec = g_param_spec_string ("artist",
            "Artist Name",
            "Artist Name",
            NULL /* default value */,
            G_PARAM_WRITABLE | G_PARAM_READABLE);
    g_object_class_install_property (object_class,
            PROP_ARTIST,
            pspec);

    pspec = g_param_spec_string ("album",
            "Album Name",
            "Album Name",
            NULL /* default value */,
            G_PARAM_WRITABLE | G_PARAM_READABLE);
    g_object_class_install_property (object_class,
            PROP_ALBUM,
            pspec);

    pspec = g_param_spec_string ("title",
            "Title Name",
            "Title Name",
            NULL /* default value */,
            G_PARAM_WRITABLE | G_PARAM_READABLE);
    g_object_class_install_property (object_class,
            PROP_TITLE,
            pspec);

    pspec = g_param_spec_string ("genre",
            "Genre Name",
            "Genre Name",
            NULL /* default value */,
            G_PARAM_WRITABLE | G_PARAM_READABLE);
    g_object_class_install_property (object_class,
            PROP_GENRE,
            pspec);

    pspec = g_param_spec_string ("length",
            "Length",
            "Length in milliseconds",
            NULL,
            G_PARAM_WRITABLE | G_PARAM_READABLE);
    g_object_class_install_property (object_class,
            PROP_LENGTH,
            pspec);

    pspec = g_param_spec_string ("year",
            "Year",
            "Year",
            NULL,
            G_PARAM_WRITABLE | G_PARAM_READABLE);
    g_object_class_install_property (object_class,
            PROP_YEAR,
            pspec);

    pspec = g_param_spec_string ("track",
            "Track",
            "Track",
            NULL,
            G_PARAM_WRITABLE | G_PARAM_READABLE);
    g_object_class_install_property (object_class,
            PROP_TRACK,
            pspec);

    pspec = g_param_spec_string ("tracks",
            "Tracks",
            "Number of Tracks",
            NULL,
            G_PARAM_WRITABLE | G_PARAM_READABLE);
    g_object_class_install_property (object_class,
            PROP_TRACKS,
            pspec);

    pspec = g_param_spec_string ("discno",
            "Disc No",
            "Disc No",
            NULL,
            G_PARAM_WRITABLE | G_PARAM_READABLE);
    g_object_class_install_property (object_class,
            PROP_DISCNO,
            pspec);

    pspec = g_param_spec_string ("discs",
            "Discs",
            "Discs",
            NULL,
            G_PARAM_WRITABLE | G_PARAM_READABLE);
    g_object_class_install_property (object_class,
            PROP_DISCS,
            pspec);
}

static void
dinle_media_metadata_init (DinleMediaMetadata *self)
{
    self->priv = MEDIA_METADATA_PRIVATE (self);

    self->priv->fields = g_hash_table_new (g_str_hash, g_str_equal);
}

DinleMediaMetadata *
dinle_media_metadata_new (void)
{
    return g_object_new (DINLE_TYPE_MEDIA_METADATA, NULL);
}

gchar **
dinle_media_metadata_get_field_value_list (DinleMediaMetadata *md)
{
    g_return_val_if_fail (DINLE_IS_MEDIA_METADATA (md), NULL);
    DinleMediaMetadataPrivate *priv = MEDIA_METADATA_PRIVATE (md);

    gchar **list = g_malloc0 (sizeof (gchar*) * g_hash_table_size (priv->fields) * 2 + 1);

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init (&iter, priv->fields);

    gint i = 0;
    while (g_hash_table_iter_next (&iter, &key, &value)) {
        list[i] = g_strdup (key);
        list[i+1] = g_strdup (value);
        i += 2;
    }

    return list;
}
