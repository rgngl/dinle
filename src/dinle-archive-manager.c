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
#include "dinle-db-sqlite.h"

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
    DinleDb *db;
    guint files;
};

typedef void (*_traverse_callback) (const gchar* file, GType objtype,  gpointer data);

static void _initialize (void);
static void _build_database (void);
static void _update_database (void);
static void _traverse_directory (const gchar *path,  _traverse_callback cb, gpointer userdata);
static void _traverse_cb (const gchar *file, GType objtype, gpointer data);
static void _count_files_cb (const gchar *file, GType objtype, gpointer data);

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
    self->priv->db = NULL;
    self->priv->files = 0;
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
    DinleConfigManager *cm = dinle_config_manager_get ();

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

    GValue media_db_prop = {0,};
    g_value_init (&media_db_prop, G_TYPE_STRING);
    g_object_get_property (G_OBJECT (cm), "media-db", &media_db_prop);
    const gchar *media_db = g_value_get_string (&media_db_prop);

    GValue media_root_prop = {0,};
    g_value_init (&media_root_prop, G_TYPE_STRING);
    g_object_get_property (G_OBJECT (cm), "media-root", &media_root_prop);
    const gchar *media_root = g_value_get_string (&media_root_prop);

    priv->db = DINLE_DB (dinle_db_sqlite_new_with_name (media_db));
    priv->files = 0;
    _traverse_directory (media_root, _count_files_cb, NULL);
    g_print ("%d files found.\n", priv->files);

   g_value_unset (&media_db_prop);
}

static void
_build_database (void)
{
    g_return_if_fail (DINLE_IS_ARCHIVE_MANAGER (instance));
    DinleArchiveManagerPrivate *priv = ARCHIVE_MANAGER_PRIVATE (instance);
    DinleConfigManager *cm = dinle_config_manager_get ();

    GValue media_root_prop = {0,};
    g_value_init (&media_root_prop, G_TYPE_STRING);
    g_object_get_property (G_OBJECT (cm), "media-root", &media_root_prop);
    const gchar *media_root = g_value_get_string (&media_root_prop);

    guint scanned = 0;
    _traverse_directory (media_root, _traverse_cb, &scanned);
    g_value_unset (&media_root_prop);
}

static void
_update_database (void)
{
    g_return_if_fail (DINLE_IS_ARCHIVE_MANAGER (instance));
    DinleArchiveManagerPrivate *priv = ARCHIVE_MANAGER_PRIVATE (instance);
    DinleConfigManager *cm = dinle_config_manager_get ();

}

static void
_traverse_directory (const gchar *path, _traverse_callback cb, gpointer userdata)
{
    GError *error = NULL;
    const gchar *current = NULL;
    GDir* dir = g_dir_open (path, 0, &error);
    DinleArchiveManagerPrivate *priv = ARCHIVE_MANAGER_PRIVATE (instance);

    if (!dir)
        return;

    while (current = g_dir_read_name (dir)) {
        gchar *file = g_strconcat (path, "/", current, NULL);
        if (g_file_test (file, G_FILE_TEST_IS_DIR) && (current[0] != '.')) {
            _traverse_directory (file, cb, userdata);
        } else {
            GHashTableIter iter;
            gpointer key, objtype;
            g_hash_table_iter_init (&iter, priv->media_formats);

            while (g_hash_table_iter_next (&iter, &key, &objtype)) {
                gchar *ext = (gchar*)key;
                gchar *suffix = g_strconcat (".", ext, NULL);
                if (g_str_has_suffix (file, suffix)) {
                    if (cb)
                        cb (file, (GType)objtype, userdata);
                }
                g_free (suffix);
            }
        }
        g_free (file);
    }

    g_dir_close (dir);
}

static void
_traverse_cb (const gchar *file, GType objtype, gpointer data)
{
    g_return_if_fail (DINLE_IS_ARCHIVE_MANAGER (instance));
    DinleArchiveManagerPrivate *priv = ARCHIVE_MANAGER_PRIVATE (instance);

    DinleMediaFile *mf = g_object_new (objtype, NULL);
    if (! DINLE_IS_MEDIA_FILE (mf) ) {
        g_object_unref (mf);
        return;
    }

    dinle_media_file_set (mf, file);
    /*if (!dinle_db_add_file (priv->db, mf))*/
        /*g_warning ("couldn't add to db...\n");*/
    const DinleMediaMetadata *md = dinle_media_file_get_metadata (mf);
    const gchar *sum = dinle_media_file_get_hash (mf);
    const guint size = dinle_media_file_get_size (mf);
    guint *scanned = (guint*) data;
    (*scanned)++;
    /*g_print ("%s\t %s\t %d\t %s\n", file, sum, size, g_type_name (objtype));*/
    g_print ("%d of %d scanned                        \r", *scanned, priv->files);
}

static void
_count_files_cb (const gchar *file, GType objtype, gpointer data)
{
    g_return_if_fail (DINLE_IS_ARCHIVE_MANAGER (instance));
    DinleArchiveManagerPrivate *priv = ARCHIVE_MANAGER_PRIVATE (instance);

    priv->files++;
}

DinleArchiveManager *
dinle_archive_manager_get (void)
{
    if (instance == NULL || !DINLE_IS_ARCHIVE_MANAGER(instance)) {
        instance = dinle_archive_manager_new ();
        DinleArchiveManagerPrivate *priv = ARCHIVE_MANAGER_PRIVATE (instance);
        _initialize ();
        if (dinle_db_file_count (priv->db) == 0) {
            _build_database ();
        } else {
            _update_database ();
        }
    }

    return instance;
}
