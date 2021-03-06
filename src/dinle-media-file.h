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
/* dinle-media-file.h */

#ifndef _DINLE_MEDIA_FILE_H
#define _DINLE_MEDIA_FILE_H

#include <glib-object.h>

#include "dinle-media-metadata.h"

G_BEGIN_DECLS

#define DINLE_TYPE_MEDIA_FILE dinle_media_file_get_type()

#define DINLE_MEDIA_FILE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  DINLE_TYPE_MEDIA_FILE, DinleMediaFile))

#define DINLE_MEDIA_FILE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  DINLE_TYPE_MEDIA_FILE, DinleMediaFileClass))

#define DINLE_IS_MEDIA_FILE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  DINLE_TYPE_MEDIA_FILE))

#define DINLE_IS_MEDIA_FILE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  DINLE_TYPE_MEDIA_FILE))

#define DINLE_MEDIA_FILE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  DINLE_TYPE_MEDIA_FILE, DinleMediaFileClass))

typedef struct _DinleMediaFile DinleMediaFile;
typedef struct _DinleMediaFileClass DinleMediaFileClass;
typedef struct _DinleMediaFilePrivate DinleMediaFilePrivate;

struct _DinleMediaFile
{
    GObject parent;

    DinleMediaFilePrivate *priv;
};

struct _DinleMediaFileClass
{
    GObjectClass parent_class;

    const gchar * (*extensions) (DinleMediaFile *);
    const DinleMediaMetadata * const (*get_metadata_file) (DinleMediaFile *self, gchar *file);
};

GType dinle_media_file_get_type (void) G_GNUC_CONST;

DinleMediaFile *dinle_media_file_new (const gchar *file);
void dinle_media_file_initialize (void);
gboolean dinle_is_file_supported (const gchar *file);
const gchar *dinle_media_file_extensions(DinleMediaFile *self);
gboolean dinle_media_file_set (DinleMediaFile *self, const gchar *file);
void dinle_media_file_set_hash_size (DinleMediaFile *self, const gchar *hash, guint size);
void dinle_media_file_set_metadata (DinleMediaFile *self, DinleMediaMetadata *md);
const gchar *dinle_media_file_get_path (DinleMediaFile *self);
const DinleMediaMetadata * dinle_media_file_get_metadata (DinleMediaFile *self);
const gchar * dinle_media_file_get_hash (DinleMediaFile *self);
guint dinle_media_file_get_size (DinleMediaFile *self);

G_END_DECLS

#endif /* _DINLE_MEDIA_FILE_H */
