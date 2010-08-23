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

G_DEFINE_TYPE (DinleDbSqlite, dinle_db_sqlite, DINLE_TYPE_DB)

#define DB_SQLITE_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), DINLE_TYPE_DB_SQLITE, DinleDbSqlitePrivate))

struct _DinleDbSqlitePrivate
{
};


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

  g_type_class_add_private (klass, sizeof (DinleDbSqlitePrivate));

  object_class->get_property = dinle_db_sqlite_get_property;
  object_class->set_property = dinle_db_sqlite_set_property;
  object_class->dispose = dinle_db_sqlite_dispose;
  object_class->finalize = dinle_db_sqlite_finalize;
}

static void
dinle_db_sqlite_init (DinleDbSqlite *self)
{
  self->priv = DB_SQLITE_PRIVATE (self);
}

DinleDbSqlite *
dinle_db_sqlite_new (void)
{
  return g_object_new (DINLE_TYPE_DB_SQLITE, NULL);
}
