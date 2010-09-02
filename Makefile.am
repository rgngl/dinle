ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = foreign dist-bzip2 subdir-objects

AM_CFLAGS = -I$(srcdir)/src -DHEDE -I$(srcdir)/include

exported_headers = include/dinle-tags.h \
		   include/dinle-commands.h

SUBDIRS = src
DIST_SUBDIRS = $(SUBDIRS) include

#
# Distribution
#

EXTRA_DIST = autogen.sh
