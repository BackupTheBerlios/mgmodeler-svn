%{
/* Again, this is C code that is inserted into the beginning of the output */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "vrml.tab.h"  /* Include the token definitions generated by yacc */

int linecount = 0;
%}

/* Prevent the need for linking with -lfl */
%option noyywrap

/* This next section is a set of regular expressions that describe input
   tokens that are passed back to yacc.  The tokens are defined in y.tab.h,
   which is generated by yacc.


   IndexFaceSet            { return INDEXFACESET; }

 */
%%
[\#].*			/* ignore comments */
(-?[0-9]+)?\.[0-9]+(e-?[0-9]+)? { sscanf (yytext, "%f", &yylval.real); return REAL; }
-?[0-9]+\.(e-?[0-9]+)?		{ sscanf (yytext, "%f", &yylval.real); return REAL; }

-?[0-9]+               { sscanf (yytext, "%d", &yylval.integer); 
			  return INTEGER; }
point                   { return POINT;}
coordIndex		{ return COORDINDEX; }
\[			{ return OBRACE; }
\]			{ return EBRACE; }
[\n]			linecount++;
[ \t,]			/* */
[a-zA-Z{}]+		/* */
[^ \t[]]			/* */
%%