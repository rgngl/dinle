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
/* dinle-media-file.c */

#include "dinle-media-file.h"

G_DEFINE_TYPE (DinleMediaFile, dinle_media_file, G_TYPE_OBJECT)

#define MEDIA_FILE_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), DINLE_TYPE_MEDIA_FILE, DinleMediaFilePrivate))

struct _DinleMediaFilePrivate
{
    gchar *file;
    DinleMediaMetadata *md;
};


static void
dinle_media_file_get_property (GObject    *object,
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
dinle_media_file_set_property (GObject      *object,
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
dinle_media_file_dispose (GObject *object)
{
    G_OBJECT_CLASS (dinle_media_file_parent_class)->dispose (object);
}

static void
dinle_media_file_finalize (GObject *object)
{
    g_return_if_fail (DINLE_IS_MEDIA_FILE (object));
    DinleMediaFilePrivate *priv = MEDIA_FILE_PRIVATE (object);

    if (priv->md)
        g_object_unref (priv->md);
    g_free (priv->file);

    G_OBJECT_CLASS (dinle_media_file_parent_class)->finalize (object);
}

static void
dinle_media_file_class_init (DinleMediaFileClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    g_type_class_add_private (klass, sizeof (DinleMediaFilePrivate));

    object_class->get_property = dinle_media_file_get_property;
    object_class->set_property = dinle_media_file_set_property;
    object_class->dispose = dinle_media_file_dispose;
    object_class->finalize = dinle_media_file_finalize;
    klass->extensions = NULL;
    klass->get_metadata_file = NULL;
}

static void
dinle_media_file_init (DinleMediaFile *self)
{
    self->priv = MEDIA_FILE_PRIVATE (self);

    self->priv->md = NULL;
    self->priv->file = NULL;
}

DinleMediaFile *
dinle_media_file_new (void)
{
    return g_object_new (DINLE_TYPE_MEDIA_FILE, NULL);
}

const gchar *
dinle_media_file_extensions(DinleMediaFile *self)
{
    g_return_val_if_fail (DINLE_IS_MEDIA_FILE (self), NULL);

    if (DINLE_MEDIA_FILE_GET_CLASS (self)->extensions) {
        return (DINLE_MEDIA_FILE_GET_CLASS (self)->extensions)(self);
    }

    g_critical ("Pure virtual _extensions method called...\n");

    return NULL;
}

gboolean
dinle_media_file_set (DinleMediaFile *self, const gchar *file)
{
    g_return_val_if_fail (DINLE_IS_MEDIA_FILE (self), FALSE);
    DinleMediaFilePrivate *priv = MEDIA_FILE_PRIVATE (self);

    if (!g_file_test (file, G_FILE_TEST_EXISTS))
        return FALSE;

    priv->file = g_strdup (file);
    return TRUE;
}

const DinleMediaMetadata *
dinle_media_file_get_metadata (DinleMediaFile *self)
{
    g_return_val_if_fail (DINLE_IS_MEDIA_FILE (self), NULL);
    DinleMediaFilePrivate *priv = MEDIA_FILE_PRIVATE (self);

    if (DINLE_MEDIA_FILE_GET_CLASS (self)->get_metadata_file) {
        return (DINLE_MEDIA_FILE_GET_CLASS (self)->get_metadata_file)(self, priv->file);
    }

    g_critical ("Pure virtual class get_metadata method called.\n");
    return NULL;
}