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

/*Sqlite Queries -----------------------------------------------------------*/
#define FILES_TABLE "files"
#define FILES_TABLE_FIELDS "path TEXT PRIMARY KEY, " \
                           " size INTEGER, hash TEXT"
#define FILES_TABLE_ADD "INSERT INTO '" FILES_TABLE "' "\
                        "VALUES ('%q', %d, '%q');"
#define FILES_TABLE_REMOVE "DELETE FROM " FILES_TABLE "WHERE path='%q';"
#define FILES_TABLE_GET_BY_NAME "SELECT path,size,hash FROM " FILES_TABLE " WHERE path='%q';"
#define FILES_TABLE_GET_FILES "SELECT path FROM " FILES_TABLE "; "

#define METADATA_TABLE "metadata"
#define METADATA_TABLE_FIELDS "path TEXT, field TEXT, value TEXT, PRIMARY KEY (path, field)"
#define METADATA_TABLE_ADD "INSERT INTO '" METADATA_TABLE "' "\
                           "VALUES ('%q', '%q', '%q');"
#define METADATA_TABLE_REMOVE "DELETE FROM " METADATA_TABLE "WHERE path='%q';"
#define METADATA_TABLE_GET_BY_NAME "SELECT path, field, value FROM " METADATA_TABLE " WHERE path='%q';"
#define METADATA_TABLE_GET_BY_TAGS   "SELECT path, hash, size FROM " FILES_TABLE " "\
                                     "WHERE path IN ( "
#define METADATA_TABLE_GET_BY_TAGS_I "SELECT DISTINCT path FROM " METADATA_TABLE \
                                     " WHERE field=='%q' and value=='%q'"
#define METADATA_TABLE_GET_BY_TAGS_A " INTERSECT " METADATA_TABLE_GET_BY_TAGS_I
#define METADATA_TABLE_GET_BY_TAGS_T ") ; "
#define METADATA_TABLE_GET_KEYWORDS "SELECT path, hash, size FROM " FILES_TABLE " "\
                                    "WHERE path IN (" \
                                    "SELECT DISTINCT path FROM " METADATA_TABLE " WHERE ("
#define METADATA_TABLE_GET_KEYWORDS_A " value LIKE '%%%q%%' "
#define METADATA_TABLE_GET_KEYWORDS_O " OR "
#define METADATA_TABLE_GET_KEYWORDS_T ")) ; "
#define METADATA_TABLE_GET_FILE_TAGS "SELECT field, value FROM metadata where path='%q';"

#define TABLE_CHECK_QUERY "SELECT name FROM sqlite_master WHERE type='table' AND name='%q';"
#define TABLE_CREATE_QUERY "CREATE TABLE %s (%s) ;"
#define TABLE_DROP_QUERY "DROP TABLE IF EXISTS %s ;"
#define TABLE_COUNT_QUERY "SELECT COUNT(*) FROM %s ;"
/*--------------------------------------------------------------------------*/

struct _DinleDbSqlitePrivate
{
    sqlite3 *db;
};

/*Virtual function implementations------------------------------------------*/
static gboolean _set_db (DinleDb *db, const gchar *name);
static gboolean _add_file (DinleDb *db, DinleMediaFile *file);
static gboolean _remove_file (DinleDb *db, const gchar *file);
static DinleMediaFile* _get_file_by_name (DinleDb *db, const gchar *file);
static DinleMediaFile** _search_keywords (DinleDb *db, const gchar **keywords);
static DinleMediaFile** _search_by_tags (DinleDb *db, const gchar **pairs);
static DinleMediaMetadata* _get_file_metadata (DinleDb *db, const gchar *file);
static gchar** _get_files (DinleDb *db);
static gboolean _file_exists (DinleDb *db, const gchar *file);
static gboolean _unset (DinleDb *db);
static gboolean _drop (DinleDb *db);
static gint _file_count (DinleDb *db);
/*--------------------------------------------------------------------------*/

