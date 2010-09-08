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
/* dinle-archive-manager.h */

#ifndef _DINLE_ARCHIVE_MANAGER_H
#define _DINLE_ARCHIVE_MANAGER_H

#include <glib-object.h>
#include "dinle-db.h"

G_BEGIN_DECLS

#define DINLE_TYPE_ARCHIVE_MANAGER dinle_archive_manager_get_type()

#define DINLE_ARCHIVE_MANAGER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  DINLE_TYPE_ARCHIVE_MANAGER, DinleArchiveManager))

#define DINLE_ARCHIVE_MANAGER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  DINLE_TYPE_ARCHIVE_MANAGER, DinleArchiveManagerClass))

#define DINLE_IS_ARCHIVE_MANAGER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  DINLE_TYPE_ARCHIVE_MANAGER))

#define DINLE_IS_ARCHIVE_MANAGER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  DINLE_TYPE_ARCHIVE_MANAGER))

#define DINLE_ARCHIVE_MANAGER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  DINLE_TYPE_ARCHIVE_MANAGER, DinleArchiveManagerClass))

typedef struct _DinleArchiveManager DinleArchiveManager;
typedef struct _DinleArchiveManagerClass DinleArchiveManagerClass;
typedef struct _DinleArchiveManagerPrivate DinleArchiveManagerPrivate;

struct _DinleArchiveManager
{
    GObject parent;

    DinleArchiveManagerPrivate *priv;
};

struct _DinleArchiveManagerClass
{
    GObjectClass parent_class;
};

GType dinle_archive_manager_get_type (void) G_GNUC_CONST;

DinleArchiveManager *dinle_archive_manager_get (void);
void dinle_archive_manager_update (void);
DinleDb* dinle_archive_manager_get_db (void);

G_END_DECLS

#endif /* _DINLE_ARCHIVE_MANAGER_H */
