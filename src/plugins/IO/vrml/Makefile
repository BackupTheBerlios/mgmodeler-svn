CXXFLAGS=-I$(QTDIR)/include -I../../../
LDFLAGS=-L$(QTDIR)/lib -shared
BIN=../../bin/iovrml.so

all: $(BIN)

$(BIN): vrml.o lex.yy.o vrml.tab.o
	$(CXX) $(LDFLAGS) $(LIBS) -o $@ $^

lex.yy.o: lex.yy.c
vrml.tab.o: vrml.tab.c vrml.tab.h

lex.yy.c: vrml.l vrml.tab.h
	flex $^
vrml.tab.c vrml.tab.h: vrml.y
	bison -d $^

clean:
	rm -f *~ *.o $(BIN)

vrml.o: vrml.cc ../../../plugin.h
