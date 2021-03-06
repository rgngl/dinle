AC_INIT([dinle], [0.01], [ustun.ergenoglu@gmail.com])
AM_INIT_AUTOMAKE([foreign -Werror])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile\
        src/Makefile\
        lib/Makefile\
        lib/src/Makefile\
        lib/include/Makefile\
        lib/include/dinle/Makefile\
        lib/example/Makefile])

dnl Programs
dnl ---------------------------------------------------------------------------
AC_PROG_CC_C99
AM_PROG_CC_C_O
PKG_PROG_PKG_CONFIG
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

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
AC_ARG_ENABLE(mp3,
	AS_HELP_STRING([--disable-mp3],
		[disable mp3 support (default: enable)]),,
	enable_mp3=yes)

AC_ARG_ENABLE(sqlite,
	AS_HELP_STRING([--enable-sqlite],
		[enable support for the SQLite database]),,
	[enable_sqlite=auto])
AC_ARG_ENABLE(ipv6,
        AS_HELP_STRING([--enable-ipv6],
                [enable IPV6 support]),,
        [enable_ipv6=no])
if test x$enable_ipv6 = xyes; then
    AC_DEFINE(HAVE_IPV6, 1, [Use IPv6])
fi

AC_SYS_LARGEFILE

dnl Mandatory Libraries
dnl ---------------------------------------------------------------------------
PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.20 gthread-2.0 gobject-2.0 gio-2.0],,
		[AC_MSG_ERROR([GLib 2.20 is required])])
PKG_CHECK_MODULES([SQLITE], [sqlite3],,
                [AC_MSG_ERROR([Sqlite is required])])

dnl LIBC Features
dnl ---------------------------------------------------------------------------
if test x$enable_largefile != xno; then
	AC_DEFINE([ENABLE_LARGEFILE], 1, [Define if large file support is enabled])
fi

dnl -------------------------------- mp3 support ------------------------------
if test x$enable_mp3 = xyes; then
	PKG_CHECK_MODULES([ID3TAG], [id3tag],,
		AC_CHECK_LIB(id3tag, id3_file_open,
			[ID3TAG_LIBS="-lid3tag -lz" ID3TAG_CFLAGS=""],
			enable_mp3=no))
        PKG_CHECK_MODULES([MAD], [mad],, enable_mp3=no)
fi

if test x$enable_mp3 = xyes; then
        AC_DEFINE(HAVE_MP3, 1, [Define to use mp3])
fi

AM_CONDITIONAL(HAVE_MP3, test x$enable_mp3 = xyes)

dnl ----------------------------------- GCC -----------------------------------
if test x$GCC = xyes
then
	#DINLE_CHECK_FLAG([-Wall])
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
