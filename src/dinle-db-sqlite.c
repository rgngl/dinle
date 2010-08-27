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
#define FILES_TABLE_FIELDS "path TEXT PRIMARY KEY, " \
                           " size INTEGER, hash TEXT"
#define FILES_TABLE_ADD "INSERT INTO \"" FILES_TABLE "\" "\
                        "VALUES ('%s', %d, '%s');"
#define FILES_TABLE_GET_BY_NAME "SELECT path,size,hash FROM " FILES_TABLE " WHERE path='%s';"

#define TABLE_CHECK_QUERY "SELECT name FROM sqlite_master WHERE type='table' AND name='%s';"
#define TABLE_CREATE_QUERY "CREATE TABLE %s (%s) ;"
#define TABLE_DROP_QUERY "DROP TABLE IF EXISTS %s ;"
#define TABLE_COUNT_QUERY "SELECT COUNT(*) FROM %s ;"

struct _DinleDbSqlitePrivate
{
    sqlite3 *db;
};

static gboolean _set_db (DinleDb *db, const gchar *name);
static gboolean _add_file (DinleDb *db, DinleMediaFile *file);
static DinleMediaFile* _get_file_by_name (DinleDb *db, const gchar *file);
static gboolean _unset (DinleDb *db);
static gboolean _drop (DinleDb *db);
static gint _file_count (DinleDb *db);

static gboolean _check_create_table (DinleDbSqlite *db, const gchar *table_name, const gchar *fields);


/******************************************************************************/

static gboolean
_check_create_table (DinleDbSqlite *db, const gchar *table_name, const gchar *fields)
{
    g_return_val_if_fail (DINLE_IS_DB_SQLITE (db), FALSE);
    DinleDbSqlitePrivate *priv = DB_SQLITE_PRIVATE (db);
    g_return_val_if_fail (priv->db, FALSE);

    gchar **result = NULL;
    gint rows = 0, columns = 0;

    gchar *query = g_strdup_printf (TABLE_CHECK_QUERY, table_name);

    int success = sqlite3_get_table (priv->db, query, &result, &rows, &columns, NULL);

    if (rows > 0)
        return TRUE;

    sqlite3_free_table (result);
    g_free (query);

    query = g_strdup_printf (TABLE_CREATE_QUERY, table_name, fields);
    success = sqlite3_get_table (priv->db, query, &result, &rows, &columns, NULL);
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
    g_print ("Destroying Sqlite backend.\n");
    dinle_db_unset (DINLE_DB (object));

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
    parent_class->unset = _unset;
    parent_class->drop = _drop;
    parent_class->file_count = _file_count;
    parent_class->get_file_by_name = _get_file_by_name;
}

static void
dinle_db_sqlite_init (DinleDbSqlite *self)
{
    self->priv = DB_SQLITE_PRIVATE (self);
    self->priv->db = NULL;
}

static gboolean
_set_db (DinleDb *db, const gchar *name)
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
    gchar *errormsg;

    if (!_check_create_table (DINLE_DB_SQLITE (db), FILES_TABLE, FILES_TABLE_FIELDS))
        return FALSE;

    gchar *query = g_strdup_printf (FILES_TABLE_ADD,
                                    dinle_media_file_get_path (file),
                                    dinle_media_file_get_size (file),
                                    dinle_media_file_get_hash (file));

    int result = sqlite3_get_table (priv->db, query, &table, &rows, &columns, &errormsg);
    sqlite3_free_table (table);
    g_free (query);

    if (result != SQLITE_OK)
        g_warning ("sqlite error happened: %s\n",errormsg);

    return (result == SQLITE_OK);
}

static DinleMediaFile *
_get_file_by_name (DinleDb *db, const gchar *name)
{
    DinleMediaFile *mf = NULL;

    g_return_val_if_fail (DINLE_IS_DB_SQLITE (db), NULL);
    DinleDbSqlitePrivate *priv = DB_SQLITE_PRIVATE (db);
    g_return_val_if_fail (priv->db, NULL);

    gchar **table;
    gint rows, cols;

    gchar *query = g_strdup_printf (FILES_TABLE_GET_BY_NAME, name);

    int result = sqlite3_get_table (priv->db, query, &table, &rows, &cols, NULL);
    if (rows >= 1) {
        /*g_print ("%s %s %s\n", table[cols], table[cols+1], table[cols+2]);*/
        mf = dinle_media_file_new ();
        dinle_media_file_set_with_hash_size (mf, table[cols], table[cols+2], g_ascii_strtoll(table[cols+1], NULL, 10));
    }

    return mf;
}

static gboolean
_unset (DinleDb *db)
{
    g_return_val_if_fail (DINLE_IS_DB_SQLITE (db), FALSE);
    DinleDbSqlitePrivate *priv = DB_SQLITE_PRIVATE (db);
    g_return_val_if_fail (priv->db, FALSE);

    sqlite3_close (priv->db);

    return TRUE;
}

static gboolean
_drop (DinleDb *db)
{
    g_return_val_if_fail (DINLE_IS_DB_SQLITE (db), FALSE);
    DinleDbSqlitePrivate *priv = DB_SQLITE_PRIVATE (db);
    g_return_val_if_fail (priv->db, FALSE);

    gchar **table;
    gint rows, cols;

    gchar *query = g_strdup_printf (TABLE_DROP_QUERY, FILES_TABLE);

    int result = sqlite3_get_table (priv->db, query, &table, &rows, &cols, NULL);
    sqlite3_free_table (table);
    g_free (query);

    return (result == SQLITE_OK);
}

static gint
_file_count (DinleDb *db)
{
    g_return_val_if_fail (DINLE_IS_DB_SQLITE (db), -1);
    DinleDbSqlitePrivate *priv = DB_SQLITE_PRIVATE (db);
    g_return_val_if_fail (priv->db, -1);

    gchar **table;
    gint rows = 0, cols = 0;

    gchar *query = g_strdup_printf (TABLE_COUNT_QUERY, FILES_TABLE);

    int result = sqlite3_get_table (priv->db, query, &table, &rows, &cols, NULL);
    if (rows < 1)
        return -1;

    gint count = (gint) g_ascii_strtoll (table[1], NULL, 10);
    g_free (query);
    sqlite3_free_table (table);

    return (result == SQLITE_OK?count:-1);
}

DinleDb *
dinle_db_sqlite_new (void)
{
    DinleDbSqlite* obj = g_object_new (DINLE_TYPE_DB_SQLITE, NULL);

    return DINLE_DB (obj);
}

DinleDb *
dinle_db_sqlite_new_with_name (const gchar *name)
{
    DinleDbSqlite *db = g_object_new (DINLE_TYPE_DB_SQLITE, NULL);

    if (name && *name)
        dinle_db_set_db (DINLE_DB (db), name);

    return DINLE_DB (db);
}
