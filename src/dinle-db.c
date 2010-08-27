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
    klass->unset = NULL;
    klass->drop = NULL;
    klass->file_count = NULL;
}

static void
dinle_db_init (DinleDb *self)
{
    self->priv = DB_PRIVATE (self);
}

DinleDb *
dinle_db_new (void)
{
    return g_object_new (DINLE_TYPE_DB, NULL);
}

gboolean
dinle_db_set_db (DinleDb* db, gchar *name)
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
