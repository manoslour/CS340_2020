#include "SymTable.h"



void initialize(){

	
	HashInsert("print", 0, Libfunc, 0,1);
	HashInsert("input", 0, Libfunc, 0,1);
	HashInsert("objectmemberkeys", 0, Libfunc, 0,1);
	HashInsert("objecttotalmembers", 0, Libfunc, 0,1);
	HashInsert("objectcopy", 0, Libfunc, 0,1);
	HashInsert("tootalarguments", 0, Libfunc, 0,1);
	HashInsert("argument", 0, Libfunc, 0,1);
	HashInsert("typeof", 0, Libfunc, 0,1);
	HashInsert("strtonum", 0, Libfunc, 0,1);
	HashInsert("sqrt", 0, Libfunc, 0,1);
	HashInsert("cos", 0, Libfunc, 0,1);
	HashInsert("sin", 0, Libfunc, 0,1);


}


/*
returns true if the symbol exists in scope 0 or the given scope false elsewhere
*/

bool LookUp(char* symbol, int scope){
	
	struct scope_list *tmp = scope_head;
	struct symbol_list *temp;

	while (tmp != NULL) {
		if (tmp->scope == 0 || tmp->scope == scope ){
			temp = tmp->symbols;
			while (temp != NULL) {
				if (!strcmp(temp->name , symbol)) return 1;
				temp = temp->scope_next;
			}
		}
		tmp = tmp->next;
	}

	return 0;

}

/*
sets isActive = true to all the symbols of a given scope 
*/
bool Enable (int scope ){

	struct scope_list *tmp = scope_head;
	struct symbol_list *temp;

	while (tmp != NULL) {

		if(tmp->scope == scope ){
			temp = tmp->symbols; 
			while(temp != NULL){
				temp->isActive = 1;
				temp = temp->scope_next;
			}
			return 1;
		}

		tmp = tmp->next;
	}
	return 0;
}



/*
sets isActive = false to all the symbols of a given scope 
*/

bool Hide (int scope ){

	struct scope_list *tmp = scope_head;
	struct symbol_list *temp;

	while (tmp != NULL) {

		if(tmp->scope == scope ){
			temp = tmp->symbols; 
			while(temp != NULL){
				temp->isActive = 0;
				temp = temp->scope_next;
			}
			return 1;
		}

		tmp = tmp->next;
	}
	return 0;
}


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
		tmp = scope_head;
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
		while(new_scope->scope > tmp->scope ) {
			if (scope_head->next == NULL) {
				scope_head->next = new_scope;
				new_scope->prev = scope_head;
				break;
			}
			else {
				prev = tmp;
				tmp = tmp->next;	
			}
			if (tmp->next == NULL) break;
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
bool HashInsert(char *name, unsigned int line, enum type type, unsigned int scope, bool isActive){

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


/*
prints all the symbols that exists in HashTable 
*/

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


/*
prints the scope list (first all the symbols of the min scope to the highest scope )
*/  

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


/**
A simple print function to print the symbols list 

	======================== THIS IS tHE MAIN PRINT ======================== 
	gege?
	lmaOs and xDs 
*/

void print(){

	struct scope_list *tmp = scope_head;
	struct symbol_list *temp;

	while (tmp != NULL){
		printf("\n-------------------------------- "YEL"SCOPE %d"RESET" --------------------------------\n",tmp->scope );
		
		temp = tmp->symbols;
		while(temp != NULL){

			printf(" \"%s\"  \t",temp->name);
			if(temp->type == Local) printf("[Local Variable]\t" );
			else if(temp->type == Global) printf("[Global Variable]\t" );
			else if(temp->type == Formal) printf("[Formal Argument]\t" );
			else if(temp->type == Userfunc) printf("[User funcion]\t" );
			else if(temp->type == Libfunc) printf("[Library function]\t" );
			printf("(line %d)\t(scope %d)\t (is active %d)\n",temp->line, temp->scope, temp->isActive);


			temp = temp->scope_next;
		} 
		tmp = tmp->next;
	}
	printf("\n");
}

int main (){


	initialize();

	HashInsert("vhvhjjhvb",3,Userfunc,0,true);
	HashInsert("LOUKAS",1,Libfunc,1,false);
	HashInsert("maria",2,Formal,9,false);
	HashInsert("manos",3,Userfunc,1,true);
	HashInsert("sakis",3,Userfunc,3,true);
	HashInsert("gus",3,Userfunc,5,true);
	printf("KALISPERA\n");
	HashInsert("liokis",3,Userfunc,1,true); // -1 den emfanizei giati to scope to exw kanei unsigned 
	print();
	


	return 0;


}