/*Utility functions---------------------------------------------------------*/
static gboolean _check_create_table (DinleDbSqlite *db, const gchar *table_name, const gchar *fields);
/*--------------------------------------------------------------------------*/


/******************************************************************************/

static gboolean
_check_create_table (DinleDbSqlite *db, const gchar *table_name, const gchar *fields)
{
    g_return_val_if_fail (DINLE_IS_DB_SQLITE (db), FALSE);
    DinleDbSqlitePrivate *priv = DB_SQLITE_PRIVATE (db);
    g_return_val_if_fail (priv->db, FALSE);

    gchar **result = NULL;
    gint rows = 0, columns = 0;

    gchar *query = sqlite3_mprintf (TABLE_CHECK_QUERY, table_name);

    int success = sqlite3_get_table (priv->db, query, &result, &rows, &columns, NULL);

    if (rows > 0)
        return TRUE;

    sqlite3_free_table (result);
    sqlite3_free (query);

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
    parent_class->search_keywords = _search_keywords;
    parent_class->search_by_tags = _search_by_tags;
    parent_class->get_files = _get_files;
    parent_class->file_exists = _file_exists;
    parent_class->remove_file = _remove_file;
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

    if (!_check_create_table (DINLE_DB_SQLITE (db), FILES_TABLE, FILES_TABLE_FIELDS))
        return FALSE;

    if (!_check_create_table (DINLE_DB_SQLITE (db), METADATA_TABLE, METADATA_TABLE_FIELDS))
        return FALSE;

    return ( (result == SQLITE_OK) && (priv->db != NULL) );
}

static gboolean
_add_file (DinleDb *db, DinleMediaFile *file)
{
    g_return_val_if_fail (DINLE_IS_DB_SQLITE (db), FALSE);
    DinleDbSqlitePrivate *priv = DB_SQLITE_PRIVATE (db);
    g_return_val_if_fail (priv->db, FALSE);
    g_return_val_if_fail (DINLE_IS_MEDIA_FILE (file), FALSE);

    gchar *errormsg;

    gchar *query = sqlite3_mprintf (FILES_TABLE_ADD,
                                    dinle_media_file_get_path (file),
                                    dinle_media_file_get_size (file),
                                    dinle_media_file_get_hash (file));

    int result = sqlite3_exec (priv->db, query, NULL, NULL, &errormsg);
    sqlite3_free (query);

    if (result != SQLITE_OK) {
        g_warning ("sqlite error happened: %s\n",errormsg);
        return FALSE;
    }

    DinleMediaMetadata *md = dinle_media_file_get_metadata (file);
    gchar **field_list = dinle_media_metadata_get_field_value_list (md);

    int i = 0;
    while (field_list && field_list[i]) {
        query = sqlite3_mprintf (METADATA_TABLE_ADD,
                                 dinle_media_file_get_path (file),
                                 field_list[i],
                                 field_list[i+1]);
        result = sqlite3_exec (priv->db, query, NULL, NULL, &errormsg);
        sqlite3_free (query);
        i += 2;
    }

    if (result != SQLITE_OK) {
        g_warning ("sqlite error happened: %s\n", errormsg);
    }

    g_strfreev (field_list);
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

    gchar *query = sqlite3_mprintf (FILES_TABLE_GET_BY_NAME,
                                    name);

    int result = sqlite3_get_table (priv->db, query, &table, &rows, &cols, NULL);
    if (rows >= 1) {
        /*g_print ("%s %s %s\n", table[cols], table[cols+1], table[cols+2]);*/
        mf = dinle_media_file_new (table[cols]);
        /*dinle_media_file_set_with_hash_size (mf, table[cols], table[cols+2],*/
                /*g_ascii_strtoll(table[cols+1], NULL, 10));*/
    }

    sqlite3_free_table (table);
    sqlite3_free (query);

    return mf;
}

