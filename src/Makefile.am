DEFS="-DDATADIR=\"${pkgdatadir}\"" \
  -DMODULES_DIR=\"@datadir@/@PACKAGE_NAME@-@VERSION@:@datadir@/@PACKAGE_NAME@\"
INCLUDES=-I../$(top_srcdir)/

EXTRA_DIST=subconfig.ui
SUFFIXES = .ui
%.h: %.ui
	@UIC@ $< -o $@

%.cc: %.ui
	@UIC@ -impl $*.h $< -o $@

%_moc.cc: %.h
	@MOC@ $< -o $@

pkgincludemathdir=@includedir@/@PACKAGE_NAME@
pkgincludemath_HEADERS=math/vector3.h math/quaternion.h math/matrix4.h \
        math/matrix.h

BUILT_SOURCES=mainwindow_moc.cc icons.cc subconfig_moc.cc subconfig.cc \
	subconfig.h subconfig.ui.h
bin_PROGRAMS=MGModeler
MGModeler_SOURCES=gc.cc mainwindow.cc openglwidget.cc plugin.h view2d.cc \
		  view3d.h gc.h mainwindow.h openglwidget.h trackball.cc \
		  view2d.h view.cc main.cc plugin.cc trackball.h view3d.cc \
		  view.h subconfig.cc subconfig_moc.cc

nodist_MGModeler_SOURCES=mainwindow_moc.cc icons.cc

mainwindow_moc.cc: mainwindow.h
images_noprefix=exit.png fill.png help.png normals.png wireframe.png \
        filenew.png fileopen.png filesave.png flieclose.png handbook.png\
	light.png move.png tile.png \
	snormals.png configure.png select.png compute.png
images=$(addprefix ../icons/, $(images_noprefix))

icons.cc: ${images}
	uic -o $@ -embed $@ $^

MGModeler_CXXFLAGS=@QT_CFLAGS@ @GL_CFLAGS@
MGModeler_LDFLAGS=
MGModeler_LDADD=@QT_LIBS@ @GL_LIBS@

mostlyclean-local:
	@-rm -f *~ *_moc.cc subconfig.h subconfig.cc icons.h
maintainer-clean-local:
	@-rm -f Makefile.in config.h.in stamp-h.in
