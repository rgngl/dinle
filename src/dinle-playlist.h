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
/* dinle-playlist.h */

#ifndef _DINLE_PLAYLIST_H
#define _DINLE_PLAYLIST_H

#include <glib-object.h>
#include "dinle-media-file.h"

G_BEGIN_DECLS

#define DINLE_TYPE_PLAYLIST dinle_playlist_get_type()

#define DINLE_PLAYLIST(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  DINLE_TYPE_PLAYLIST, DinlePlaylist))

#define DINLE_PLAYLIST_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  DINLE_TYPE_PLAYLIST, DinlePlaylistClass))

#define DINLE_IS_PLAYLIST(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  DINLE_TYPE_PLAYLIST))

#define DINLE_IS_PLAYLIST_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  DINLE_TYPE_PLAYLIST))

#define DINLE_PLAYLIST_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  DINLE_TYPE_PLAYLIST, DinlePlaylistClass))

typedef struct _DinlePlaylist DinlePlaylist;
typedef struct _DinlePlaylistClass DinlePlaylistClass;
typedef struct _DinlePlaylistPrivate DinlePlaylistPrivate;

typedef enum {
    DINLE_PLAYLIST_TYPE_UNKNOWN,

    DINLE_PLAYLIST_TYPE_TAG,
    DINLE_PLAYLIST_TYPE_FREE
} DinlePlaylistType;

struct _DinlePlaylist
{
    GObject parent;

    DinlePlaylistPrivate *priv;
};

struct _DinlePlaylistClass
{
    GObjectClass parent_class;
};

GType dinle_playlist_get_type (void) G_GNUC_CONST;

DinlePlaylist *dinle_playlist_new (DinlePlaylistType type);
void dinle_playlist_add (DinlePlaylist *playlist, DinleMediaFile *file);
gboolean dinle_playlist_remove (DinlePlaylist *playlist, DinleMediaFile *file);
gchar* dinle_playlist_client_reply (DinlePlaylist *playlist);

G_END_DECLS

#endif /* _DINLE_PLAYLIST_H */
