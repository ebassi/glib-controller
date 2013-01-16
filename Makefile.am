ACLOCAL_AMFLAGS = -I build ${ACLOCAL_FLAGS}

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

noinst_PROGRAMS =

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
	glib-controller/gcontrollerevent.c	\
	glib-controller/gcontroller.c 		\
	glib-controller/ghashcontroller.c	\
	glib-controller/gindexset.c		\
	glib-controller/gptrarraycontroller.c	\
	glib-controller/grange.c		\
	$(NULL)

source_public_h = \
	glib-controller/garraycontroller.h	\
	glib-controller/gcontrollerevent.h	\
	glib-controller/gcontrollertypes.h	\
	glib-controller/gcontroller.h		\
	glib-controller/ghashcontroller.h	\
	glib-controller/gindexset.h		\
	glib-controller/gptrarraycontroller.h	\
	glib-controller/grange.h		\
	$(NULL)

source_h = \
	$(source_public_h)			\
	glib-controller/gcontrollerversion.h	\
	glib-controller/glib-controller.h	\
	glib-controller/gcontrollerenumtypes.h	\
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
libglib_controllerdir = $(includedir)/glib-controller/1.0/glib-controller

# Introspection
-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(top_srcdir)/glib-controller
INTROSPECTION_COMPILER_ARGS = --includedir=$(top_srcdir) --includedir=$(top_builddir)

if HAVE_INTROSPECTION
GController-1.0.gir: $(INTROSPECTION_SCANNER) $(top_builddir)/libglib-controller-1.0.la

GController_1_0_gir_SCANNERFLAGS = --identifier-prefix=G --symbol-prefix=g --c-include='glib-controller/glib-controller.h' --pkg-export glib-controller-1.0 --warn-all
GController_1_0_gir_INCLUDES = GObject-2.0
GController_1_0_gir_CFLAGS = -I$(top_srcdir) -I$(top_srcdir)/glib-controller -I$(top_builddir) -DG_DISABLE_DEPRECATED -DGLIB_CONTROLLER_COMPILATION
GController_1_0_gir_LIBS = $(top_builddir)/libglib-controller-1.0.la
GController_1_0_gir_FILES = $(BUILT_SOURCES) $(source_h) $(source_c)

INTROSPECTION_GIRS += GController-1.0.gir
endif

girdir = $(datadir)/gir-1.0
dist_gir_DATA = $(INTROSPECTION_GIRS)

typelibsdir = $(libdir)/girepository-1.0
typelibs_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(dist_gir_DATA) $(typelibs_DATA)

# GTest rules
include $(top_srcdir)/build/Makefile.am.gtest

test_cppflags = $(AM_CPPFLAGS) $(GCONTROLLER_DEBUG_CFLAGS) -DG_DISABLE_DEPRECATION_WARNINGS -I$(top_srcdir) -I$(top_srcdir)/glib-controller -I$(top_builddir)
test_cflags = $(GCONTROLLER_CFLAGS) $(MAINTAINER_CFLAGS)
test_ldadd = $(GCONTROLLER_LIBS) $(top_builddir)/libglib-controller-1.0.la

TEST_PROGS += array-controller
array_controller_SOURCES = glib-controller/tests/array-controller.c
array_controller_CPPFLAGS = $(test_cppflags)
array_controller_CFLAGS = $(test_cflags)
array_controller_LDADD = $(test_ldadd)

TEST_PROGS += hash-controller
hash_controller_SOURCES = glib-controller/tests/hash-controller.c
hash_controller_CPPFLAGS = $(test_cppflags)
hash_controller_CFLAGS = $(test_cflags)
hash_controller_LDADD = $(test_ldadd)

TEST_PROGS += index-set
index_set_SOURCES = glib-controller/tests/index-set.c
index_set_CPPFLAGS = $(test_cppflags)
index_set_CFLAGS = $(test_cflags)
index_set_LDADD = $(test_ldadd)

# Examples
noinst_PROGRAMS += simple-model
simple_model_SOURCES = examples/simple-model.c
simple_model_CPPFLAGS = $(AM_CPPFLAGS) $(GCONTROLLER_DEBUG_CFLAGS) -I$(top_srcdir) -I$(top_srcdir)/glib-controller -I$(top_builddir)
simple_model_CFLAGS = $(GCONTROLLER_CFLAGS) $(MAINTAINER_CFLAGS)
simple_model_LDADD = $(GCONTROLLER_LIBS) $(top_builddir)/libglib-controller-1.0.la

# GCov rules
#include $(top_srcdir)/build/Makefile.am.gcov
