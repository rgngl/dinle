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
#include "config.h"

#ifdef HAVE_MP3
#include "dinle-media-file-mp3.h"
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

G_DEFINE_TYPE (DinleMediaFile, dinle_media_file, G_TYPE_OBJECT)

#define MEDIA_FILE_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), DINLE_TYPE_MEDIA_FILE, DinleMediaFilePrivate))


#define HASH_BUF_SIZE (1024*1024)

struct _DinleMediaFilePrivate
{
    gchar *file;
    gchar *hash;
    DinleMediaMetadata *md;
    guint size;
};

static GHashTable *media_formats = NULL;

static void _unset (DinleMediaFile *self);


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
dinle_media_file_new (const gchar *file)
{
    if (!media_formats)
        return NULL;

    const gchar *suffix = g_strrstr (file, ".");
    suffix++;

    if (!suffix)
        return NULL;

    gpointer t = g_hash_table_lookup (media_formats, suffix);
    if (!t)
        return NULL;

    GType type = (GType) t;
    DinleMediaFile *mf = g_object_new (type, NULL);
    dinle_media_file_set (mf, file);
    return mf;
}

void
dinle_media_file_initialize (void)
{
    if (!media_formats) {
        media_formats = g_hash_table_new (g_str_hash, g_str_equal);
    }

    gboolean mf_found = FALSE;

    /*Add supported media file extensions.*/
#ifdef HAVE_MP3
    DinleMediaFile *mp3file = DINLE_MEDIA_FILE (dinle_media_file_mp3_new ());
    const gchar *mp3ext = dinle_media_file_extensions (DINLE_MEDIA_FILE (mp3file));
    gchar **mp3extensions = g_strsplit (mp3ext, " ", 0);
    int i = 0;
    while (mp3extensions[i]) {
        g_hash_table_insert (media_formats,
                             g_strdup(mp3extensions[i]), (gpointer)G_OBJECT_TYPE (mp3file));
        g_print ("extension added: %s %s\n", mp3extensions[i], G_OBJECT_TYPE_NAME (mp3file));
        i++;
    }
    g_strfreev (mp3extensions);
    g_object_unref (G_OBJECT (mp3file));
    mf_found = TRUE;
#endif

    if (!mf_found) {
        g_error ("No media file plugin enabled... Quitting.\n");
        abort();
    }
}

gboolean
dinle_is_file_supported (const gchar *file)
{
    if (!media_formats)
        return FALSE;

    const gchar *suffix = g_strrstr (file, ".");

    if (!suffix)
        return FALSE;

    suffix++;

    return (g_hash_table_lookup (media_formats, suffix) != NULL);
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

    _unset (self);

    if (!g_file_test (file, G_FILE_TEST_EXISTS))
        return FALSE;

    priv->file = g_strdup (file);
    return TRUE;
}

void
dinle_media_file_set_hash_size (DinleMediaFile *self, const gchar *hash, guint size)
{
    g_return_val_if_fail (DINLE_IS_MEDIA_FILE (self), FALSE);
    DinleMediaFilePrivate *priv = MEDIA_FILE_PRIVATE (self);

    g_free (priv->hash);
    priv->hash = g_strdup (hash);

    priv->size = size;
}

const gchar *
dinle_media_file_get_path (DinleMediaFile *self)
{
    g_return_val_if_fail (DINLE_IS_MEDIA_FILE (self), NULL);
    DinleMediaFilePrivate *priv = MEDIA_FILE_PRIVATE (self);

    return priv->file;
}

static void
_unset (DinleMediaFile *self)
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
    if (priv->md) {
        g_object_unref (priv->md);
        priv->md = NULL;
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
        priv->md = DINLE_MEDIA_FILE_GET_CLASS (self)->get_metadata_file(self, priv->file);
        return priv->md;
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
