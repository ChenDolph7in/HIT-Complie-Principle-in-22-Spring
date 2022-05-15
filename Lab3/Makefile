result: syntax.y lexical.l main.h analyse.h interCode.h
	bison -d syntax.y 
	flex lexical.l 
	gcc lex.yy.c syntax.tab.c main.c analyse.c interCode.c -o scanner
