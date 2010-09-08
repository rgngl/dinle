bin_PROGRAMS = dinled

dinled_CFLAGS = -I$(top_srcdir)/common \
		$(AM_CPPFLAGS) $(AM_CFLAGS) $(DINLE_CFLAGS) \
                $(SQLITE_CFLAGS) \
		$(GLIB_CFLAGS)

dinled_LDADD = $(DINLE_LIBS) \
               $(GLIB_LIBS) \
	       $(SQLITE_LIBS)

dinled_headers = $(common_headers)\
		 dinle-archive-manager.h\
		 dinle-config-manager.h\
		 dinle-media-file.h\
		 dinle-media-metadata.h\
		 dinle-db.h\
		 dinle-db-sqlite.h\
		 dinle-session-manager.h\
		 dinle-session.h\
		 dinle-session-handler.h\
		 dinle-session-handler-auth.h\
		 dinle-session-handler-new.h\
		 dinle-session-handler-ready.h

dinled_SOURCES = $(dinled_headers)\
		 main.c\
		 dinle-archive-manager.c\
		 dinle-config-manager.c\
		 dinle-media-file.c\
		 dinle-media-metadata.c\
		 dinle-db.c\
		 dinle-db-sqlite.c\
		 dinle-session-manager.c\
		 dinle-session.c\
		 dinle-session-handler.c\
		 dinle-session-handler-auth.c\
		 dinle-session-handler-new.c\
		 dinle-session-handler-ready.c

if HAVE_MP3
dinled_SOURCES += dinle-media-file-mp3.c
dinled_headers += dinle-media-file-mp3.h
dinled_CFLAGS += $(ID3TAG_CFLAGS) $(MAD_CFLAGS)
dinled_LDADD += $(ID3TAG_LIBS) $(MAD_LIBS)
endif
