dnl Process this file with autoconf to produce a configure script.
AC_INIT([MGModeler], [0.1], [http://developer.berlios.de/bugs/?func=addbug&group_id=2445])

CFLAGS="$CFLAGS"
CXXFLAGS="$CXXFLAGS"

AC_CONFIG_AUX_DIR(admin)
dnl Setup for automake
AC_CONFIG_SRCDIR([src/main.cc])

dnl Detect the canonical host and target build environment
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

dnl Setup for automake
AM_INIT_AUTOMAKE([$PACKAGE_NAME], [$PACKAGE_VERSION])
AM_CONFIG_HEADER([src/config.h])

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CXX

AC_PROG_YACC
AM_PROG_LEX

AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_PROG_LN_S
AC_C_INLINE

dnl Checks for header files.
AC_HEADER_STDC

dnl Check for dlopen support
AC_DISABLE_STATIC
case "$target" in
	*-*-cygwin* | *-*-mingw32*)
        	AC_LIBTOOL_WIN32_DLL
		;;
esac
AC_PROG_LIBTOOL
		    

dnl Figure out which math library to use
case "$target" in
    *-*-cygwin* | *-*-mingw32*)
	MATHLIB=""
	SYS_GL_LIB="opengl32"
	SYS_GLU_LIB="glu32"
	;;
    *-*-beos*)
        MATHLIB=""
        SYS_GL_LIB="GL"
        SYS_GLU_LIB="GLU"
        ;;
    *-*-darwin*)
        MATHLIB=""
        SYS_GL_LIBS=""
        ;;
    *-*-aix*)
        if test x$ac_cv_prog_gcc = xyes; then
          SYS_GL_CFLAGS="-mthreads"
        fi
        SYS_GL_LIBS=""
        ;;
    *)
	dnl Checks for x11
	AC_PATH_X
	AC_PATH_XTRA
        MATHLIB="m"
        SYS_GL_LIB="GL"
        SYS_GLU_LIB="GLU"
        ;;
esac

LIBS="$LIBS $X_LIBS"
CFLAGS="$CFLAGS $X_CFLAGS"
CXXFLAGS="$CXXFLAGS $X_CFLAGS"

save_LIBS="$LIBS"
    LIBS="$LIBS -l$SYS_GL_LIB $X_LIBS"
    if test -n "$SYS_GL_LIB";then
       AC_MSG_CHECKING([for OpenGL($SYS_GL_LIB)])
       AC_TRY_LINK([
        #ifdef _WIN32
        #include <windows.h>
        #include <SDL_opengl.h>
        #else
        char glEnd();
        #endif
       ], [ glEnd(); ], has_opengl="yes", has_opengl="")
        if test x$has_opengl != x; then
          AC_MSG_RESULT([yes])
          GL_LIBS="-l$SYS_GL_LIB $GL_LIB"
        else
          AC_MSG_RESULT([no])
          AC_MSG_ERROR(OpenGL is not installed: missing $SYS_GL_LIB library)
        fi
    fi
    LIBS="$LIBS -l$SYS_GLU_LIB"
    if test -n "$SYS_GLU_LIB";then
       AC_MSG_CHECKING([for OpenGL Utility($SYS_GLU_LIB)])
       AC_TRY_LINK([
        #ifdef _WIN32
        #include <windows.h>
        #include <GL/gl.h>
        #include <GL/glu.h>
        #else
        char gluBeginPolygon(void *p);
        #endif
       ], [ gluBeginPolygon(0); ], has_glu="yes", has_glu="")
       if test x$has_glu != x; then
          AC_MSG_RESULT([yes])
          GL_LIBS="$GL_LIBS -l$SYS_GLU_LIB"
       else
          AC_MSG_RESULT([no])
          AC_MSG_ERROR(GLU is not installed: missing $SYS_GLU_LIB library)
       fi
    fi
    LIBS="$save_LIBS"
    if test -n "$MATHLIB";then
       AC_CHECK_LIB($MATHLIB, cos,:,
       AC_MSG_ERROR([ *** $MATHLIB is missing ***]))
       GL_LIBS="$GL_LIBS -l$MATHLIB"
    fi
    GL_CFLAGS="$SYS_GL_CFLAGS -DHAVE_OPENGL"
CFLAGS="$save_CFLAGS"

AC_SUBST(GL_CFLAGS)
AC_SUBST(GL_LIBS)


dnl Check for Qt
AC_ARG_WITH(qt-dir, [  --with-qt-dir           directory where Qt is installed ], [ qt_dir=`eval echo "$withval"/` ])

AC_MSG_CHECKING([whether QTDIR environment variable is set])
if test -z "$qt_dir"; then
   if test "x$QTDIR" = "x"; then
      AC_MSG_RESULT([no])
      AC_MSG_ERROR([QTDIR or --with-qt-dir must be properly set.])
   else
      qt_dir=$QTDIR
   fi
fi
AC_MSG_RESULT([$qt_dir])

dnl Checks for Qt library.
AC_ARG_WITH(qt-libraries, [  --with-qt-libraries           directory where Qt libraries are installed ], [ qt_libs=`eval echo "$withval"` ], [ qt_libs=$qt_dir/lib ])

