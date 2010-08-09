AC_INIT([dinle], [0.01], [ustun.ergenoglu@gmail.com])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])

dnl Programs
dnl ---------------------------------------------------------------------------
AC_PROG_CC_C99
AM_PROG_CC_C_O
PKG_PROG_PKG_CONFIG
AC_PROG_INSTALL
AC_PROG_MAKE_SET

dnl Declare Variables
dnl ---------------------------------------------------------------------------
AC_SUBST(AM_CFLAGS,"")

AC_SUBST(DINLED_LIBS)
AC_SUBST(DINLED_CFLAGS)
DINLE_LIBS=""
DINLE_CFLAGS=""

dnl OS Specific - Ripped of from MPD source
dnl ---------------------------------------------------------------------------
AC_CANONICAL_HOST

case "$host_os" in
mingw32* | windows*)
	DINLE_LIBS="$DINLE_LIBS -lws2_32"
	;;
esac

if test -z "$prefix" || test "x$prefix" = xNONE; then
	local_lib=
	local_include=

	# aren't autotools supposed to be smart enough to figure this out?  oh
	# well, the git-core Makefile managed to do some of the work for us :)
	case "$host_os" in
	darwin*)
		local_lib='/sw/lib /opt/local/lib'
		local_include='/sw/include /opt/local/include'
		;;
	freebsd* | openbsd*)
		local_lib=/usr/local/lib
		local_include=/usr/local/include
		;;
	netbsd*)
		local_lib=/usr/pkg/lib
		local_include=/usr/pkg/include
		LDFLAGS="$LDFLAGS -Wl,-rpath,/usr/pkg/lib"
		;;
	esac

	for d in $local_lib; do
		if test -d "$d"; then
			LDFLAGS="$LDFLAGS -L$d"
			break
		fi
	done
	for d in $local_include; do
		if test -d "$d"; then
			CFLAGS="$CFLAGS -I$d"
			break
		fi
	done
fi

dnl Header/Library Checks
dnl ---------------------------------------------------------------------------
AC_CHECK_FUNCS(daemon fork)

AC_CHECK_LIB(m,exp,DINLE_LIBS="$DINLE_LIBS -lm",)

AC_CHECK_HEADERS(locale.h)

dnl Argument checks
dnl ---------------------------------------------------------------------------
AC_ARG_ENABLE(id3,
	AS_HELP_STRING([--disable-id3],
		[disable id3 support (default: enable)]),,
	enable_id3=yes)

AC_ARG_ENABLE(sqlite,
	AS_HELP_STRING([--enable-sqlite],
		[enable support for the SQLite database]),,
	[enable_sqlite=auto])

dnl Mandatory Libraries
dnl ---------------------------------------------------------------------------
PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.20 gthread-2.0],,
		[AC_MSG_ERROR([GLib 2.12 is required])])

dnl LIBC Features
dnl ---------------------------------------------------------------------------
if test x$enable_largefile != xno; then
	AC_DEFINE([ENABLE_LARGEFILE], 1, [Define if large file support is enabled])
fi

dnl -------------------------------- libid3tag --------------------------------
if test x$enable_id3 = xyes; then
	PKG_CHECK_MODULES([ID3TAG], [id3tag],,
		AC_CHECK_LIB(id3tag, id3_file_open,
			[ID3TAG_LIBS="-lid3tag -lz" ID3TAG_CFLAGS=""],
			enable_id3=no))
fi

if test x$enable_id3 = xyes; then
	AC_DEFINE(HAVE_ID3TAG, 1, [Define to use id3tag])
fi

AM_CONDITIONAL(HAVE_ID3TAG, test x$enable_id3 = xyes)

dnl ---------------------------------- sqlite ---------------------------------

DINLE_AUTO_PKG(sqlite, SQLITE, [sqlite3],
	[SQLite database support], [sqlite not found])
if test x$enable_sqlite = xyes; then
	AC_DEFINE([ENABLE_SQLITE], 1, [Define to enable sqlite database support])
fi

AM_CONDITIONAL(ENABLE_SQLITE, test x$enable_sqlite = xyes)

dnl ----------------------------------- GCC -----------------------------------
if test x$GCC = xyes
then
	DINLE_CHECK_FLAG([-Wall])
	DINLE_CHECK_FLAG([-Wextra])
	DINLE_CHECK_FLAG([-Wno-deprecated-declarations])
	DINLE_CHECK_FLAG([-Wmissing-prototypes])
	DINLE_CHECK_FLAG([-Wshadow])
	DINLE_CHECK_FLAG([-Wpointer-arith])
	DINLE_CHECK_FLAG([-Wstrict-prototypes])
	DINLE_CHECK_FLAG([-Wcast-qual])
	DINLE_CHECK_FLAG([-Wwrite-strings])
	DINLE_CHECK_FLAG([-pedantic])
fi

AC_OUTPUT
