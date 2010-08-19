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
/* dinle-archive-manager.c */

#include "config.h"

#include <stdlib.h>

#include "dinle-archive-manager.h"
#include "dinle-config-manager.h"
#include "dinle-media-file.h"

#ifdef HAVE_MP3
#include "dinle-media-file-mp3.h"
#endif

G_DEFINE_TYPE (DinleArchiveManager, dinle_archive_manager, G_TYPE_OBJECT)

#define ARCHIVE_MANAGER_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), DINLE_TYPE_ARCHIVE_MANAGER, DinleArchiveManagerPrivate))

static DinleArchiveManager *instance = NULL;

struct _DinleArchiveManagerPrivate
{
    GHashTable *media_formats;
};

typedef void (*_traverse_callback) (const gchar* file);

static void _initialize (void);
static void _update_database (void);
static void _traverse_directory (const gchar *path, const gchar *ext, _traverse_callback cb);
static void _traverse_cb (const gchar *file);
static void _media_format_action (gpointer key, gpointer value, gpointer userdata);

static void
dinle_archive_manager_get_property (GObject    *object,
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
dinle_archive_manager_set_property (GObject      *object,
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
dinle_archive_manager_dispose (GObject *object)
{
    G_OBJECT_CLASS (dinle_archive_manager_parent_class)->dispose (object);
}

static void
dinle_archive_manager_finalize (GObject *object)
{
    G_OBJECT_CLASS (dinle_archive_manager_parent_class)->finalize (object);
}

static void
dinle_archive_manager_class_init (DinleArchiveManagerClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    g_type_class_add_private (klass, sizeof (DinleArchiveManagerPrivate));

    object_class->get_property = dinle_archive_manager_get_property;
    object_class->set_property = dinle_archive_manager_set_property;
    object_class->dispose = dinle_archive_manager_dispose;
    object_class->finalize = dinle_archive_manager_finalize;
}

static void
dinle_archive_manager_init (DinleArchiveManager *self)
{
    self->priv = ARCHIVE_MANAGER_PRIVATE (self);

    self->priv->media_formats = g_hash_table_new (g_str_hash, g_str_equal);
}

static DinleArchiveManager *
dinle_archive_manager_new (void)
{
    return g_object_new (DINLE_TYPE_ARCHIVE_MANAGER, NULL);
}

static void
_initialize (void)
{
    gboolean mf_found = FALSE;
    DinleArchiveManagerPrivate *priv = ARCHIVE_MANAGER_PRIVATE (instance);

    /*Add supported media file extensions.*/
#ifdef HAVE_MP3
    DinleMediaFile *mp3file = DINLE_MEDIA_FILE (dinle_media_file_mp3_new ());
    const gchar *mp3ext = dinle_media_file_extensions (DINLE_MEDIA_FILE (mp3file));
    gchar **mp3extensions = g_strsplit (mp3ext, " ", 0);
    int i = 0;
    while (mp3extensions[i]) {
        g_hash_table_insert (priv->media_formats,
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

static void
_update_database (void)
{
    DinleArchiveManagerPrivate *priv = ARCHIVE_MANAGER_PRIVATE (instance);
    DinleConfigManager *cm = dinle_config_manager_get ();

    GValue media_root_prop = {0,};
    g_value_init (&media_root_prop, G_TYPE_STRING);
    g_object_get_property (G_OBJECT (cm), "media-root", &media_root_prop);
    const gchar *media_root = g_value_get_string (&media_root_prop);

    g_hash_table_foreach (priv->media_formats,
                          _media_format_action,
                          (gpointer)media_root);

    g_value_unset (&media_root_prop);
}

static void
_traverse_directory (const gchar *path, const gchar *ext, _traverse_callback cb)
{
    GError *error = NULL;
    const gchar *current = NULL;
    GDir* dir = g_dir_open (path, 0, &error);

    if (!dir)
        return;

    while (current = g_dir_read_name (dir)) {
        if (g_file_test (current, G_FILE_TEST_IS_DIR)) {
            gchar *newdir = g_strconcat (path, "/", current, NULL);
            _traverse_directory (newdir, ext, NULL);
            g_free (newdir);
        } else {
            gchar *suffix = g_strconcat (".", ext, NULL);
            if (g_str_has_suffix (current, suffix)) {
                gchar *file = g_strconcat (path, "/", current, NULL);
                if (cb)
                    cb (file);
                g_free (file);
            }
            g_free (suffix);
        }
    }

    g_dir_close (dir);
}

static void _traverse_cb (const gchar *file)
{
    g_print ("%s\n", file);
    DinleMediaFile *mf = dinle_media_file_mp3_new ();
    dinle_media_file_set (mf, file);
    const DinleMediaMetadata *md = dinle_media_file_get_metadata (mf);
}

static void _media_format_action (gpointer key, gpointer value, gpointer userdata)
{
    /*g_print ("media format action %s %s %s\n",*/
             /*(const gchar*)userdata,*/
             /*(const gchar*)key,*/
             /*g_type_name ((GType)value));*/
    _traverse_directory ((const gchar*)userdata, (const gchar*)key, _traverse_cb);
}

DinleArchiveManager *
dinle_archive_manager_get (void)
{
    if (instance == NULL || !DINLE_IS_ARCHIVE_MANAGER(instance)) {
        instance = dinle_archive_manager_new ();
        _initialize ();
        _update_database ();
    }

    return instance;
}