static DinleMediaFile **
_search_keywords (DinleDb *db, const gchar **keywords)
{
    DinleMediaFile **list = NULL;

    g_return_val_if_fail (DINLE_IS_DB_SQLITE (db), NULL);
    DinleDbSqlitePrivate *priv = DB_SQLITE_PRIVATE (db);
    g_return_val_if_fail (priv->db, NULL);
    g_return_val_if_fail (keywords, NULL);

    GString *query = g_string_new (METADATA_TABLE_GET_KEYWORDS);

    gint i = 0;
    const gchar *key = keywords[i];
    while (key) {
        gchar *eq = sqlite3_mprintf (METADATA_TABLE_GET_KEYWORDS_A, key);
        if (i > 0)
            g_string_append (query, METADATA_TABLE_GET_KEYWORDS_O);
        g_string_append (query, eq);
        sqlite3_free (eq);

        key = keywords[++i];
    }

    g_string_append (query, METADATA_TABLE_GET_KEYWORDS_T);

    gchar **table = NULL;
    gint rows, cols;
    gchar *error_msg = NULL;

    gint result = sqlite3_get_table (priv->db, query->str, &table, &rows, &cols, &error_msg);
    if (result != SQLITE_OK) {
        g_print ("error happened :/ %s\nquery: %s", error_msg, query->str);
        goto clean;
    }

    if (rows >= 1) {
        list = g_malloc0 (sizeof(DinleMediaFile*)*(rows + 1));
        gint index = 0;
        for (i = cols; i <= rows*cols; i+=cols) {
            list[index] = dinle_media_file_new (table[i]);
            if (list[index])
                dinle_media_file_set_hash_size (list[index], table[i+1], table[i+2]);
            index++;
        }
    }

clean:
    sqlite3_free_table (table);
    g_string_free (query, TRUE);
    return list;
}

static DinleMediaFile **
_search_by_tags (DinleDb *db, const gchar **pairs)
{
    DinleMediaFile **list = NULL;

    g_return_val_if_fail (DINLE_IS_DB_SQLITE (db), NULL);
    DinleDbSqlitePrivate *priv = DB_SQLITE_PRIVATE (db);
    g_return_val_if_fail (priv->db, NULL);
    g_return_val_if_fail (pairs, NULL);

    gchar *query = g_strdup (METADATA_TABLE_GET_BY_TAGS);

    gint i = 0;
    const gchar *tag = pairs[i];
    const gchar *term = pairs[++i];

    gchar *eq = sqlite3_mprintf (METADATA_TABLE_GET_BY_TAGS_I, tag, term);
    gchar *temp = g_strconcat (query, eq, NULL);
    sqlite3_free (eq);
    g_free (query);
    query = temp;

    tag = pairs[++i];
    while (tag) {
        term = pairs[++i];

        eq = sqlite3_mprintf (METADATA_TABLE_GET_BY_TAGS_A, tag, term);
        temp = g_strconcat (query, eq, NULL);
        sqlite3_free (eq);
        g_free (query);
        query = temp;

        tag = pairs[++i];
    }

    temp = g_strconcat (query, METADATA_TABLE_GET_BY_TAGS_T, NULL);
    g_free (query);
    query = temp;
    /*g_print ("tag search query: %s\n", query);*/

    gchar **table = NULL;
    gint rows, cols;
    gchar *error_msg = NULL;

    gint result = sqlite3_get_table (priv->db, query, &table, &rows, &cols, &error_msg);
    if (result != SQLITE_OK) {
        g_print ("error happened :/ %s\nquery: %s", error_msg, query);
        goto clean;
    }
    /*g_print ("rows: %d cols: %d\n", rows, cols);*/

    if (rows >= 1) {
        list = g_malloc0 (sizeof(DinleMediaFile*)*(rows + 1));
        gint index = 0;
        for (i = cols; i <= rows*cols; i+=cols) {
            list[index] = dinle_media_file_new (table[i]);
            if (list[index])
                dinle_media_file_set_hash_size (list[index], table[i+1], table[i+2]);
            index++;
        }
    }

clean:
    sqlite3_free_table (table);
    g_free (query);
    return list;
}

