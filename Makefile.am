# preamble
NULL =

lib_LTLIBRARIES =
noinst_HEADERS =

BUILT_SOURCES =

CLEANFILES =
DISTCLEANFILES =

EXTRA_DIST =

TEST_PROGS =
check_PROGRAMS = $(TEST_PROGS)

# unfortunately, gtk-doc has issues with non-recursive autotools
SUBDIRS = . doc

# silent rules
include $(top_srcdir)/build/Makefile.am.silent

# pkg-config rules
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = glib-controller-1.0.pc
EXTRA_DIST += glib-controller.pc.in
CLEANFILES += glib-controller-1.0.pc

glib-controller-1.0.pc: glib-controller.pc
	$(QUIET_CP)cp -f $< $(@F)

# main library
lib_LTLIBRARIES += libglib-controller-1.0.la

source_c = \
	glib-controller/garraycontroller.c 	\
	glib-controller/gcontrollerreference.c	\
	glib-controller/gcontroller.c 		\
	glib-controller/ghashcontroller.c	\
	$(NULL)

source_public_h = \
	glib-controller/garraycontroller.h	\
	glib-controller/gcontrollerreference.h	\
	glib-controller/gcontrollertypes.h	\
	glib-controller/gcontroller.h		\
	glib-controller/ghashcontroller.h	\
	$(NULL)

source_h = \
	$(source_public_h)			\
	glib-controller/gcontrollerversion.h	\
	glib-controller/glib-controller.h	\
	$(NULL)

# marshallers
glib_marshal_list = glib-controller/gcontrollermarshal.list
glib_marshal_prefix = _gcontroller_marshal
include $(top_srcdir)/build/Makefile.am.marshal

# enumerations
glib_enum_h = glib-controller/gcontrollerenumtypes.h
glib_enum_c = glib-controller/gcontrollerenumtypes.c
glib_enum_headers = $(source_public_h)
include $(top_srcdir)/build/Makefile.am.enums

libglib_controller_1_0_la_CPPFLAGS = 	\
	-I$(top_srcdir)			\
	-I$(top_srcdir)/glib-controller	\
	-I$(top_builddir)		\
	-DG_LOG_DOMAIN=\"GController\" 	\
	-DG_DISABLE_DEPRECATED 		\
	-DG_DISABLE_SINGLE_INCLUDES 	\
	-DGLIB_CONTROLLER_COMPILATION	\
	$(AM_CPPFLAGS) 			\
	$(GCONTROLLER_DEBUG_CFLAGS)	\
	$(NULL)
libglib_controller_1_0_la_CFLAGS = 	\
	$(GCONTROLLER_CFLAGS) 		\
	$(MAINTAINER_CFLAGS) 		\
	$(GCOV_CFLAGS)			\
	$(NULL)
libglib_controller_1_0_la_LDFLAGS = 	\
	-export-symbols-regex "^[^_]"	\
	-no-undefined 			\
	$(GCONTROLLER_LDFLAGS)		\
	$(NULL)
libglib_controller_1_0_la_LIBADD = $(GCONTROLLER_LIBS) $(GCOV_LIBS)
libglib_controller_1_0_la_SOURCES = $(BUILT_SOURCES) $(source_c) $(source_h)
libglib_controller_HEADERS = $(source_h)
libglib_controllerdir = $(includedir)/glib-controller/1.0

# GTest rules
include $(top_srcdir)/build/Makefile.am.gtest

TEST_PROGS += array-controller
array_controller_SOURCES = glib-controller/tests/array-controller.c
array_controller_CPPFLAGS = $(AM_CPPFLAGS) $(GCONTROLLER_DEBUG_CFLAGS) -I$(top_srcdir) -I$(top_srcdir)/glib-controller -I$(top_builddir)
array_controller_CFLAGS = $(GCONTROLLER_CFLAGS) $(MAINTAINER_CFLAGS)
array_controller_LDADD = $(GCONTROLLER_LIBS) $(top_builddir)/libglib-controller-1.0.la

TEST_PROGS += hash-controller
hash_controller_SOURCES = glib-controller/tests/hash-controller.c
hash_controller_CPPFLAGS = $(AM_CPPFLAGS) $(GCONTROLLER_DEBUG_CFLAGS) -I$(top_srcdir) -I$(top_srcdir)/glib-controller -I$(top_builddir)
hash_controller_CFLAGS = $(GCONTROLLER_CFLAGS) $(MAINTAINER_CFLAGS)
hash_controller_LDADD = $(GCONTROLLER_LIBS) $(top_builddir)/libglib-controller-1.0.la

# GCov rules
#include $(top_srcdir)/build/Makefile.am.gcov