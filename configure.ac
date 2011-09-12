# bump micro_version to the next even number for each point release
# bump micro_version to the next odd number after each release
m4_define([controller_major_version], [0])
m4_define([controller_minor_version], [1])
m4_define([controller_micro_version], [1])

m4_define([controller_version], [controller_major_version.controller_minor_version.controller_micro_version])

m4_define([controller_release_status],
          [m4_if(m4_eval(controller_micro_version % 2), [1], [git],
                 [m4_if(m4_eval(controller_minor_version % 2), [1], [snapshot],
                                                              [release])])])

# bump up by 1 for every micro release with no API changes, otherwise
# set to 0. after release, bump up by 1
m4_define([controller_interface_age], [0])
m4_define([controller_binary_age], [m4_eval(100 * controller_minor_version + controller_micro_version)])

m4_define([lt_current], [m4_eval(100 * controller_minor_version + controller_micro_version - controller_interface_age)])
m4_define([lt_revision], [controller_interface_age])
m4_define([lt_age], [m4_eval(controller_binary_age - controller_interface_age)])

m4_define([glib_req_version], [2.28])

AC_PREREQ([2.63])

AC_INIT([glib-controller],
        [controller_version],
        [],
        [glib-controller])

AM_CONFIG_HEADER([config.h])
AC_CONFIG_SRCDIR([glib-controller/glib-controller.h])
AC_CONFIG_MACRO_DIR([build])

AM_INIT_AUTOMAKE([1.11 no-define -Wno-portability dist-bzip2 foreign])

AM_SILENT_RULES([yes])

# Check for programs
AC_PROG_CC_C_O

AM_PATH_GLIB_2_0

# Initialize libtool
LT_PREREQ([2.2.6])
LT_INIT([disable-static])


GCONTROLLER_MAJOR_VERSION=controller_major_version
GCONTROLLER_MINOR_VERSION=controller_minor_version
GCONTROLLER_MICRO_VERSION=controller_micro_version
GCONTROLLER_VERSION=controller_version
GCONTROLLER_RELEASE_STATUS=controller_release_status
AC_SUBST(GCONTROLLER_MAJOR_VERSION)
AC_SUBST(GCONTROLLER_MICRO_VERSION)
AC_SUBST(GCONTROLLER_MINOR_VERSION)
AC_SUBST(GCONTROLLER_VERSION)
AC_SUBST(GCONTROLLER_RELEASE_STATUS)

GCONTROLLER_LT_CURRENT=lt_current
GCONTROLLER_LT_REVISION=lt_revision
GCONTROLLER_LT_AGE=lt_age
GCONTROLLER_LT_VERSION="$GCONTROLLER_LT_CURRENT:$GCONTROLLER_LT_REVISION:$GCONTROLLER_LT_AGE"

GCONTROLLER_LDFLAGS="-version-info $GCONTROLLER_LT_VERSION"
AC_SUBST(GCONTROLLER_LDFLAGS)

GLIB_PREFIX="`$PKG_CONFIG --variable=prefix gobject-2.0`"
AC_SUBST(GLIB_PREFIX)

PKG_CHECK_MODULES(GCONTROLLER, [gobject-2.0 >= glib_req_version])
AC_SUBST(GCONTROLLER_CFLAGS)
AC_SUBST(GCONTROLLER_LIBS)

dnl = Enable debug level ======================================================

m4_define([debug_default], [m4_if(m4_eval(controller_minor_version % 2), [1], [yes], [minimum])])

AC_ARG_ENABLE([debug],
              [AC_HELP_STRING([--enable-debug=@<:@no/minimum/yes@:>@],
                              [turn on debugging @<:@default=debug_default@:>@])],
              [],
              [enable_debug=debug_default])

AS_CASE([$enable_debug],

        [yes],
        [
          test "$cflags_set" = set || CFLAGS="$CFLAGS -g"
          GCONTROLLER_DEBUG_CFLAGS="-DGCONTROLLER_ENABLE_DEBUG"
        ],

        [minimum],
        [
          GCONTROLLER_DEBUG_CFLAGS="-DGCONTROLLER_ENABLE_DEBUG -DG_DISABLE_CAST_CHECKS"
        ],

        [no],
        [
          GCONTROLLER_DEBUG_CFLAGS="-DG_DISABLE_ASSERT -DG_DISABLE_CHECKS -DG_DISABLE_CAST_CHECKS"
        ],

        [AC_MSG_ERROR([Unknown argument to --enable-debug])]
)

AC_SUBST(GCONTROLLER_DEBUG_CFLAGS)

dnl = Enable strict compiler flags ============================================

# use strict compiler flags only on development releases
m4_define([maintainer_flags_default], [m4_if(m4_eval(controller_minor_version % 2), [1], [yes], [no])])
AC_ARG_ENABLE([maintainer-flags],
              [AC_HELP_STRING([--enable-maintainer-flags=@<:@no/yes@:>@],
                              [Use strict compiler flags @<:@default=maintainer_flags_default@:>@])],
              [],
              [enable_maintainer_flags=maintainer_flags_default])

AS_IF([test "x$enable_maintainer_flags" = "xyes" && test "x$GCC" = "xyes"],
      [
        AS_COMPILER_FLAGS([MAINTAINER_CFLAGS],
                          ["-Wall -Wshadow -Wcast-align
                            -Wno-uninitialized -Wformat-security
                            -Wno-strict-aliasing -Winit-self
                            -Wempty-body -Wdeclaration-after-statement
                            -Wno-strict-aliasing -Wno-strict-aliasing"])
      ]
)
AC_SUBST([MAINTAINER_CFLAGS])

dnl = GCOV coverage checks ====================================================
AC_MSG_CHECKING([whether to build with gcov testing])
AC_ARG_ENABLE([gcov],
              [AS_HELP_STRING([--enable-gcov],
                              [Whether to enable coverage testing (requires gcc and gcov)])],
              [],
              [enable_gcov=no])

AS_IF([test "x$enable_gcov" = "xyes" && test "x$GCC" = "xyes"],
      [
        GCOV_CFLAGS="$CFLAGS -O0 -g -fprofile-arcs -ftest-coverage"
        GCOV_LDFLAGS="$LDFLAGS -lgcov"
      ]
)

AM_CONDITIONAL([GCOV_ENABLED], [test "x$enable_gcov" = "xyes"])
AC_SUBST([GCOV_CFLAGS])
AC_SUBST([GCOV_LDFLAGS])
AC_MSG_RESULT([$enable_gcov])

GTK_DOC_CHECK([1.13],[--flavour no-tmpl])

GOBJECT_INTROSPECTION_CHECK([0.6.5])

AC_CONFIG_FILES([
        Makefile
        glib-controller.pc
        glib-controller/gcontrollerversion.h
        doc/Makefile
        doc/reference/Makefile
        doc/reference/version.xml
])

AC_OUTPUT

dnl === Summary ===============================================================

echo ""
echo " GLib-Controller - $VERSION (${GCONTROLLER_RELEASE_STATUS})"
echo ""
echo " • Prefix: ${prefix}"
echo ""
echo " • Debug level: ${enable_debug}"
echo " • Compiler flags: ${MAINTAINER_CFLAGS}"
echo ""
echo " • API reference: ${enable_gtk_doc}"
echo ""
echo " • Build introspection data: ${enable_introspection}"
echo " • Enable coverage test: ${enable_gcov}"
echo ""
