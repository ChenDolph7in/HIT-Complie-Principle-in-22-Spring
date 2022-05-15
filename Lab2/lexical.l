%{
#include "stdio.h"
#include "stdlib.h"
#include "main.h"
#include "syntax.tab.h"
extern int error;
%}

%option yylineno
INT_DEX [1-9][0-9]{0,31}|[0]
INT_HEX [0][Xx](([1-9]|[A-F]|[a-f])([0-9]|[A-F]|[a-f])*|[0])
INT_OCT [0][0-7]*
FLOAT [0-9]*[.][0-9]+([eE][+-]?[0-9]+|[0])?f?
ID [a-zA-Z_][a-zA-Z_0-9]*
SEMI ;
COMMA ,
ASSIGNOP =
RELOP >|<|>=|<=|==|!=(^[=])
PLUS \+
MINUS -
STAR \*
DIV \/
AND &&
OR \|\|
DOT \.
NOT !
TYPE int|float
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
STRUCT struct
RETURN return
IF if
ELSE else 
WHILE while
VACUUM [ \t\r]*
DELIM \n|\t\n
COMMENT [/][/]
AERROR .
COMMENT2 "/*"([^\*]|(\*)*[^\*/])*(\*)*"*/"

%%
{TYPE} {yylval.n=insertNode("TYPE",0,yylineno);return TYPE;}
{STRUCT} {yylval.n=insertNode("STRUCT",0,yylineno);return STRUCT;}
{RETURN} {yylval.n=insertNode("RETURN",0,yylineno); return RETURN;}
{IF} { yylval.n=insertNode("IF",0,yylineno);return IF;}
{ELSE} {yylval.n=insertNode("ELSE",0,yylineno); return ELSE;}
{WHILE} {yylval.n=insertNode("WHILE",0,yylineno); return WHILE;}
{PLUS} {yylval.n=insertNode("PLUS",0,yylineno); return PLUS;}
{MINUS} {yylval.n=insertNode("MINUS",0,yylineno); return MINUS;}
{INT_DEX} {yylval.n=insertNode("INT_DEX",0,yylineno); return INT;}
{INT_HEX} {yylval.n=insertNode("INT_HEX",0,yylineno); return INT;}
{INT_OCT} {yylval.n=insertNode("INT_OCT",0,yylineno); return INT;}
{FLOAT} {yylval.n=insertNode("FLOAT",0,yylineno); return FLOAT;}
{ID} {yylval.n=insertNode("ID",0,yylineno); return ID;}
{VACUUM} {}
{DELIM} {}
{SEMI} {yylval.n=insertNode("SEMI",0,yylineno); return SEMI;}
{COMMA} {yylval.n=insertNode("COMMA",0,yylineno); return COMMA;}
{ASSIGNOP} {yylval.n=insertNode("ASSIGNOP",0,yylineno); return ASSIGNOP;}
{RELOP} {yylval.n=insertNode("RELOP",0,yylineno); return RELOP;}
{STAR} {yylval.n=insertNode("STAR",0,yylineno); return STAR;}
{DIV} {yylval.n=insertNode("DIV",0,yylineno); return DIV;}
{AND} {yylval.n=insertNode("AND",0,yylineno); return AND;}
{OR} {yylval.n=insertNode("OR",0,yylineno); return OR;}
{DOT} {yylval.n=insertNode("DOT",0,yylineno); return DOT;}
{NOT} {yylval.n=insertNode("NOT",0,yylineno); return NOT;}
{LP} {yylval.n=insertNode("LP",0,yylineno); return LP;}
{RP} {yylval.n=insertNode("RP",0,yylineno); return RP;}
{LB} {yylval.n=insertNode("LB",0,yylineno); return LB;}
{RB} {yylval.n=insertNode("RB",0,yylineno); return RB;}
{LC} {yylval.n=insertNode("LC",0,yylineno); return LC;}
{RC} {yylval.n=insertNode("RC",0,yylineno); return RC;}
{AERROR} { printf("Error type A at line %d: Mystirious charachter '%s'\n",yylineno,yytext);error++;}
{COMMENT} {
        char c = input();
        while (c != '\n'){c = input();}
}
{COMMENT2} {}

%%
int yywrap()
{
        return 1;
}