CXXFLAGS=-I$(QTDIR)/include -I ../.. -g
LDFLAGS=-L$(QTDIR)/lib -shared
BIN=../bin/polyline.so ../bin/nurbs.so

all: $(BIN)

../bin/polyline.so: polyline.o
	$(CXX) $(LDFLAGS) $(LIBS) -o $@ $^

../bin/nurbs.so: nurbs.o
	$(CXX) $(LDFLAGS) $(LIBS) -o $@ $^


clean:
	rm -f *~ *.o $(BIN)
polyline.o: polyline.cc ../../plugin.h
nurbs.o: nurbs.cc ../../plugin.h ../../math/vector3.h polyline.h
