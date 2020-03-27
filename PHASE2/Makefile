all: calc

calc: scanner.c parser.c symTable.o
	gcc -o calc scanner.c parser.c symTable.o

scanner.c: scanner.l
	flex --outfile=scanner.c scanner.l

parser.c: parser.y
	bison --yacc --defines -v --output=parser.c parser.y

symTable.o: symTable.c
	gcc -c symTable.c

clean:
	rm *o
	rm parser.c parser.h scanner.c calc grammarOutput parser.output
