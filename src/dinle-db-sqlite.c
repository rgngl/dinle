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
/* dinle-db-sqlite.c */

#include "dinle-db-sqlite.h"

#include <sqlite3.h>

G_DEFINE_TYPE (DinleDbSqlite, dinle_db_sqlite, DINLE_TYPE_DB)

#define DB_SQLITE_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), DINLE_TYPE_DB_SQLITE, DinleDbSqlitePrivate))


#define FILES_TABLE "files"
#define FILES_TABLE_CREATE "CREATE TABLE " FILES_TABLE \
                           "(hash TEXT PRIMARY KEY, " \
                           " size INTEGER, path TEXT);"
#define FILES_TABLE_ADD "INSERT INTO \"" FILES_TABLE "\" "\
                        "VALUES ('%s', %d, '%s');"

#define TABLE_CHECK_QUERY "SELECT name FROM sqlite_master WHERE type='table' AND name='%s';"

struct _DinleDbSqlitePrivate
{
    sqlite3 *db;
};

static gboolean _set_db (DinleDb *db, gchar *name);
static gboolean _add_file (DinleDb *db, DinleMediaFile *file);

static gboolean _check_create_table (DinleDbSqlite *db, const gchar *table_name);


/******************************************************************************/

static gboolean
_check_create_table (DinleDbSqlite *db, const gchar *table_name)
{
    g_return_val_if_fail (DINLE_IS_DB_SQLITE (db), FALSE);
    DinleDbSqlitePrivate *priv = DB_SQLITE_PRIVATE (db);
    g_return_val_if_fail (priv->db, FALSE);

    gchar **result = NULL;
    gint rows = 0, columns = 0;

    gchar *query = g_strdup_printf (TABLE_CHECK_QUERY, table_name);

    int success = sqlite3_get_table (priv->db, query, &result, &rows, &columns, NULL);
    g_print ("rows: %d, cols: %d\n", rows, columns);

    sqlite3_free_table (result);
    g_free (query);

    return (success == SQLITE_OK);
}

static void
dinle_db_sqlite_get_property (GObject    *object,
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
dinle_db_sqlite_set_property (GObject      *object,
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
dinle_db_sqlite_dispose (GObject *object)
{
    G_OBJECT_CLASS (dinle_db_sqlite_parent_class)->dispose (object);
}

static void
dinle_db_sqlite_finalize (GObject *object)
{
    G_OBJECT_CLASS (dinle_db_sqlite_parent_class)->finalize (object);
}

static void
dinle_db_sqlite_class_init (DinleDbSqliteClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    DinleDbClass *parent_class = DINLE_DB_CLASS (klass);

    g_type_class_add_private (klass, sizeof (DinleDbSqlitePrivate));

    object_class->get_property = dinle_db_sqlite_get_property;
    object_class->set_property = dinle_db_sqlite_set_property;
    object_class->dispose = dinle_db_sqlite_dispose;
    object_class->finalize = dinle_db_sqlite_finalize;
    parent_class->set_db = _set_db;
    parent_class->add_file = _add_file;
}

static void
dinle_db_sqlite_init (DinleDbSqlite *self)
{
    self->priv = DB_SQLITE_PRIVATE (self);
    self->priv->db = NULL;
}

static gboolean
_set_db (DinleDb *db, gchar *name)
{
    g_return_val_if_fail (DINLE_IS_DB_SQLITE (db), FALSE);
    DinleDbSqlitePrivate *priv = DB_SQLITE_PRIVATE (db);

    int result = sqlite3_open (name, &(priv->db));

    return ( (result == SQLITE_OK) && (priv->db != NULL) );
}

static gboolean
_add_file (DinleDb *db, DinleMediaFile *file)
{
    g_return_val_if_fail (DINLE_IS_DB_SQLITE (db), FALSE);
    DinleDbSqlitePrivate *priv = DB_SQLITE_PRIVATE (db);
    g_return_val_if_fail (priv->db, FALSE);

    gchar **table;
    gint rows, columns;

    if (!_check_create_table (DINLE_DB_SQLITE (db), FILES_TABLE))
        return FALSE;

    gchar *query = g_strdup_printf (FILES_TABLE_ADD,
                                    dinle_media_file_get_hash (file),
                                    dinle_media_file_get_size (file),
                                    dinle_media_file_get_path (file));

    int result = sqlite3_get_table (priv->db, query, &table, &rows, &columns, NULL);

    return (result == SQLITE_OK);
}

DinleDb *
dinle_db_sqlite_new (void)
{
    DinleDbSqlite* obj = g_object_new (DINLE_TYPE_DB_SQLITE, NULL);

    return DINLE_DB (obj);
}

DinleDb *
dinle_db_sqlite_new_with_name (gchar *name)
{
    DinleDbSqlite *db = g_object_new (DINLE_TYPE_DB_SQLITE, NULL);

    if (name && *name)
        dinle_db_set_db (DINLE_DB (db), name);

    return DINLE_DB (db);
}
