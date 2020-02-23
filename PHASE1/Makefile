all: scanner

scanner: scanner.c
	gcc -o scanner scanner.c

scanner.c: scanner.l
	flex --outfile=scanner.c scanner.l

clean:
	rm *c scanner
