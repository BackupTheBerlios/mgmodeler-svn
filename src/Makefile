all: MGModeler

CXXFLAGS=-I$(QTDIR)/include -Wall
LDFLAGS=-L$(QTDIR)/lib -lqt-mt -lGL -lGLU -rdynamic -ldl
BIN=MGModeler


$(BIN): mainwindow.o mainwindow_moc.o main.o view3d.o openglwidget.o plugin.o

	$(CXX) $(LDFLAGS) $(LIBS) -o $@ $^

mainwindow_moc.cc: mainwindow.h
	moc $^ > $@
clean:
	rm -f *~ *.o $(BIN)
mainwindow_moc.o: mainwindow_moc.cc mainwindow.h
mainwindow.o : mainwindow.cc mainwindow.h view3d.h
plugin.o: plugin.cc plugin.h
view3d.o: view3d.cc view3d.h openglwidget.h
openglwidget.o: openglwidget.cc openglwidget.h