static DinleMediaMetadata* _get_file_metadata (DinleDb *db, const gchar *file)
{
    DinleMediaMetadata *md = NULL;

    g_return_val_if_fail (DINLE_IS_DB_SQLITE (db), NULL);
    DinleDbSqlitePrivate *priv = DB_SQLITE_PRIVATE (db);
    g_return_val_if_fail (priv->db, NULL);

    gchar *query = sqlite3_mprintf (METADATA_TABLE_GET_FILE_TAGS, file);

    gchar **table = NULL;
    gint rows, cols;
    gchar *error_msg = NULL;

    gint result = sqlite3_get_table (priv->db, query, &table, &rows, &cols, &error_msg);
    if (result != SQLITE_OK) {
        g_print ("error happened :/ %s\n", error_msg);
        goto clean;
    }
    int i;

    if (rows >=1) {
        md = dinle_media_metadata_new ();
        for (i = 1; i <= rows; i+=cols) {
            g_object_set (md, table[i], table[i+1], NULL);
        }
    }

clean:
    sqlite3_free_table (table);
    g_free (query);
    return md;
}

static gchar **
_get_files (DinleDb *db)
{
    gchar **list = NULL;

    g_return_val_if_fail (DINLE_IS_DB_SQLITE (db), NULL);
    DinleDbSqlitePrivate *priv = DB_SQLITE_PRIVATE (db);
    g_return_val_if_fail (priv->db, NULL);

    gchar **table;
    gint rows, cols;

    int result = sqlite3_get_table (priv->db, FILES_TABLE_GET_FILES, &table, &rows, &cols, NULL);
    if (rows >= 1) {
        list = g_malloc0 (sizeof(gchar*) * rows + 1);
        int i = 1;
        while (i <= rows) {
            list [i-1] = g_strdup (table[i]);
            i++;
        }
    }

    sqlite3_free_table (table);

    return list;
}

static gboolean
_file_exists (DinleDb *db, const gchar *file)
{
    g_return_val_if_fail (DINLE_IS_DB_SQLITE (db), FALSE);
    DinleDbSqlitePrivate *priv = DB_SQLITE_PRIVATE (db);
    g_return_val_if_fail (priv->db, FALSE);

    gboolean exists = FALSE;

    gchar **table;
    gint rows, cols;

    gchar *query = sqlite3_mprintf (FILES_TABLE_GET_BY_NAME, file);
    int result = sqlite3_get_table (priv->db, query, &table, &rows, &cols, NULL);
    if (rows > 0) {
        exists = TRUE;
    }
    sqlite3_free_table (table);
    sqlite3_free (query);

    return exists;
}

static gboolean
_remove_file (DinleDb *db, const gchar *file)
{
    g_return_val_if_fail (DINLE_IS_DB_SQLITE (db), FALSE);
    DinleDbSqlitePrivate *priv = DB_SQLITE_PRIVATE (db);
    g_return_val_if_fail (priv->db, FALSE);

    gchar *query = sqlite3_mprintf (FILES_TABLE_REMOVE, file);
    int result = sqlite3_exec (priv->db, query, NULL, NULL, NULL);
    sqlite3_free (query);

    if (result != SQLITE_OK)
        return FALSE;

    query = sqlite3_mprintf (METADATA_TABLE_REMOVE, file);
    result = sqlite3_exec (priv->db, query, NULL, NULL, NULL);
    sqlite3_free (query);

    return (result == SQLITE_OK);
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

    gchar *query = g_strdup_printf (TABLE_DROP_QUERY, FILES_TABLE);
    int result = sqlite3_exec (priv->db, query, NULL, NULL, NULL);
    g_free (query);

    if (result != SQLITE_OK)
        return FALSE;

    query = g_strdup_printf (TABLE_DROP_QUERY, METADATA_TABLE);
    result = sqlite3_exec (priv->db, query, NULL, NULL, NULL);
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
