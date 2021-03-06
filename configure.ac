AC_PREREQ(2.59)
AC_INIT(
	HoverRace,
	[m4_normalize(esyscmd(build/mkversion.sh))],
	[http://svn.igglybob.com/hoverrace/])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR(m4)
AC_CONFIG_SRCDIR([engine/Util/FuzzyLogic.h])
AM_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE
AM_SILENT_RULES

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
LT_INIT([disable-static])
AC_SUBST([LIBTOOL_DEPS])
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION(0.17)

# pthread support.
AX_PTHREAD(, [
	AC_MSG_ERROR(["Could not determine thread support."])
])
LIBS="$PTHREAD_LIBS $LIBS"
CXXFLAGS="$CXXFLAGS $PTHREAD_CFLAGS"
CC="$PTHREAD_CC"

# Checks for libraries.
AM_ICONV
BOOST_REQUIRE(1.34)
BOOST_ASIO
BOOST_FILESYSTEM
BOOST_SMART_PTR
BOOST_THREADS
LUA51_REQUIRE
LUABIND_REQUIRE
PKG_CHECK_MODULES(DEPS, [
	freealut >= 1.1
	libcurl >= 7.18
	openal >= 0.0.8
	OIS >= 1.2.0
	sdl >= 1.2.10
])
AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)

dnl TODO: Make LibYAML location configurable.
YAML_REQUIRE(0.1.1)

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_BIGENDIAN
AC_TYPE_LONG_LONG_INT

# Checks for library functions.
AC_CHECK_FUNC(setenv,
	[AC_DEFINE([HAVE_SETENV], [1],
		[Define to 1 if you have the setenv() function.])])

PLATFORM_NAME=`uname -s`
AC_DEFINE_UNQUOTED([PLATFORM_NAME], ["$PLATFORM_NAME"], [Platform name.])

# Additional global flags.
HR_CPPFLAGS="-DWITH_OPENAL -DWITH_SDL $BOOST_CPPFLAGS $DEPS_CFLAGS $YAML_CPPFLAGS $LUA_CFLAGS $LUABIND_CPPFLAGS"
AC_SUBST(HR_CPPFLAGS)
HR_CXXFLAGS="-Winvalid-pch"
AC_SUBST(HR_CXXFLAGS)

AC_CONFIG_FILES([
	Makefile
	client/Makefile
	client/Game2/Makefile
	compilers/Makefile
	compilers/ParcelDump/Makefile
	engine/Makefile
	engine/ColorTools/Makefile
	engine/MainCharacter/Makefile
	engine/Model/Makefile
	engine/Net/Makefile
	engine/ObjFac1/Makefile
	engine/ObjFacTools/Makefile
	engine/Parcel/Makefile
	engine/Script/Makefile
	engine/Util/Makefile
	engine/VideoServices/Makefile
	share/Makefile
	po/Makefile.in
])
AC_OUTPUT
