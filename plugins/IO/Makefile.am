pluginsdir=@libdir@/@PACKAGE_NAME@-@VERSION@
iconesdir=@datadir@/@PACKAGE_NAME@
plugins_LTLIBRARIES=vrmlimport.la vrmlexport.la
icones=vrml.png vrmlout.png

pkgdata_DATA=${icones}
EXTRA_DIST=${icones}

INCLUDES=-I../../src

AM_CXXFLAGS=@QT_CFLAGS@
AM_YFLAGS=-d
BUILT_SOURCES=parser.h
vrmlimport_la_SOURCES=vrmlimport.cc parser.y lexer.l
vrmlimport_la_LDFLAGS=-module -avoid-version

vrmlexport_la_SOURCES=vrmlexport.cc
vrmlexport_la_LDFLAGS=-module -avoid-version

mostlyclean-local:
	@-rm -f *~
maintainer-clean-local:
	@-rm -f Makefile.in config.h.in stamp-h.in
	@-rm parser.c parser.h lexer.c
		
