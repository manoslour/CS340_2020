all: a.out

lex.yy.c: al2.l
	flex al2.l
a.out: lex.yy.c 
	gcc lex.yy.c -I.