AC_CACHE_CHECK([for Qt library],
  QT_LIBS, [
  for X in qt-mt qt; do
    if test "x$ac_qtlib" = "x"; then
      if test -f $qt_libs/lib$X.so -o -f $qt_libs/lib$X.a; then
        QT_LIBS="-L$qt_libs -l$X"
	break
      fi
    fi
  done
])
if test "x$QT_LIBS" = "x"; then
  AC_MSG_ERROR([Qt library not found. Maybe QTDIR isn't properly set.])
fi
AC_SUBST(QT_LIBS)

dnl Check for Qt multi-thread support.
if test "x$ac_qtlib" = "xqt-mt"; then
  QT_THREADS="thread"
fi
AC_SUBST(QT_THREADS)

dnl Checks for Qt includes.
AC_ARG_WITH(qt-includes, [  --with-qt-includes           directory where Qt includes are installed ], [ qt_includes=`eval echo "$withval"` ], [ qt_includes=$qt_dir/include ])
QT_CFLAGS="-I$qt_includes"

AC_LANG_PUSH([C++])

save_CXXFLAGS="$CXXFLAGS"
save_CPPFLAGS="$CPPFLAGS"
CXXFLAGS="$CXXFLAGS $QT_CFLAGS"
CPPFLAGS="$CPPFLAGS -I$qt_includes"
AC_CHECK_HEADERS([qapplication.h],,AC_MSG_ERROR([Qt headers not found.]))
CPPFLAGS="$save_CPPFLAGS"

AC_SUBST(QT_CFLAGS)

save_LIBS="$LIBS"
LIBS="$QT_LIBS"

AC_CACHE_CHECK([for Qt library version >= 3.1.1],
  ac_qtlib_version, [
  AC_TRY_LINK([#include "qglobal.h"], [
#if QT_VERSION < 0x030101
#error Qt library 3.1.1 or greater required.
#endif
    ],
    ac_qtlib_version="yes", [
      echo "no; Qt 3.1.1 or greater is required"
      exit
    ])
])

AC_CACHE_CHECK([for QT OpenGL support],
  ac_qtopengl, [
  AC_TRY_COMPILE([#include "qgl.h"
], [
#ifndef GL_TRUE
#error Cannot find QT OpenGL support
#endif
  ],
  ac_qtopengl=yes, [ 
    echo "no"
    exit
  ])
])
CXXFLAGS="$save_CXXFLAGS"
LIBS="$save_LIBS"
AC_LANG_POP

dnl A common error message:
ac_qtdir_errmsg="not found in current PATH. Maybe QT development environment isn't available (qt3-devel)."

dnl Check for Qt moc utility.
AC_PATH_PROG(MOC, moc, [no], $qt_dir/bin:${PATH})
if test "x$MOC" = "xno"; then
   AC_MSG_ERROR([moc $ac_qtdir_errmsg])
fi
AC_SUBST(MOC)

dnl Check for Qt uic utility.
AC_PATH_PROG(UIC, uic, [no], $qt_dir/bin:${PATH})
if test "x$UIC" = "xno"; then
   AC_MSG_ERROR([uic $ac_qtdir_errmsg])
fi
AC_SUBST(UIC)

dnl Modify CFLAGS after all tests
dnl Add args to configure
AC_ARG_ENABLE(debug,   [  --disable-debug          Enable debugging information],
        USE_DEBUG="no", USE_DEBUG="yes")

AC_ARG_ENABLE(optimise,[  --enable-optimise       Enable code optimising],
	USE_OPTIMISE="$enableval", USE_OPTIMISE="no")

AC_ARG_ENABLE(profile, [  --enable-profile        Enable code profiling],
	USE_PROFILE="$enableval", USE_PROFILE="no")

dnl Flags
AC_TRY_CFLAGS("-Wall", wall="-Wall", wall="")
CFLAGS="$CFLAGS $wall"
CXXFLAGS="$CXXFLAGS $wall"
dnl Support for debug
if test "$USE_DEBUG" = yes
then
	CFLAGS="$CFLAGS -g"
	CXXFLAGS="$CXXFLAGS -g"
fi

dnl Support for profiling
if test "$USE_PROFILE" = yes
then
	CFLAGS="$CFLAGS -pg"
	CXXFLAGS="$CXXFLAGS -pg"
fi

dnl Support for optimize
if test "$USE_OPTIMIZE" = yes
then
	CFLAGS="$CFLAGS -O3"
	CXXFLAGS="$CXXFLAGS -O3"
        LDFLAGS="$LDFLAGS -s"
fi

dnl Warning as error
AC_ARG_ENABLE(werror, [  --enable-werror                Make all warnings into errors], CFLAGS="$CFLAGS -Werror" CXXFLAGS="$CXXFLAGS -Werror",:)


dnl ---------------------------------------------
dnl It seems automake 1.5 don't take care about this script
dnl ---------------------------------------------
if test ! -z "$am_depcomp"; then
        DEPCOMP="depcomp"
fi
AC_SUBST(DEPCOMP)

AC_OUTPUT([Makefile src/Makefile icons/Makefile plugins/Makefile plugins/Objects/Makefile plugins/IO/Makefile wrl/Makefile])
