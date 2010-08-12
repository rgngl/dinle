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
/* dinle-media-file-mp3.h */

#ifndef _DINLE_MEDIA_FILE_MP3_H
#define _DINLE_MEDIA_FILE_MP3_H

#include <glib-object.h>
#include "dinle-media-file.h"

G_BEGIN_DECLS

#define DINLE_TYPE_MEDIA_FILE_MP3 dinle_media_file_mp3_get_type()

#define DINLE_MEDIA_FILE_MP3(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  DINLE_TYPE_MEDIA_FILE_MP3, DinleMediaFileMp3))

#define DINLE_MEDIA_FILE_MP3_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  DINLE_TYPE_MEDIA_FILE_MP3, DinleMediaFileMp3Class))

#define DINLE_IS_MEDIA_FILE_MP3(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  DINLE_TYPE_MEDIA_FILE_MP3))

#define DINLE_IS_MEDIA_FILE_MP3_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  DINLE_TYPE_MEDIA_FILE_MP3))

#define DINLE_MEDIA_FILE_MP3_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  DINLE_TYPE_MEDIA_FILE_MP3, DinleMediaFileMp3Class))

typedef struct _DinleMediaFileMp3 DinleMediaFileMp3;
typedef struct _DinleMediaFileMp3Class DinleMediaFileMp3Class;
typedef struct _DinleMediaFileMp3Private DinleMediaFileMp3Private;

struct _DinleMediaFileMp3
{
    DinleMediaFile parent;

    DinleMediaFileMp3Private *priv;
};

struct _DinleMediaFileMp3Class
{
    DinleMediaFileClass parent_class;
};

GType dinle_media_file_mp3_get_type (void) G_GNUC_CONST;

DinleMediaFileMp3 *dinle_media_file_mp3_new (void);

G_END_DECLS

#endif /* _DINLE_MEDIA_FILE_MP3_H */
