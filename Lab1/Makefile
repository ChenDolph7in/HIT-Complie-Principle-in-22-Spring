result: syntax.y lexical.l main.h
	bison -d syntax.y 
	flex lexical.l 
	gcc lex.yy.c syntax.tab.c main.c -o scanner
