bin_PROGRAMS = dinled

dinled_CFLAGS = $(AM_CFLAGS) $(DINLE_CFLAGS) \
                    $(SQLITE_CFLAGS) \
		    $(GLIB_CFLAGS)

dinled_LDADD = $(DINLE_LIBS) \
               $(GLIB_LIBS) \
	       $(SQLITE_LIBS)

dinled_SOURCES = main.c \
		 dinle-archive-manager.c \
		 dinle-config-manager.c \
		 dinle-media-file.c \
		 dinle-media-metadata.c \
		 dinle-db.c \
		 dinle-db-sqlite.c

dinled_headers = dinle-archive-manager.h \
		 dinle-config-manager.h \
		 dinle-media-file.h \
		 dinle-media-metadata.h \
		 dinle-db.h \
		 dinle-db-sqlite.h

if HAVE_MP3
dinled_SOURCES += dinle-media-file-mp3.c
dinled_headers += dinle-media-file-mp3.h
dinled_CFLAGS += $(ID3TAG_CFLAGS) $(MAD_CFLAGS)
dinled_LDADD += $(ID3TAG_LIBS) $(MAD_LIBS)
endif
