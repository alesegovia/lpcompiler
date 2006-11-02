/*
 *  LPC - The L+ Compiler. A compiler for a toy language similar to C.
 *  Copyright (C) 2011  Alejandro Segovia and Emilio Pombo
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Tokenizer para L+.*/
/* Las expresiones regulares que llevan \ son para 
 * escapar caracteres reservados
 */
%{
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "samp.tab.hpp"
#include "ast.h"

int line = 1;

%}

ID	[a-zA-Z_][a-zA-Z0-9]*
NUM	[0-9]+
REAL	[0-9]+\.[0-9]+
STRING	[^"]*

%%

true	{ yylval.bval = true; return(TRUE); }

false	{ yylval.bval = false; return(FALSE); }

bool	{ return(BOOL); }

int 	{ return(INT); }

float	{ return(FLOAT); }

string	{ return(STRING); }

void	{ return(VOID); }

if	{ return(IF); }

else	{ return(ELSE); }

while	{ return(WHILE); }

return	{ return(RETURN); }

!	{ return(NOT); }

==	{ return(EQUALS); }

!=	{ return(NOTEQUAL); }

\|\|	{ return(OR); }

&&	{ return(AND); }

\<	{ return(LESS); }

\<= 	{ return(LESSEQ); }

>	{ return(GREATER); }

>=	{ return(GREATEREQ); }

\*	{ return(TIMES); }

\/	{ return(DIVIDEBY); }

\+	{ return(PLUS); }

-	{ return(MINUS); }

=	{ return(ASSIGN); }

,	{ return(COLON); }

;	{ return(SEMICOLON); }

\(	{ return(LPAR); }

\)	{ return(RPAR); }

\{	{ return(LCURL); }

\}	{ return (RCURL); }

{NUM}	{ yylval.ival = atoi(yytext); return(NUM); }

{REAL}	{ yylval.fval = atof(yytext); return(REAL); }

{ID}	{ char* tmp = new char[strlen(yytext)+1]; strcpy(tmp, yytext); yylval.cpval = tmp; return(ID); }

\"{STRING}\" { char* tmp = new char[strlen(yytext)+1]; strcpy(tmp, yytext); yylval.cpval = tmp; return(STRING); }

\n	{ ++line;}

[ \t]+ /* Ignorar espacios, etc.*/

\/\/.*	/* Ignorar Comentarios de Linea*/

.	{ printf("Caracter no reconocido: %s\n",yytext); }

%%


int yywrap()
{
	return 1;
}

