all: calc

calc: scanner.c parser.c symTable.o stack.o
	gcc -o calc scanner.c parser.c symTable.o stack.o

scanner.c: scanner.l
	flex --outfile=scanner.c scanner.l

parser.c: parser.y
	bison --yacc --defines -v --output=parser.c parser.y

symTable.o: symTable.c
	gcc -c symTable.c

stack.o: stack.c
	gcc -c stack.c

clean:
	rm *o
	rm parser.c parser.h scanner.c calc syntaxAnalysis parser.output
