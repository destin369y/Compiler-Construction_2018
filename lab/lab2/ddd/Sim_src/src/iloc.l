WHITESPACE [ \t]
NUM [0-9]+

%{
  /* iloc.l
   * Lex specification for the ILOC subset defined in
   * "Engineering a Compiler" by Cooper and Torczon
   * written by Todd Waterman
   * 11/30/00 */

  #include <stdlib.h>
  #include <stdio.h>
  #include <string.h>

  #include "instruction.h"
  #include "iloc.tab.h"

  int line_counter = 1;
  Opcode* current_opcode;

  char token[256];

%}

%%

\[            {return OPEN_BRACKET;}
\]            {return CLOSE_BRACKET;}
\;            {return SEMICOLON;}
\,            {return COMMA;}
\=\>          {return ARROW;}
\-\>          {return ARROW;}

r{NUM}        {yylval.ival = atoi(&yytext[1]); return REGISTER;}
{NUM}         {(void) strcpy(token,yytext);
                yylval.ival = atoi(yytext); 
                return NUMBER;
              }
\-{NUM}       {(void) strcpy(token,yytext);
                yylval.ival = atoi(yytext); 
                return NUMBER;
              }

dis         {return DATA_INT;}
dcs         {return DATA_CHAR;}

[a-zA-Z0-9\_]+: {(void) strcpy(token,yytext); return TARGET;}

[a-zA-Z0-9\_]+  {
                  (void) strcpy(token,yytext);
		  current_opcode = get_opcode(token);
                  if (current_opcode) { 
                     return OPCODE;
                  }
                  else {
                     return LABEL;
                  }
                }


\/\/[^\n]*  { /* Comment */}

[\n]          {line_counter++;}

{WHITESPACE}  {;}

%%

int yywrap()
{
  return 1;
}

