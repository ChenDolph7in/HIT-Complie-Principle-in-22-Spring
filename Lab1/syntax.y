%{
#include <unistd.h>
#include <stdio.h>   
#include "main.h"
extern int yylex();
int error = 0;
%}

/*declare types*/
%union TYPE{
struct node* n;
double d;
}

/*declare tokens*/
%token  <n> INT FLOAT
%token <n> TYPE STRUCT RETURN IF ELSE WHILE ID SPACE SEMI COMMA ASSIGNOP RELOP PLUS
MINUS STAR DIV AND OR DOT NOT LP RP LB RB LC RC AERROR
%token <n> EOL

/*declare priority*/
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT 
%left LP RP LB RB DOT
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

/*declare non-terminals*/
%type  <n> Program ExtDefList ExtDef1 ExtDef2 ExtDecList Specifier StructSpecifier 
OptTag  Tag VarDec  FunDec VarList ParamDec Compst StmtList Stmt DefList Def DecList Dec Exp Args

%%
/*High-level Definitions*/
Program:ExtDefList {$$=insertNode("Program",1,$1);if(error==0) preorderTraversal($$,0);}
    ;
ExtDefList:ExtDef1 ExtDefList {$$=insertNode("ExtDefList",2,$1,$2);}
	| ExtDef2 ExtDefList {$$=insertNode("ExtDefList",2,$1,$2);}
	| {$$=insertNode("ExtDefList",0,-1);}
	;
ExtDef1:Specifier ExtDecList SEMI    {$$=insertNode("ExtDef",3,$1,$2,$3);}    
	|Specifier SEMI	{$$=insertNode("ExtDef",2,$1,$2);}
	|error SEMI {error++;}
	;
ExtDef2:Specifier FunDec Compst	{$$=insertNode("ExtDef",3,$1,$2,$3);}
	|error RC {error++;}
	;
ExtDecList:VarDec {$$=insertNode("ExtDecList",1,$1);}
	|VarDec COMMA ExtDecList {$$=insertNode("ExtDecList",3,$1,$2,$3);}
	;
/*Specifiers*/
Specifier:TYPE {$$=insertNode("Specifier",1,$1);}
	|StructSpecifier {$$=insertNode("Specifier",1,$1);}
	;
StructSpecifier:STRUCT OptTag LC DefList RC {$$=insertNode("StructSpecifier",5,$1,$2,$3,$4,$5);}
	|STRUCT Tag {$$=insertNode("StructSpecifier",2,$1,$2);}
	;
OptTag:ID {$$=insertNode("OptTag",1,$1);}
	|{$$=insertNode("OptTag",0,-1);}
	;
Tag:ID {$$=insertNode("Tag",1,$1);}
	;
/*Declarators*/
VarDec:ID {$$=insertNode("VarDec",1,$1);}
	| VarDec LB INT RB {$$=insertNode("VarDec",4,$1,$2,$3,$4);}
	;
FunDec:ID LP VarList RP {$$=insertNode("FunDec",4,$1,$2,$3,$4);}
	|ID LP RP {$$=insertNode("FunDec",3,$1,$2,$3);}
	;
VarList:ParamDec COMMA VarList {$$=insertNode("VarList",3,$1,$2,$3);}
	|ParamDec {$$=insertNode("VarList",1,$1);}
	;
ParamDec:Specifier VarDec {$$=insertNode("ParamDec",2,$1,$2);}
    ;

/*Statements*/
Compst:LC DefList StmtList RC {$$=insertNode("Compst",4,$1,$2,$3,$4);}
	;
StmtList:Stmt StmtList{$$=insertNode("StmtList",2,$1,$2);}
	| {$$=insertNode("StmtList",0,-1);}
	;
Stmt:Exp SEMI {$$=insertNode("Stmt",2,$1,$2);}
	|Compst {$$=insertNode("Stmt",1,$1);}
	|RETURN Exp SEMI {$$=insertNode("Stmt",3,$1,$2,$3);}
	|IF LP Exp RP Stmt %prec LOWER_THAN_ELSE{$$=insertNode("Stmt",5,$1,$2,$3,$4,$5);}
	|IF LP Exp RP Stmt ELSE Stmt {$$=insertNode("Stmt",7,$1,$2,$3,$4,$5,$6,$7);}
	|WHILE LP Exp RP Stmt {$$=insertNode("Stmt",5,$1,$2,$3,$4,$5);}
	|error SEMI {error++;}
	;
/*Local Definitions*/
DefList:Def DefList{$$=insertNode("DefList",2,$1,$2);}
	| {$$=insertNode("DefList",0,-1);}
	;
Def:Specifier DecList SEMI {$$=insertNode("Def",3,$1,$2,$3);}
	;
DecList:Dec {$$=insertNode("DecList",1,$1);}
	|Dec COMMA DecList {$$=insertNode("DecList",3,$1,$2,$3);}
	;
Dec:VarDec {$$=insertNode("Dec",1,$1);}
	|VarDec ASSIGNOP Exp {$$=insertNode("Dec",3,$1,$2,$3);}
	;
/*Expressions*/
Exp:Exp ASSIGNOP Exp{$$=insertNode("Exp",3,$1,$2,$3);}
        |Exp AND Exp{$$=insertNode("Exp",3,$1,$2,$3);}
        |Exp OR Exp{$$=insertNode("Exp",3,$1,$2,$3);}
        |Exp RELOP Exp{$$=insertNode("Exp",3,$1,$2,$3);}
        |Exp PLUS Exp{$$=insertNode("Exp",3,$1,$2,$3);}
        |Exp MINUS Exp{$$=insertNode("Exp",3,$1,$2,$3);}
        |Exp STAR Exp{$$=insertNode("Exp",3,$1,$2,$3);}
        |Exp DIV Exp{$$=insertNode("Exp",3,$1,$2,$3);}
        |LP Exp RP{$$=insertNode("Exp",3,$1,$2,$3);}
        |MINUS Exp {$$=insertNode("Exp",2,$1,$2);}
        |NOT Exp {$$=insertNode("Exp",2,$1,$2);}
        |ID LP Args RP {$$=insertNode("Exp",4,$1,$2,$3,$4);}
        |ID LP RP {$$=insertNode("Exp",3,$1,$2,$3);}
        |Exp LB Exp RB {$$=insertNode("Exp",4,$1,$2,$3,$4);}
        |Exp DOT ID {$$=insertNode("Exp",3,$1,$2,$3);}
        |ID {$$=insertNode("Exp",1,$1);}
        |INT {$$=insertNode("Exp",1,$1);}
        |FLOAT{$$=insertNode("Exp",1,$1);}	
        ;
Args:Exp COMMA Args {$$=insertNode("Args",3,$1,$2,$3);}
        |Exp {$$=insertNode("Args",1,$1);}
        ;
%%