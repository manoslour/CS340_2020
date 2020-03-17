#include "SymTable.h"


bool add_scope_sym(char *name, unsigned int line,enum type type, unsigned int scope, bool isActive){

	struct scope_list *tmp = scope_head, *new_scope;
	struct symbol_list *new_sym, *parse;
	bool done = false ; //true if the insertion is success false if the given scope list doesnt exist

	new_sym = (struct symbol_list*)malloc(100 * sizeof(struct symbol_list));
	new_sym->next = NULL;
	new_sym->name = name ;
	new_sym->line = line ;
	new_sym->type = type ;
	new_sym->scope = scope ;
	new_sym->isActive = isActive ;

	if (scope_head == NULL){	//AN EINAI TO PRWTO EVER SYMBOL
		new_scope = (struct scope_list*)malloc(100 * sizeof(struct scope_list));
		new_scope->next = new_scope->prev = NULL;
		scope_head = new_scope;
		new_scope->scope = scope;
		new_scope->symbols = new_sym;
	}
	else {
		tmp = scope_head;
		while (tmp != NULL){
			if (tmp->scope == scope){
				
				if (tmp->symbols == NULL) tmp->symbols = new_sym;
				else {
					parse = tmp->symbols;
					while (parse->next != NULL ) parse = parse->next;
					parse->next = new_sym;
				}
				
				done == true;
				return 1;
			}

			tmp = tmp->next;
		}
		if (done == false ){
			printf("MPENW EDW GIA TO SCOPE %d\n",scope );
			tmp = scope_head; 
			new_scope = (struct scope_list*)malloc(100 * sizeof(struct scope_list));
		
			while (tmp->next != NULL) tmp = tmp->next;
			
			tmp->next = new_scope;
			new_scope->prev = tmp;
			new_scope->scope = scope; 

			tmp = scope_head;
			while (tmp != NULL){
				if (tmp->scope == scope){
					if (tmp->symbols == NULL) tmp->symbols = new_sym;
					else {
						parse = tmp->symbols;
						while (parse->next != NULL ) parse = parse->next;
						parse->next = new_sym;
					}

					done == true;
					return 1 ;
				}
				tmp = tmp->next;
			}
		}
	}

	return 0 ;

}

/**
A simple print function to print the symbols list
*/

void print(){
	struct scope_list *temp = scope_head;
	struct symbol_list *tmp;


	while (temp != NULL){
		tmp = temp->symbols;
		printf("---------------------------------- Scope %d ----------------------------------\n",temp->scope );
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
		temp = temp->next;
	}
}

int main (){


	printf("Kalispera man\n");
	add_scope_sym("LOUKAS",1,Global,0,false);
	add_scope_sym("maria",2,Formal,9,false);
	add_scope_sym("manos",3,Userfunc,0,true);
	add_scope_sym("manos",3,Userfunc,1,true);
	print();
	int i=0;
	

	for(i=0; i<Buckets; i++){
		printf("HASH ME %d :%p\n",i,HashTable[i] );
	}


	return 0;


}