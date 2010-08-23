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

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

G_DEFINE_TYPE (DinleMediaFile, dinle_media_file, G_TYPE_OBJECT)

#define MEDIA_FILE_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), DINLE_TYPE_MEDIA_FILE, DinleMediaFilePrivate))


#define HASH_BUF_SIZE (1024)

struct _DinleMediaFilePrivate
{
    gchar *file;
    gchar *hash;
    DinleMediaMetadata *md;
    guint size;
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
    g_free (priv->hash);

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
    self->priv->hash = NULL;
    self->priv->size = NULL;
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

void
dinle_media_file_unset (DinleMediaFile *self)
{
    g_return_if_fail (DINLE_IS_MEDIA_FILE (self));
    DinleMediaFilePrivate *priv = MEDIA_FILE_PRIVATE (self);

    if (priv->file) {
        g_free (priv->file);
        priv->file = NULL;
    }
    if (priv->hash) {
        g_free (priv->hash);
        priv->hash = NULL;
    }

    priv->size = 0;
}

const DinleMediaMetadata *
dinle_media_file_get_metadata (DinleMediaFile *self)
{
    g_return_val_if_fail (DINLE_IS_MEDIA_FILE (self), NULL);
    DinleMediaFilePrivate *priv = MEDIA_FILE_PRIVATE (self);

    if (priv->md)
        return priv->md;

    if (DINLE_MEDIA_FILE_GET_CLASS (self)->get_metadata_file) {
        return (DINLE_MEDIA_FILE_GET_CLASS (self)->get_metadata_file)(self, priv->file);
    }

    g_critical ("Pure virtual class get_metadata method called.\n");
    return NULL;
}

const gchar *
dinle_media_file_get_hash (DinleMediaFile *self)
{
    guchar data[HASH_BUF_SIZE] = {0,};
    DinleMediaFilePrivate *priv = NULL;
    GChecksum *cs = NULL;
    FILE *input = NULL;
    gsize size = 0;
    const gchar *sum;

    g_return_val_if_fail (DINLE_IS_MEDIA_FILE (self), NULL);
    priv = MEDIA_FILE_PRIVATE (self);

    if (priv->hash)
        return priv->hash;

    g_return_val_if_fail (priv->file, NULL);

    cs = g_checksum_new (G_CHECKSUM_MD5);
    input = fopen (priv->file, "rb");
    do {
        size = fread( (void *)data, sizeof( guchar ), HASH_BUF_SIZE, input );
        g_checksum_update( cs, data, size );
    } while (size == HASH_BUF_SIZE);
    fclose (input);
    sum = g_checksum_get_string (cs);
    priv->hash = g_strdup (sum);

    return sum;
}

guint
dinle_media_file_get_size (DinleMediaFile *self)
{
    DinleMediaFilePrivate *priv = NULL;

    g_return_val_if_fail (DINLE_IS_MEDIA_FILE (self), 0);
    priv = MEDIA_FILE_PRIVATE (self);

    if (priv->size > 0)
        return priv->size;

    g_return_val_if_fail (priv->file, 0);

    struct stat st;
    stat (priv->file, &st);

    priv->size = st.st_size;

    return (guint)st.st_size;
}
