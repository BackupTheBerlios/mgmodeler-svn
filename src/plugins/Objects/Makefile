CXXFLAGS=-I$(QTDIR)/include -I ../..
LDFLAGS=-L$(QTDIR)/lib -shared
BIN=../bin/polylines.so

all: $(BIN)

$(BIN): polylines.o

	$(CXX) $(LDFLAGS) $(LIBS) -o $@ $^

clean:
	rm -f *~ *.o $(BIN)
polylines.o: polylines.cc ../../plugin.h
