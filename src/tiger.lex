%x COMMENT
%x STR
%x STR1
LineTerminator (\n|\r|\n\r|\r\n)
WhiteSpace     (\n|\r|\r\n|\n\r|\t|\f| )
%{
#include <string.h>
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "errormsg.h"
#include "y.tab.h"

static int comLevel = 0; 

int charPos=1;

/* int stringStart = 0; */

#define STRINGMAX 1024
char stringBuilder[STRINGMAX+1];
int stringLen = 0;

int yywrap(void)
{
    /*
    if(comLevel) {
        EM_error(EM_tokPos,"unclosed comment");
    }
    */
    charPos=1;
    return 1;
}


void adjust(void)
{
 EM_tokPos=charPos;
 charPos+=yyleng;
}

static void init()
{
    stringLen = 0;
    stringBuilder[0] = '\0';
}

static void appendChar(char c) 
{
    if (stringLen<STRINGMAX) {
        stringBuilder[stringLen++]= c;
    }    
    else {   
        EM_error(EM_tokPos,"string too long.");
        stringLen = 0;
    }
}

static string getString(void)
{   
    stringBuilder[stringLen]=0;
    stringLen=0;
    return String(stringBuilder);
}

static void appendStr(char *str)
{
    if (stringLen + strlen(str) < STRINGMAX) {
        strcat(stringBuilder, str);
        stringLen += strlen(str);
    }
    else {
        EM_error(EM_tokPos, "string too long.");
        stringLen = 0;
    }
}

%}

%%
    /* White Spaces */
[ \t]            {adjust(); continue;}
(\n|\r\n|\n\r)        {adjust(); EM_newline(); continue;}

    /* Reserved Words */
"array"       {adjust(); return ARRAY;}
"break"       {adjust(); return BREAK;}
"do"          {adjust(); return DO;}
"else"        {adjust(); return ELSE;}
"end"         {adjust(); return END;}
"for"         {adjust(); return FOR;}
"function"    {adjust(); return FUNCTION;}
"if"          {adjust(); return IF;}
"in"          {adjust(); return IN;}
"let"         {adjust(); return LET;}
"nil"         {adjust(); return NIL;}
"of"          {adjust(); return OF;}
"then"        {adjust(); return THEN;}
"to"          {adjust(); return TO;}
"type"        {adjust(); return TYPE;}
"var"         {adjust(); return VAR;}
"while"       {adjust(); return WHILE;}

    /* Punctuation Symbols */
","           {adjust(); return COMMA;}
":"           {adjust(); return COLON;}
";"           {adjust(); return SEMICOLON;}
"("           {adjust(); return LPAREN;}
")"           {adjust(); return RPAREN;}
"["           {adjust(); return LBRACK;}
"]"           {adjust(); return RBRACK;}
"{"           {adjust(); return LBRACE;}
"}"           {adjust(); return RBRACE;}
"."           {adjust(); return DOT;}
"+"           {adjust(); return PLUS;}
"-"           {adjust(); return MINUS;}
"*"           {adjust(); return TIMES;}
"/"           {adjust(); return DIVIDE;}
"="           {adjust(); return EQ;}
"<>"          {adjust(); return NEQ;}
"<"           {adjust(); return LT;}
"<="          {adjust(); return LE;}
">"           {adjust(); return GT;}
">="          {adjust(); return GE;}
"&"           {adjust(); return AND;}
"|"           {adjust(); return OR;}
":="          {adjust(); return ASSIGN;}

    /* Identifiers */
[a-zA-Z][a-zA-Z0-9_]*   {adjust(); yylval.sval = String(yytext); return ID;}

    /* Integer Constants */
[0-9]+                  {adjust(); yylval.ival = atoi(yytext); return INT;}

    /* Comments */
"/*"            {adjust(); comLevel += 1; BEGIN(COMMENT);}
"*/"            {adjust(); EM_error(EM_tokPos, "Comment not open"); yyterminate();}
<COMMENT>{
    "/*"        {adjust(); comLevel += 1;}
    "*/"        {adjust(); if(--comLevel == 0) BEGIN(INITIAL);}
    (\n|\r\n)   {adjust(); EM_newline();}
    <<EOF>>     {EM_error(EM_tokPos, "Comment unclosed, encounter EOF."); yyterminate();}
    .           {adjust();}
}
    /* String Constants */
\"              {adjust(); BEGIN(STR); init(); /* stringStart = charPos - 1; */}
<STR>{
    \"          {adjust(); yylval.sval = getString(); /* EM_tokPos = stringStart; */ BEGIN(INITIAL); return STRING;}
    \\n         {adjust(); appendChar('\n');}
    \\t         {adjust(); appendChar('\t');}
    \\\\        {adjust(); appendChar('\\');}
    \\\"        {adjust(); appendChar('"');}
    \\[0-9][0-9][0-9]   {
        int x = yytext[1]*100 + yytext[2]*10 + yytext[3] - ('0' * 111);
	    adjust();
        if (x>255)
            EM_error(EM_tokPos, "illegal ascii escape");
	    else appendChar(x);      
    }
    \\          {adjust(); BEGIN(STR1);}
    \n          {adjust(); EM_error(EM_tokPos, "Illegal token.");}
    .           {adjust(); appendChar(yytext[0]);}
    <<EOF>>     {EM_error(EM_tokPos, "String unclosed, encounter EOF."); yyterminate();}
}

    /* Long String */
<STR1>{
    (\n|\r|\r\n|\n\r|\t|\f|" ")    {adjust(); continue;}
    \\              {adjust(); BEGIN(STR);}
    \"              {adjust(); EM_error(EM_tokPos, "\\ not match.");}
    .               {adjust(); appendStr(yytext);}
}

    /* default */
.	 {adjust(); EM_error(EM_tokPos,"illegal token");}

%%

