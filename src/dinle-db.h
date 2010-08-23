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
/* dinle-db.h */

#ifndef _DINLE_DB_H
#define _DINLE_DB_H

#include <glib-object.h>

G_BEGIN_DECLS

#define DINLE_TYPE_DB dinle_db_get_type()

#define DINLE_DB(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  DINLE_TYPE_DB, DinleDb))

#define DINLE_DB_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  DINLE_TYPE_DB, DinleDbClass))

#define DINLE_IS_DB(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  DINLE_TYPE_DB))

#define DINLE_IS_DB_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  DINLE_TYPE_DB))

#define DINLE_DB_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  DINLE_TYPE_DB, DinleDbClass))

typedef struct _DinleDb DinleDb;
typedef struct _DinleDbClass DinleDbClass;
typedef struct _DinleDbPrivate DinleDbPrivate;

struct _DinleDb
{
  GObject parent;

  DinleDbPrivate *priv;
};

struct _DinleDbClass
{
  GObjectClass parent_class;
};

GType dinle_db_get_type (void) G_GNUC_CONST;

DinleDb *dinle_db_new (void);

G_END_DECLS

#endif /* _DINLE_DB_H */