all: MGModeler
CXX=g++
CXXFLAGS=-I$(QTDIR)/include -Wall -I. -g -fPIC -DPIC -I$(QTDIR)/include/qt
LDFLAGS=-lGL -lGLU -L$(QTDIR)/lib -lqt-mt -ldl
BIN=MGModeler


$(BIN): mainwindow.o mainwindow_moc.o main.o view3d.o openglwidget.o plugin.o trackball.o gc.o

	$(CXX) $(LDFLAGS) $(LIBS) -o $@ $^

mainwindow_moc.cc: mainwindow.h
	$(QTDIR)/bin/moc $^ > $@
clean:
	rm -f *~ *.o $(BIN)
mainwindow_moc.o: mainwindow_moc.cc mainwindow.h
mainwindow.o : mainwindow.cc mainwindow.h view3d.h
plugin.o: plugin.cc plugin.h
view3d.o: view3d.cc view3d.h openglwidget.h
openglwidget.o: openglwidget.cc openglwidget.h
trackball.o: trackball.cc trackball.h math/vector3.h math/matrix.h \
  math/matrix4.h math/quaternion.h math/vector3.h math/matrix.h
gc.o: gc.cc gc.h plugin.h /usr/qt/3/include/qevent.h \
  /usr/qt/3/include/qwindowdefs.h /usr/qt/3/include/qobjectdefs.h \
  /usr/qt/3/include/qglobal.h /usr/qt/3/include/qconfig.h \
  /usr/qt/3/include/qmodules.h /usr/qt/3/include/qfeatures.h \
  /usr/qt/3/include/qstring.h /usr/qt/3/include/qcstring.h \
  /usr/qt/3/include/qmemarray.h /usr/qt/3/include/qgarray.h \
  /usr/qt/3/include/qshared.h /usr/qt/3/include/qwinexport.h \
  /usr/qt/3/include/qnamespace.h /usr/qt/3/include/qregion.h \
  /usr/qt/3/include/qrect.h /usr/qt/3/include/qsize.h \
  /usr/qt/3/include/qpoint.h /usr/qt/3/include/qmime.h \
  /usr/qt/3/include/qmap.h /usr/qt/3/include/qdatastream.h \
  /usr/qt/3/include/qiodevice.h /usr/qt/3/include/qpair.h \
  /usr/qt/3/include/qvaluelist.h /usr/qt/3/include/qtl.h \
  /usr/qt/3/include/qtextstream.h math/vector3.h
