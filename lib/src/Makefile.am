lib_LTLIBRARIES = libdinleclient.la

libdinleclient_la_CFLAGS = -I$(top_srcdir)/common \
			   -I$(top_srcdir)/lib/include\
			   $(AM_CPPFLAGS) $(AM_CFLAGS)\
			   $(GLIB_CFLAGS)

libdinleclient_la_SOURCES = dinle.c

libdinleclient_la_LDFLAGS = -version-info 1:0:1
