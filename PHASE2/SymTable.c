#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>



enum type {Global, Local, Formal, Userfunc, Libfunc}; 


struct symbol_list{
	char *name ;
	unsigned int line; 
	enum type type ;
	unsigned int scope ;
	bool isActive;
	struct symbol_list *next;
};

struct symbol_list *Head = NULL;





/*

function to enter a sybol into symbol table 


*/
void add_sym(char *name, unsigned int line,enum type type, unsigned int scope, bool isActive){

	struct symbol_list *tmp, *new_sym;


	new_sym = (struct symbol_list*)malloc(100 * sizeof(struct symbol_list));
	new_sym->next = NULL;
	new_sym->name = name ;
	new_sym->line = line ;
	new_sym->type = type ;
	new_sym->scope = scope ;
	new_sym->isActive = isActive ;

	if (Head == NULL) Head = new_sym;
	else {
		new_sym->next = Head; 
		Head = new_sym;
	}
}

/**
A simple print function to print the symbols list
*/

void print(){
	struct symbol_list *tmp = Head;

	while (tmp != NULL){
		printf("Name : %s\tLine no: %d\t",tmp->name,tmp->line);
		if(tmp->type == Global) printf("type : Global\t" );
		else if(tmp->type == Global) printf("type : Global\t" );
		else if(tmp->type == Formal) printf("type : Formal\t" );
		else if(tmp->type == Userfunc) printf("type : Userfunc\t" );
		else if(tmp->type == Libfunc) printf("type : Libfunc\t" );
	
		printf("Scope :%d\tIs Active :%d\n",tmp->scope, tmp->isActive);

		tmp = tmp->next;

	}	
}

int main (){


	printf("Kalispera man\n");
	add_sym("LOUKAS",3,Global,5,false);
	add_sym("maria",2,Formal,511,false);
	add_sym("manos",1,Userfunc,11,true);
	print();
	return 0;


}