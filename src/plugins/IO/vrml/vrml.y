%{
#include <stdlib.h>
#include <stdio.h>

void yyerror(char *message);
extern int linecount;
int coordIndexes[256];
int cilength = 0;

int yylex(void);

extern void addPoint (double x, double y, double z);
extern void addFace (int *vec, int length);
%}

%union {
  int integer;
  float real;
}

%token <real> REAL
%token <integer> INTEGER
%token  INDEXFACESET WORD POINT COORDINDEX EBRACE OBRACE


%%

expression: | pointset expression 
	    | coordset expression 
	    | REAL expression 
	    | INTEGER expression
	    | OBRACE expression
	    | EBRACE expression
	    | WORD expression
{
	fprintf(stderr, "parsing an expression\n");
}
;

pointset: POINT OBRACE points EBRACE
{
	fprintf(stderr, "found a pointset\n");
}
;

points: point | point points
;

point: REAL REAL REAL
{
	addPoint ($1, $2, $3);
	fprintf(stderr, "found a point at (%f, %f, %f)\n", $1, $2, $3);
}
;

coordset: COORDINDEX OBRACE coords EBRACE
{
	addFace (coordIndexes, cilength);
	cilength = 0;
	fprintf(stderr, "found a coordset\n");
}

coords: coord | coord coords
;

coord: INTEGER
{
	coordIndexes[cilength++]=$1;
	fprintf(stderr, "found a coord %d\n", $1);
}

%%
extern int yydebug;
int main(int argc, char *argv[])
{
	yydebug=argc-1;
	yyparse();
	return 0;
}

/* This is an error function used by yacc, and must be defined */
void yyerror(char *message)
{
	fprintf(stderr, "%d: %s\n\n\n", linecount, message);
}
