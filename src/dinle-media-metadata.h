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
/* dinle-media-metadata.h */

#ifndef _DINLE_MEDIA_METADATA_H
#define _DINLE_MEDIA_METADATA_H

#include <glib-object.h>

G_BEGIN_DECLS

#define DINLE_TYPE_MEDIA_METADATA dinle_media_metadata_get_type()

#define DINLE_MEDIA_METADATA(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  DINLE_TYPE_MEDIA_METADATA, DinleMediaMetadata))

#define DINLE_MEDIA_METADATA_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  DINLE_TYPE_MEDIA_METADATA, DinleMediaMetadataClass))

#define DINLE_IS_MEDIA_METADATA(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  DINLE_TYPE_MEDIA_METADATA))

#define DINLE_IS_MEDIA_METADATA_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  DINLE_TYPE_MEDIA_METADATA))

#define DINLE_MEDIA_METADATA_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  DINLE_TYPE_MEDIA_METADATA, DinleMediaMetadataClass))

typedef struct _DinleMediaMetadata DinleMediaMetadata;
typedef struct _DinleMediaMetadataClass DinleMediaMetadataClass;
typedef struct _DinleMediaMetadataPrivate DinleMediaMetadataPrivate;

struct _DinleMediaMetadata
{
  GObject parent;

  DinleMediaMetadataPrivate *priv;
};

struct _DinleMediaMetadataClass
{
  GObjectClass parent_class;
};

GType dinle_media_metadata_get_type (void) G_GNUC_CONST;

DinleMediaMetadata *dinle_media_metadata_new (void);
gchar **dinle_media_metadata_get_field_value_list (DinleMediaMetadata *md);

G_END_DECLS

#endif /* _DINLE_MEDIA_METADATA_H */
