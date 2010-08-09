bin_PROGRAMS = dinled

dinled_CFLAGS = $(AM_CFLAGS) $(DINLE_CFLAGS) \
                    $(SQLITE_CFLAGS) \
		    $(ID3TAG_CFLAGS) \
		    $(GLIB_CFLAGS)

dinled_LDADD = $(DINLE_LIBS) \
                   $(GLIB_LIBS) \
		   $(SQLITE_LIBS) \
		   $(ID3TAG_LIBS)

dinled_SOURCES = main.c
