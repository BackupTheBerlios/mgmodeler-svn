all: MGModeler
CXX=g++
CXXFLAGS=-I$(QTDIR)/include -Wall -I. -g -fPIC -DPIC -I$(QTDIR)/include/qt
LDFLAGS=-L/net/cremi/ddallaro/opt/src/Mesa-6.1/lib -lGL -lGLU -L$(QTDIR)/lib -lqt-mt -ldl
BIN=MGModeler


$(BIN): mainwindow.o mainwindow_moc.o main.o view3d.o openglwidget.o plugin.o trackball.o

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
