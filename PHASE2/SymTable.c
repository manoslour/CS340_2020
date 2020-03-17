#include "SymTable.h"



/*
bool add_scope_sym(char *name, unsigned int line,enum type type, unsigned int scope, bool isActive){

	struct scope_list *tmp = scope_head, *new_scope;
	struct symbol_list *new_sym, *parse;
	bool done = false ; //true if the insertion is success false if the given scope list doesnt exist

	new_sym = (struct symbol_list*)malloc(sizeof(struct symbol_list));
	new_sym->next = NULL;
	new_sym->name = name ;
	new_sym->line = line ;
	new_sym->type = type ;
	new_sym->scope = scope ;
	new_sym->isActive = isActive ;

	if (scope_head == NULL){	//AN EINAI TO PRWTO EVER SYMBOL
		new_scope = (struct scope_list*)malloc(sizeof(struct scope_list));
		new_scope->next = new_scope->prev = NULL;
		scope_head = new_scope;
		new_scope->scope = scope;
		new_scope->symbols = new_sym;
	}
	else {
		tmp = scope_head;
		while (tmp != NULL){
			if (tmp->scope == scope){ // an uparxei idi to scope tote vazw k allo sto telos 
				
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
			
			tmp = scope_head; 
			new_scope = (struct scope_list*)malloc(sizeof(struct scope_list));
		
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

}*/

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


/*
newsym = malloc(sizeof(struct SymTable));
    newsym->key = (char*)malloc(strlen(pcKey)+1);
    strcpy((char*)newsym->key,pcKey);
    newsym->value = (void*)pvValue;
  */

/*insert the scope list akrivws opws to sxhma sto 3o front 15o slide*/

bool ScopeListInsert(struct symbol_list *sym_node){

	struct scope_list *tmp = scope_head , *new_scope, *prev = NULL;
	struct symbol_list *parse;
	

	if(tmp == NULL){ // an einai to prwto
		new_scope = (struct scope_list*)malloc(sizeof(struct scope_list));
		new_scope->scope = sym_node->scope;
		new_scope->next = new_scope->prev = NULL;
		new_scope->symbols = sym_node;
		scope_head = new_scope; 
		return 1;
	}
	else { // an uparxei idi to skope valto sto telos 
		while(tmp != NULL){
			
			if (tmp->scope == sym_node->scope){
				parse = tmp->symbols;
				while (parse->scope_next != NULL) parse = parse->scope_next;
				parse->scope_next = sym_node;
				return 1;
			}

			tmp = tmp->next;
		}
		// an den uparxei to scope kane new 
		new_scope = (struct scope_list*)malloc(sizeof(struct scope_list));
		new_scope->scope = sym_node->scope;
		new_scope->next = new_scope-> prev = NULL;
		new_scope->symbols = sym_node;
		tmp = scope_head;
		// edw kanw ta dika m gia insertion sort xD lmao :P 
		while(new_scope->scope > tmp->scope) {
			if (scope_head->next == NULL) {
				scope_head->next = new_scope;
				new_scope->prev = scope_head;
				break;
			}
			else {
				prev = tmp;
				tmp = tmp->next;	
			}
		}
		
		if(prev == NULL){
			tmp = scope_head;
			if (new_scope->scope < tmp->scope ){
				new_scope->next = tmp;
				tmp->prev = new_scope;
				scope_head = new_scope;
			}
			else {
				tmp->next = new_scope;
				new_scope->prev = tmp;
			}
		}
		else {
			prev->next = new_scope; 
			new_scope->prev = prev;
			new_scope->next = tmp; 
			tmp->prev = new_scope ;
		}
		return 1;
	}
	return 0;
}




/*
returns true if the insertion is successfull or false if its unsuccessfull

calculate the posisition in var pos (from a simple hash function) , check if this position its takken.
If its takken then add the symbol in the end of the list , if it isnt the the HashTable position has this symbol as its first

*/
bool HashInsert(char *name, unsigned int line,enum type type, unsigned int scope, bool isActive){

	int pos = (int)*name % Buckets;
	
	struct scope_list *tmp = scope_head, *new_scope;
	struct symbol_list *new_sym, *parse;
	bool done = false ; //true if the insertion is success false if the given scope list doesnt exist


	//create the node
	new_sym = (struct symbol_list*)malloc(sizeof(struct symbol_list));
	new_sym->next = NULL;
	new_sym->scope_next = NULL;
	new_sym->name = name ;
	new_sym->line = line ;
	new_sym->type = type ;
	new_sym->scope = scope ;
	new_sym->isActive = isActive ;
	//printf("\n\nKANW INSERT me SCOPE %d\n\n", new_sym->scope);
	ScopeListInsert(new_sym);

	//if its the first
	if (HashTable[pos] == NULL){
		HashTable[pos] = new_sym;
		return 1; 
	}
	else {
		parse = HashTable[pos];
		while (parse->next != NULL) parse = parse->next;

		parse->next = new_sym;
		return 1;
	}
	return 0;

}

void HashPrint(){
	int i; 
	struct symbol_list *tmp;
	for (i=0; i<Buckets; i++){
		
		tmp = HashTable[i];
		if (HashTable[i] == NULL) continue;
		
		printf("HASH NUM %d :",i );
		while (tmp != NULL){

			printf("===Name : %s\tLine no: %d\t",tmp->name,tmp->line);
			if(tmp->type == Global) printf("type : Global\t" );
			else if(tmp->type == Global) printf("type : Global\t" );
			else if(tmp->type == Formal) printf("type : Formal\t" );
			else if(tmp->type == Userfunc) printf("type : Userfunc\t" );
			else if(tmp->type == Libfunc) printf("type : Libfunc\t" );
			printf("Scope :%d\tIs Active :%d\n",tmp->scope, tmp->isActive);

			tmp = tmp->next;
		}
	}


}


  

void ScopePrint(){
	struct scope_list *tmp = scope_head;
	struct symbol_list *temp;
	printf("\n\n ------BEGIN------\n\n");
	while (tmp != NULL){
		temp = tmp->symbols; 
		printf("SCOPE %d\n",tmp->scope  );
		while (temp != NULL){
			printf("Name : %s , Scope %d\n", temp->name, temp->scope );
			temp = temp->scope_next;
		}
		tmp = tmp->next;
	}


}


int main (){


	printf("Kalispera man\n");
	HashInsert("LOUKAS",1,Global,0,false);
	HashInsert("maria",2,Formal,9,false);
	HashInsert("vhvhjjhvb",3,Userfunc,0,true);
	HashInsert("manos",3,Userfunc,1,true);
	HashInsert("sakis",3,Userfunc,3,true);
	HashInsert("gus",3,Userfunc,5,true);
	HashInsert("liokis",3,Userfunc,-1,true); // -1 den emfanizei giati to scope to exw kanei unsigned 

	//print();
	//HashPrint();

	ScopePrint();

	int i=0;
	


	return 0;


}