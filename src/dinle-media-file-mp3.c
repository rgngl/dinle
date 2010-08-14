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
/* dinle-media-file-mp3.c */

#include "dinle-media-file-mp3.h"

#include <id3tag.h>

G_DEFINE_TYPE (DinleMediaFileMp3, dinle_media_file_mp3, DINLE_TYPE_MEDIA_FILE)

#define MEDIA_FILE_MP3_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), DINLE_TYPE_MEDIA_FILE_MP3, DinleMediaFileMp3Private))

struct _DinleMediaFileMp3Private
{
    int foo;
};

static const gchar * extensions = "mp3";

/*****************************************************************************/
/* virtual method overrides */

static const gchar * _extensions (DinleMediaFile *self);

/*****************************************************************************/
static void
dinle_media_file_mp3_get_property (GObject    *object,
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
dinle_media_file_mp3_set_property (GObject      *object,
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
dinle_media_file_mp3_dispose (GObject *object)
{
    G_OBJECT_CLASS (dinle_media_file_mp3_parent_class)->dispose (object);
}

static void
dinle_media_file_mp3_finalize (GObject *object)
{
    G_OBJECT_CLASS (dinle_media_file_mp3_parent_class)->finalize (object);
}

static void
dinle_media_file_mp3_class_init (DinleMediaFileMp3Class *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    DinleMediaFileClass *parent_class = g_type_class_peek_parent (klass);

    g_type_class_add_private (klass, sizeof (DinleMediaFileMp3Private));

    object_class->get_property = dinle_media_file_mp3_get_property;
    object_class->set_property = dinle_media_file_mp3_set_property;
    object_class->dispose = dinle_media_file_mp3_dispose;
    object_class->finalize = dinle_media_file_mp3_finalize;
    DINLE_MEDIA_FILE_CLASS(klass)->extensions = _extensions;
}

static void
dinle_media_file_mp3_init (DinleMediaFileMp3 *self)
{
    self->priv = MEDIA_FILE_MP3_PRIVATE (self);
}

DinleMediaFileMp3 *
dinle_media_file_mp3_new (void)
{
    return g_object_new (DINLE_TYPE_MEDIA_FILE_MP3, NULL);
}

static const gchar *
_extensions (DinleMediaFile *self)
{
    return extensions;
}
