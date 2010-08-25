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
/* dinle-db-sqlite.h */

#ifndef _DINLE_DB_SQLITE_H
#define _DINLE_DB_SQLITE_H

#include <glib-object.h>
#include "dinle-db.h"

G_BEGIN_DECLS

#define DINLE_TYPE_DB_SQLITE dinle_db_sqlite_get_type()

#define DINLE_DB_SQLITE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  DINLE_TYPE_DB_SQLITE, DinleDbSqlite))

#define DINLE_DB_SQLITE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  DINLE_TYPE_DB_SQLITE, DinleDbSqliteClass))

#define DINLE_IS_DB_SQLITE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  DINLE_TYPE_DB_SQLITE))

#define DINLE_IS_DB_SQLITE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  DINLE_TYPE_DB_SQLITE))

#define DINLE_DB_SQLITE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  DINLE_TYPE_DB_SQLITE, DinleDbSqliteClass))

typedef struct _DinleDbSqlite DinleDbSqlite;
typedef struct _DinleDbSqliteClass DinleDbSqliteClass;
typedef struct _DinleDbSqlitePrivate DinleDbSqlitePrivate;

struct _DinleDbSqlite
{
    DinleDb parent;

    DinleDbSqlitePrivate *priv;
};

struct _DinleDbSqliteClass
{
    DinleDbClass parent_class;
};

GType dinle_db_sqlite_get_type (void) G_GNUC_CONST;

DinleDb *dinle_db_sqlite_new (void);
DinleDb *dinle_db_sqlite_new_with_name (gchar *name);

G_END_DECLS

#endif /* _DINLE_DB_SQLITE_H */
