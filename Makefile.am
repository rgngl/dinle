ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = foreign dist-bzip2 subdir-objects

AM_CPPFLAGS = -I$(srcdir)/src

common_headers = common/dinle-tags.h \
		 common/dinle-commands.h

SUBDIRS = src lib
DIST_SUBDIRS = $(SUBDIRS)

#
# Distribution
#

EXTRA_DIST = autogen.sh $(common_headers)
