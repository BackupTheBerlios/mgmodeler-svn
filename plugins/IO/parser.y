%{
#include <stdlib.h>
#include <stdio.h>

void yyerror(char *message);
extern int linecount;
int coordIndexes[120000];
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

%type <real> value

%token WORD POINT COORDINDEX EBRACE OBRACE


%%

expression: | pointset expression 
	    | coordset expression 
	    | REAL expression 
	    | INTEGER expression
	    | OBRACE expression
	    | EBRACE expression
	    | WORD expression
{
}
;

pointset: POINT OBRACE points EBRACE
{
}
;

points: point | point points
;

point: value value value
{
	addPoint ($1, $2, $3);
}
;

value: REAL | INTEGER
{
	$$ = $1;
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
int yydebug;
/* This is an error function used by yacc, and must be defined */
void yyerror(char *message)
{
	fprintf(stderr, "%d: %s\n\n\n", linecount, message);
}
