CXX=g++
CFLAGS=-g -fPIC
CXXFLAGS=-I$(QTDIR)/include -I ../.. -g -fPIC
LDFLAGS=-L$(QTDIR)/lib -shared
BIN=../bin/VRMLimport.so

all: $(BIN)

../bin/VRMLimport.so: vrml.o lex.yy.o vrml.tab.o
	$(CXX) $(LDFLAGS) $(LIBS) -o $@ $^
lex.yy.o: lex.yy.c
vrml.tab.o: vrml.tab.c vrml.tab.h

lex.yy.c: vrml.l vrml.tab.h
	flex -d $^
vrml.tab.c vrml.tab.h: vrml.y
	bison -d $^

vrml.o: vrml.cc ../../plugin.h

clean:
	@-rm -f *~ lex.yy.* vrml.tab.c vrml.tab.h *.o $(BIN)
