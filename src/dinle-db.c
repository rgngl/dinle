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
/* dinle-db.c */

#include "dinle-db.h"

G_DEFINE_TYPE (DinleDb, dinle_db, G_TYPE_OBJECT)

#define DB_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), DINLE_TYPE_DB, DinleDbPrivate))

struct _DinleDbPrivate
{
    int foo;
};


static void
dinle_db_get_property (GObject    *object,
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
dinle_db_set_property (GObject      *object,
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
dinle_db_dispose (GObject *object)
{
    G_OBJECT_CLASS (dinle_db_parent_class)->dispose (object);
}

static void
dinle_db_finalize (GObject *object)
{
    g_print ("Destroying database object.\n");

    dinle_db_unset (DINLE_DB (object));
    G_OBJECT_CLASS (dinle_db_parent_class)->finalize (object);
}

static void
dinle_db_class_init (DinleDbClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    g_type_class_add_private (klass, sizeof (DinleDbPrivate));

    object_class->get_property = dinle_db_get_property;
    object_class->set_property = dinle_db_set_property;
    object_class->dispose = dinle_db_dispose;
    object_class->finalize = dinle_db_finalize;
    klass->set_db = NULL;
    klass->add_file = NULL;
    klass->get_file_by_name = NULL;
    klass->search_by_tags_valist = NULL;
    klass->get_files = NULL;
    klass->remove_file = NULL;
    klass->unset = NULL;
    klass->drop = NULL;
    klass->file_count = NULL;
}

static void
dinle_db_init (DinleDb *self)
{
    self->priv = DB_PRIVATE (self);
}

gboolean
dinle_db_set_db (DinleDb* db, const gchar *name)
{
    g_return_val_if_fail (DINLE_IS_DB (db), FALSE);

    if (DINLE_DB_GET_CLASS (db)->set_db)
        return DINLE_DB_GET_CLASS (db)->set_db (db, name);

    g_critical ("Pure virtual function set_db called...\n");
    return FALSE;
}

gboolean
dinle_db_add_file (DinleDb *db, DinleMediaFile *file)
{
    g_return_val_if_fail (DINLE_IS_DB (db), FALSE);

    if (DINLE_DB_GET_CLASS (db)->add_file)
        return DINLE_DB_GET_CLASS (db)->add_file (db, file);

    g_critical ("Pure virtual function add_file called...\n");
    return FALSE;
}

DinleMediaFile *
dinle_db_get_file_by_name (DinleDb *db, const gchar *name)
{
    g_return_val_if_fail (DINLE_IS_DB (db), NULL);

    if (DINLE_DB_GET_CLASS (db)->get_file_by_name)
        return DINLE_DB_GET_CLASS (db)->get_file_by_name (db, name);

    g_critical ("Pure virtual function get_file_by_name called...\n");
    return NULL;
}

DinleMediaFile **
dinle_db_search_by_tags_valist (DinleDb *db, const gchar *first_tag, va_list vars)
{
    g_return_val_if_fail (DINLE_IS_DB (db), NULL);

    if (DINLE_DB_GET_CLASS (db)->search_by_tags_valist) {
        DinleMediaFile **list = DINLE_DB_GET_CLASS (db)->search_by_tags_valist (db,
                                                                                first_tag,
                                                                                vars);
        return list;
    }

    g_critical ("Pure virtual function search_by_tag called...\n");
    return NULL;
}

DinleMediaFile **
dinle_db_search_by_tags (DinleDb *db, const gchar *first_tag, ...)
{
    va_list var_args;

    g_return_val_if_fail (DINLE_IS_DB (db), NULL);

    va_start (var_args, first_tag);
    DinleMediaFile **list = dinle_db_search_by_tags_valist (db, first_tag, var_args);
    va_end (var_args);

    return list;
}

gchar **
dinle_db_get_files (DinleDb *db)
{
    g_return_val_if_fail (DINLE_IS_DB (db), NULL);

    if (DINLE_DB_GET_CLASS (db)->get_files)
        return DINLE_DB_GET_CLASS (db)->get_files (db);

    g_critical ("Pure virtual function get_files...\n");
    return NULL;
}

gboolean
dinle_db_file_exists (DinleDb *db, const gchar *name)
{
    g_return_val_if_fail (DINLE_IS_DB (db), FALSE);

    if (DINLE_DB_GET_CLASS (db)->file_exists)
        return DINLE_DB_GET_CLASS (db)->file_exists (db, name);

    g_critical ("Pure virtual function file_exists called...\n");
    return FALSE;
}

gboolean dinle_db_remove_file (DinleDb *db, const gchar *file)
{
    g_return_val_if_fail (DINLE_IS_DB (db), FALSE);

    if (DINLE_DB_GET_CLASS (db)->remove_file)
        return DINLE_DB_GET_CLASS (db)->remove_file (db, file);

    g_critical ("Pure virtual function remove_file called...\n");
    return FALSE;
}

gboolean dinle_db_unset (DinleDb *db)
{
    g_return_val_if_fail (DINLE_IS_DB (db), FALSE);

    if (DINLE_DB_GET_CLASS (db)->unset)
        return DINLE_DB_GET_CLASS (db)->unset (db);

    g_critical ("Pure virtual function unset called...\n");
    return FALSE;
}

gboolean dinle_db_drop (DinleDb *db)
{
    g_return_val_if_fail (DINLE_IS_DB (db), FALSE);

    if (DINLE_DB_GET_CLASS (db)->drop)
        return DINLE_DB_GET_CLASS (db)->drop (db);

    g_critical ("Pure virtual function drop called...\n");
    return FALSE;
}

gint dinle_db_file_count (DinleDb *db)
{
    g_return_val_if_fail (DINLE_IS_DB (db), -1);

    if (DINLE_DB_GET_CLASS (db)->file_count)
        return DINLE_DB_GET_CLASS (db)->file_count (db);

    g_critical ("Pure virtual function file_count called...\n");
    return -1;
}
