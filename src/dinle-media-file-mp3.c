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
static const DinleMediaMetadata * _get_metadata_file (DinleMediaFile *self, gchar *file);

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
    DINLE_MEDIA_FILE_CLASS(klass)->get_metadata_file = _get_metadata_file;
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

static const DinleMediaMetadata *
_get_metadata_file (DinleMediaFile *self, gchar *file)
{
    id3_utf8_t *title = NULL;
    id3_utf8_t *artist = NULL;
    id3_utf8_t *album = NULL;
    id3_utf8_t *genre = NULL;
    id3_utf8_t *year = NULL;
    id3_utf8_t *track = NULL;
    id3_utf8_t *tracks = NULL;
    id3_utf8_t *length = NULL;
    id3_utf8_t *disc = NULL;
    id3_utf8_t *discs = NULL;
    id3_utf8_t *discno = NULL;

    struct id3_file *tagfile = id3_file_open (file, ID3_FILE_MODE_READONLY);
    const struct id3_tag *tag = id3_file_tag (tagfile);
    struct id3_frame *frame = id3_tag_findframe (tag, ID3_FRAME_TITLE, 0);
    if (frame) {
        union id3_field const *field;
        unsigned int nstrings;

        field    = id3_frame_field(frame, 1);
        nstrings = id3_field_getnstrings(field);

        const id3_ucs4_t *str = id3_field_getstrings(field, 0);
        title = id3_ucs4_utf8duplicate (str);
        g_print ("title: %s\n", title);
    }

    frame = id3_tag_findframe (tag, ID3_FRAME_ARTIST, 0);
    if (frame) {
        union id3_field const *field;
        unsigned int nstrings;

        field    = id3_frame_field(frame, 1);
        nstrings = id3_field_getnstrings(field);

        const id3_ucs4_t *str = id3_field_getstrings(field, 0);
        artist = id3_ucs4_utf8duplicate (str);
        g_print ("artist: %s\n", artist);
    }

    frame = id3_tag_findframe (tag, ID3_FRAME_ALBUM, 0);
    if (frame) {
        union id3_field const *field;
        unsigned int nstrings;

        field    = id3_frame_field(frame, 1);
        nstrings = id3_field_getnstrings(field);

        const id3_ucs4_t *str = id3_field_getstrings(field, 0);
        album = id3_ucs4_utf8duplicate (str);
        g_print ("album: %s\n", album);
    }

    frame = id3_tag_findframe (tag, ID3_FRAME_GENRE, 0);
    if (frame) {
        union id3_field const *field;
        unsigned int nstrings;

        field    = id3_frame_field(frame, 1);
        nstrings = id3_field_getnstrings(field);

        const id3_ucs4_t *str = id3_field_getstrings(field, 0);
        genre = id3_ucs4_utf8duplicate (str);
        g_print ("genre: %s\n", genre);
    }

    frame = id3_tag_findframe (tag, ID3_FRAME_YEAR, 0);
    if (frame) {
        union id3_field const *field;
        unsigned int nstrings;

        field    = id3_frame_field(frame, 1);
        nstrings = id3_field_getnstrings(field);

        const id3_ucs4_t *str = id3_field_getstrings(field, 0);
        year = id3_ucs4_utf8duplicate (str);
        g_print ("year: %s\n", year);
    }

    gchar **trck = NULL;
    gchar *trackno = NULL;
    gchar *totaltracks = NULL;
    frame = id3_tag_findframe (tag, ID3_FRAME_TRACK, 0);
    if (frame) {
        union id3_field const *field;
        unsigned int nstrings;

        field    = id3_frame_field(frame, 1);
        nstrings = id3_field_getnstrings(field);

        const id3_ucs4_t *str = id3_field_getstrings(field, 0);
        track = id3_ucs4_utf8duplicate (str);
        trck = g_strsplit (track, "/", 0);
        if (trck[0])
            trackno = trck[0];
        if (trck[1])
            totaltracks = trck[1];
        g_print ("track: %s %s\n", trackno, totaltracks);
    }

    frame = id3_tag_findframe (tag, "TLEN", 0);
    if (frame) {
        union id3_field const *field;
        unsigned int nstrings;

        field    = id3_frame_field(frame, 1);
        nstrings = id3_field_getnstrings(field);

        const id3_ucs4_t *str = id3_field_getstrings(field, 0);
        length = id3_ucs4_utf8duplicate (str);
        g_print ("length: %s\n", length);
    }

    gchar** pos = NULL;
    frame = id3_tag_findframe (tag, "TPOS", 0);
    if (frame) {
        union id3_field const *field;
        unsigned int nstrings;

        field    = id3_frame_field(frame, 1);
        nstrings = id3_field_getnstrings(field);

        const id3_ucs4_t *str = id3_field_getstrings(field, 0);
        disc = id3_ucs4_utf8duplicate (str);
        pos = g_strsplit (disc, "/", 0);
        if (pos[0])
            discno = pos[0];
        if (pos[1])
            discs = pos[1];
        g_print ("disc: %s %s\n", discno, discs);
    }

    id3_file_close (tagfile);

    DinleMediaMetadata *md = g_object_new (DINLE_TYPE_MEDIA_METADATA,
                                           "title", title,
                                           "artist", artist,
                                           "album", album,
                                           "genre", genre,
                                           "year", year,
                                           "track", trackno,
                                           "tracks", totaltracks,
                                           "length", length,
                                           "discno", discno,
                                           "discs", discs,
                                           NULL);

    g_free (title);
    g_free (album);
    g_free (artist);
    g_free (genre);
    g_free (year);
    g_free (track);
    g_free (length);
    g_free (disc);
    g_strfreev (pos);
    g_strfreev (trck);

    return md;
}
