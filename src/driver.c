#include <stdio.h>
#include "util.h"
#include "errormsg.h"
#include "tokens.h"

YYSTYPE yylval;

int yylex(void); /* prototype for the lexing function */



string toknames[] = {
"ID", "STRING", "INT", "COMMA", "COLON", "SEMICOLON", "LPAREN",
"RPAREN", "LBRACK", "RBRACK", "LBRACE", "RBRACE", "DOT", "PLUS",
"MINUS", "TIMES", "DIVIDE", "EQ", "NEQ", "LT", "LE", "GT", "GE",
"AND", "OR", "ASSIGN", "ARRAY", "IF", "THEN", "ELSE", "WHILE", "FOR",
"TO", "DO", "LET", "IN", "END", "OF", "BREAK", "NIL", "FUNCTION",
"VAR", "TYPE"
};


string tokname(tok) {
  return tok<258 || tok>300 ? "BAD_TOKEN" : toknames[tok-258];
}

int main(int argc, char **argv) {
 string fname; int tok;
 if (argc!=2) {fprintf(stderr,"usage: a.out filename\n"); exit(1);}
 fname=argv[1];
 EM_reset(fname);
 for(;;) {
   tok=yylex();
   if (tok==0) break;
   /* printf("token = %d\n", tok); */
   switch(tok) {
   case ID: case STRING:
     /* printf("found id or string.\n"); */
     printf("%10s %4d %s\n",tokname(tok),EM_tokPos,yylval.sval);
     break;
   case INT:
     /* printf("found int.\n"); */
     printf("%10s %4d %d\n",tokname(tok),EM_tokPos,yylval.ival);
     break;
   default:
     printf("%10s %4d\n",tokname(tok),EM_tokPos);
   }
 }
 return 0;
}


