all: avm

avm: avm.c exec_arith.o exec_assign.o exec_equality.o exec_funcs.o exec_table.o target
	gcc -o avm avm.c exec_arith.o exec_assign.o exec_equality.o exec_funcs.o exec_table.o

exec_arith.o: exec_arith.c
	gcc -c exec_arith.c

exec_assign.o: exec_assign.c
	gcc -c exec_assign.c

exec_equality.o: exec_equality.c
	gcc -c exec_equality.c

exec_funcs.o: exec_funcs.c
	gcc -c exec_funcs.c

exec_table.o: exec_table.c
	gcc -c exec_table.c

target:
	cd TARGET_CODE/ && $(MAKE)

clean:
	rm *o
	rm target.abc avm
	cd TARGET_CODE/ && $(MAKE) clean