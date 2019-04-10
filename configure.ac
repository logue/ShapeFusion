dnl Process this file with autoconf to produce a configure script.
dnl Written in 2000 by Christian Bauer <Christian.Bauer@uni-mainz.de>

AC_INIT([ShapeFusion], [0.6b1], [https://sf.net/p/shapefusion/bugs/], [shapefusion], [http://shapefusion.sf.net/])
AC_CONFIG_MACRO_DIR([m4])
AC_PREREQ([2.65])

dnl Detect the canonical host and target build environment.
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

dnl Init automake.
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(config.h)

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_INSTALL
AC_PROG_RANLIB

dnl Require C++11 or newer and enable C++11 if needed.
AX_CXX_COMPILE_STDCXX([11], [noext], [mandatory])

dnl some platform specific stuff
AS_CASE([$target],
        [*-*-mingw32*],
        [ make_windows=true
          LDFLAGS="$LDFLAGS -Wl,-subsystem,windows"
          LIBS="$LIBS -lstdc++"
          AC_CHECK_TOOL(WINDRES, windres, :) ],
        [*-*-netbsd*],
        [
          CPPFLAGS="$CPPFLAGS -I/usr/pkg/include"]
)
AM_CONDITIONAL([MAKE_WINDOWS], [test "x$make_windows" = "xtrue"])

dnl wxWidgets
AM_OPTIONS_WXCONFIG
reqwx=3.0.0
AM_PATH_WXCONFIG($reqwx, wxWin=1)
if test "$wxWin" != 1; then
    AC_MSG_ERROR([
        wxWidgets must be installed on your system.

        Please check that wx-config is in path, the directory
        where wxWidgets libraries are installed (returned by
        'wx-config --libs' or 'wx-config --static --libs' command)
        is in LD_LIBRARY_PATH or equivalent variable and
        wxWidgets version is $reqwx or above.
    ])
else
   CPPFLAGS="$CPPFLAGS $WX_CPPFLAGS"
   CXXFLAGS="$CXXFLAGS $WX_CXXFLAGS_ONLY"
   CFLAGS="$CFLAGS $WX_CFLAGS_ONLY"
   LIBS="$LIBS $WX_LIBS"
fi

PKG_CHECK_MODULES(SNDFILE, sndfile, LIBS="-lsndfile $LIBS", AC_ERROR([ShapeFusion requires libsndfile]))
AC_SUBST(SNDFILE_CFLAGS)
AC_SUBST(SNDFILE_LIBS)

PKG_CHECK_MODULES([SNDFILE], [sndfile], [
		  CPPFLAGS="$SNDFILE_CFLAGS $CPPFLAGS"
		  LIBS="$SNDFILE_LIBS $LIBS"], AC_ERROR([ShapeFusion requires libsndfile]))

dnl Check for libsndfile
dnl AC_CHECK_HEADER(sndfile.h, , AC_ERROR([ShapeFusion requires libsndfile]))
dnl AC_CHECK_LIB(sndfile, sf_open, [LIBS="-lvorbis -lvorbisenc -lvorbisfile -logg -lsndfile $LIBS"], AC_ERROR([ShapeFusion requires libsndfile]), [-lvorbis -lvorbisenc -lvorbisfile -logg -lFLAC -lsndfile])

dnl Generate Makefiles.
AC_CONFIG_FILES([
Makefile
Physics/Makefile
Shapes/Makefile
Sounds/Makefile
])
AC_OUTPUT

dnl Print summary.
echo "Configuration done. Now type \"make\"."
