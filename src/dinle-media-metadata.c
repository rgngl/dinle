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

G_DEFINE_TYPE (DinleMediaMetadata, dinle_media_metadata, G_TYPE_OBJECT)

#define MEDIA_METADATA_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), DINLE_TYPE_MEDIA_METADATA, DinleMediaMetadataPrivate))

struct _DinleMediaMetadataPrivate
{
    gchar *artist;
    gchar *album;
    gchar *title;
    gchar *genre;
    guint length;
};

enum {
    PROP_0,

    PROP_ARTIST,
    PROP_ALBUM,
    PROP_TITLE,
    PROP_GENRE,
    PROP_LENGTH,

    PROP_NUMBER
};

static void
dinle_media_metadata_get_property (GObject    *object,
                                   guint       property_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
    g_return_if_fail (DINLE_IS_MEDIA_METADATA (object));

    DinleMediaMetadataPrivate *priv = MEDIA_METADATA_PRIVATE (object);

    switch (property_id)
    {
        case PROP_ARTIST:
            g_value_set_string (value, priv->artist);
            break;
        case PROP_ALBUM:
            g_value_set_string (value, priv->album);
            break;
        case PROP_TITLE:
            g_value_set_string (value, priv->title);
            break;
        case PROP_GENRE:
            g_value_set_string (value, priv->genre);
            break;
        case PROP_LENGTH:
            g_value_set_uint (value, priv->length);
            break;
        default:
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

    switch (property_id)
    {
        case PROP_ARTIST:
            g_free (priv->artist);
            priv->artist = g_value_dup_string (value);
            break;
        case PROP_ALBUM:
            g_free (priv->album);
            priv->album = g_value_dup_string (value);
            break;
        case PROP_TITLE:
            g_free (priv->title);
            priv->title = g_value_dup_string (value);
            break;
        case PROP_GENRE:
            g_free (priv->genre);
            priv->genre = g_value_dup_string (value);
            break;
        case PROP_LENGTH:
            priv->length = g_value_get_uint (value);
            break;
        default:
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

    g_free (priv->artist);
    g_free (priv->album);
    g_free (priv->title);
    g_free (priv->genre);

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

    pspec = g_param_spec_uint ("length",
            "Length",
            "Length in seconds",
            0,
            G_MAXUINT,
            0,
            G_PARAM_WRITABLE | G_PARAM_READABLE);
    g_object_class_install_property (object_class,
            PROP_LENGTH,
            pspec);

}

static void
dinle_media_metadata_init (DinleMediaMetadata *self)
{
    self->priv = MEDIA_METADATA_PRIVATE (self);

    self->priv->artist = NULL;
    self->priv->album = NULL;
    self->priv->title = NULL;
    self->priv->genre = NULL;
    self->priv->length = 0;
}

DinleMediaMetadata *
dinle_media_metadata_new (void)
{
    return g_object_new (DINLE_TYPE_MEDIA_METADATA, NULL);
}
