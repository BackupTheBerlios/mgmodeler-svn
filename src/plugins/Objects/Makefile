CXXFLAGS=-I$(QTDIR)/include -I ../.. -g
LDFLAGS=-L$(QTDIR)/lib -shared
BIN=../bin/polyline.so 

all: $(BIN)

$(BIN): polyline.o

	$(CXX) $(LDFLAGS) $(LIBS) -o $@ $^

clean:
	rm -f *~ *.o $(BIN)
polylines.o: polyline.cc ../../plugin.h
