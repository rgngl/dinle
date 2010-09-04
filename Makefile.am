ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = foreign dist-bzip2 subdir-objects

AM_CPPFLAGS = -I$(srcdir)/src

exported_headers = include/dinle-tags.h \
		   include/dinle-commands.h

SUBDIRS = src include
DIST_SUBDIRS = $(SUBDIRS)

#
# Distribution
#

EXTRA_DIST = autogen.sh
