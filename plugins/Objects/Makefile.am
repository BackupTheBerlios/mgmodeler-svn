pluginsdir=@libdir@/@PACKAGE_NAME@-@VERSION@
plugins_LTLIBRARIES=01_polyline.la 02_circle.la 03_nurbs.la

INCLUDES=-I../../src

AM_CXXFLAGS=@QT_CFLAGS@

01_polyline_la_SOURCES=polyline.cc
01_polyline_la_LDFLAGS=-module -avoid-version
02_circle_la_SOURCES=circle.cc
02_circle_la_LDFLAGS=-module -avoid-version
03_nurbs_la_SOURCES=nurbs.cc
03_nurbs_la_LDFLAGS=-module -avoid-version

