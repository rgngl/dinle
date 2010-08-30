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

#include "dinle-archive-manager.h"
#include "dinle-config-manager.h"
#include "dinle-media-file.h"
#include "dinle-db-sqlite.h"
#include "dinle-tags.h"

#ifdef HAVE_MP3
#include "dinle-media-file-mp3.h"
#endif

#include <stdlib.h>

G_DEFINE_TYPE (DinleArchiveManager, dinle_archive_manager, G_TYPE_OBJECT)

#define ARCHIVE_MANAGER_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), DINLE_TYPE_ARCHIVE_MANAGER, DinleArchiveManagerPrivate))

static DinleArchiveManager *instance = NULL;

struct _DinleArchiveManagerPrivate
{
    DinleDb *db;
    guint files;
};

typedef void (*_traverse_callback) (const gchar* file, gpointer data);

static void _initialize (void);
static void _build_database (void);
static void _update_database (void);
static void _traverse_directory (const gchar *path,  _traverse_callback cb, gpointer userdata);
static void _traverse_cb (const gchar *file, gpointer data);
static void _count_files_cb (const gchar *file, gpointer data);
static void _get_from_db_cb (const gchar *file, gpointer data);
static void _add_new_files_cb (const gchar *file, gpointer data);

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
    g_print ("Destroying archive manager instance.\n");
    DinleArchiveManagerPrivate *priv = ARCHIVE_MANAGER_PRIVATE (instance);

    if (priv->db)
        g_object_unref (priv->db);

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
    DinleArchiveManagerPrivate *priv = ARCHIVE_MANAGER_PRIVATE (instance);
    DinleConfigManager *cm = dinle_config_manager_get ();
    dinle_media_file_initialize ();

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

    GValue media_root_prop = {0,};
    g_value_init (&media_root_prop, G_TYPE_STRING);
    g_object_get_property (G_OBJECT (cm), "media-root", &media_root_prop);
    const gchar *media_root = g_value_get_string (&media_root_prop);

    /* check for files that no longer exist and remove them. */
    g_print ("checking non-existent files.\n");
    gchar **file_list = dinle_db_get_files (priv->db);
    int i = 0;
    while (file_list && file_list[i]) {
        if (!g_file_test (file_list[i], G_FILE_TEST_EXISTS)) {
            dinle_db_remove_file (priv->db, file_list[i]);
        }
        i++;
    }
    g_strfreev (file_list);

    /* add recently appeared files to the db. */
    g_print ("adding recently added tracks to the db.\n");
    _traverse_directory (media_root, _add_new_files_cb, NULL);

    /* check for size change in existent files. */
    g_print ("checking changes in known files.\n");
    _traverse_directory (media_root, _get_from_db_cb, NULL);

    g_value_unset (&media_root_prop);
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
            if (dinle_is_file_supported (file)) {
                if (cb)
                    cb (file, userdata);
            }
        }
        g_free (file);
    }

    g_dir_close (dir);
}

static void
_traverse_cb (const gchar *file, gpointer data)
{
    g_return_if_fail (DINLE_IS_ARCHIVE_MANAGER (instance));
    DinleArchiveManagerPrivate *priv = ARCHIVE_MANAGER_PRIVATE (instance);

    DinleMediaFile *mf = dinle_media_file_new (file);
    if (! DINLE_IS_MEDIA_FILE (mf) ) {
        return;
    }

    dinle_media_file_set (mf, file);
    if (!dinle_db_add_file (priv->db, mf))
        g_warning ("couldn't add to db...\n");
    const DinleMediaMetadata *md = dinle_media_file_get_metadata (mf);
    const gchar *sum = dinle_media_file_get_hash (mf);
    const guint size = dinle_media_file_get_size (mf);
    guint *scanned = (guint*) data;
    (*scanned)++;
    /*g_print ("%s\t %s\t %d\t %s\n", file, sum, size, g_type_name (objtype));*/
    g_print ("%d of %d scanned                        \r", *scanned, priv->files);
    g_object_unref (mf);
}

static void
_count_files_cb (const gchar *file, gpointer data)
{
    g_return_if_fail (DINLE_IS_ARCHIVE_MANAGER (instance));
    DinleArchiveManagerPrivate *priv = ARCHIVE_MANAGER_PRIVATE (instance);

    priv->files++;
}

static void
_get_from_db_cb (const gchar *file, gpointer data)
{
    g_return_if_fail (DINLE_IS_ARCHIVE_MANAGER (instance));
    DinleArchiveManagerPrivate *priv = ARCHIVE_MANAGER_PRIVATE (instance);

    DinleMediaFile *mf = dinle_db_get_file_by_name (priv->db, file);
    DinleMediaFile *mf2 = dinle_media_file_new (file);
    if (dinle_media_file_get_size (mf) == dinle_media_file_get_size (mf2)) {
        /* if the file size haven't changed, do nothing. */
        /*g_print ("size of the file matches the one in the database.\n");*/
    } else {
        dinle_db_remove_file (priv->db, dinle_media_file_get_path (mf));
        dinle_db_add_file (priv->db, mf2);
    }

    if (mf)
        g_object_unref (G_OBJECT (mf));
    g_object_unref (mf2);
}

static void
_add_new_files_cb (const gchar *file, gpointer data)
{
    g_return_if_fail (DINLE_IS_ARCHIVE_MANAGER (instance));
    DinleArchiveManagerPrivate *priv = ARCHIVE_MANAGER_PRIVATE (instance);

    if (!dinle_db_file_exists (priv->db, file)) {
        DinleMediaFile *mf = dinle_media_file_new (file);
        if (!mf)
            return;

        if (!dinle_db_add_file (priv->db, mf))
            g_warning ("couldn't add to db...\n");
        g_object_unref (mf);
    }
}

DinleArchiveManager *
dinle_archive_manager_get (void)
{
    if (instance == NULL || !DINLE_IS_ARCHIVE_MANAGER(instance)) {
        instance = dinle_archive_manager_new ();
        DinleArchiveManagerPrivate *priv = ARCHIVE_MANAGER_PRIVATE (instance);
        _initialize ();
        if (dinle_db_file_count (priv->db) <= 0) {
            g_print("building db\n");
            _build_database ();
        } else {
            g_print ("updating db, it has %d files.\n", dinle_db_file_count (priv->db));
            _update_database ();
        }
        dinle_db_search_by_tags (priv->db,
                                 DINLE_TAG_ARTIST, "Kurban",
                                 DINLE_TAG_ALBUM, "Sahip",
                                 NULL);
    }

    return instance;
}
