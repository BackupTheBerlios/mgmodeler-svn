pluginsdir=@libdir@/@PACKAGE_NAME@-@VERSION@
iconesdir=@datadir@/@PACKAGE_NAME@
plugins_LTLIBRARIES=01_polyline.la 02_circle.la 03_nurbs.la

icones=circle.png  nurbs.png  polyline.png

pkgdata_DATA=${icones}
EXTRA_DIST=${icones}


INCLUDES=-I../../src

AM_CXXFLAGS=@QT_CFLAGS@

01_polyline_la_SOURCES=polyline.cc polyline.h
01_polyline_la_LDFLAGS=-module -avoid-version
02_circle_la_SOURCES=circle.cc
02_circle_la_LDFLAGS=-module -avoid-version
03_nurbs_la_SOURCES=nurbs.cc polyline.h
03_nurbs_la_LDFLAGS=-module -avoid-version

