all: MGModeler
CXX=g++
CXXFLAGS=-I$(QTDIR)/include -Wall -I. -g -fPIC -DPIC -I$(QTDIR)/include/qt
LDFLAGS=-lGL -lGLU -L$(QTDIR)/lib -lqt-mt -ldl -lqassistantclient
BIN=MGModeler


$(BIN): view.o view2d.o view3d.o openglwidget.o mainwindow.o mainwindow_moc.o main.o plugin.o trackball.o gc.o

	$(CXX) $(LDFLAGS) $(LIBS) -o $@ $^

mainwindow_moc.cc: mainwindow.h
	$(QTDIR)/bin/moc $^ > $@
clean:
	rm -f *~ *.o $(BIN)
mainwindow_moc.o: mainwindow_moc.cc mainwindow.h
mainwindow.o : mainwindow.cc mainwindow.h view.h view2d.h view2d.h
plugin.o: plugin.cc plugin.h
view3d.o: view3d.cc view3d.h view.h openglwidget.h
view2d.o: view2d.cc view2d.h view.h openglwidget.h
openglwidget.o: openglwidget.cc openglwidget.h
trackball.o: trackball.cc trackball.h math/vector3.h math/matrix.h \
  math/matrix4.h math/quaternion.h math/vector3.h math/matrix.h
gc.o: gc.cc gc.h plugin.h
