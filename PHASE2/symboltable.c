#include "SymTable.h"





void ActivateScope(unsigned int scope){

	struct ScopeListEntry *temp = scope_head;
	struct SymbolTableEntry *tmp;
	
	while (temp != NULL){
		if (temp->scope == scope ){
			tmp = temp->symbols; 
			while (tmp != NULL) {
				tmp->isActive = 1;
				tmp = tmp->scope_next;
			}
		}
		temp = temp->next;
	}

}

void HideScope(unsigned int scope){
	
	struct ScopeListEntry *temp = scope_head;
	struct SymbolTableEntry *tmp;
	
	while (temp != NULL){
		if (temp->scope == scope ){
			tmp = temp->symbols; 
			while (tmp != NULL) {
				tmp->isActive = 0;
				tmp = tmp->scope_next;
			}
		}
		temp = temp->next;
	}

}


/*
Return 1 if a symbol exists in a given scope 0 elsewere
*/
bool ScopeLookUp(char *name, unsigned int scope){
	

	struct ScopeListEntry *temp = scope_head;
	struct SymbolTableEntry *tmp;


	while (temp != NULL){

		if(scope == temp->scope){
			tmp = temp->symbols;
			while (tmp != NULL) {
				
				if (tmp->type == LIBFUNC || tmp->type == USERFUNC)
					if (!strcmp(tmp->value.funcVal->name, name)) return 1; // symbol find in a current scope return true
				else 
					if (!strcmp(tmp->value.varVal->name, name)) return 1; // symbol find in a current scope return true
				
				tmp = tmp->scope_next;
			}
		} 
		temp = temp->next;
	} 
	return 0;
}


/*
return 1 if a symbol exists in all the scopes and symbols that we have 0 elsewere
*/
bool GeneralLookUp(char *name){
	
	struct ScopeListEntry *temp = scope_head;
	struct SymbolTableEntry *tmp;

	while (temp != NULL){
		
		tmp = temp->symbols;
		while (tmp != NULL){
			
			if (tmp->type == LIBFUNC || tmp->type == USERFUNC)
				if (!strcmp(tmp->value.funcVal->name, name)) return 1; // symbol find return true
			else 
				if (!strcmp(tmp->value.varVal->name, name)) return 1; // symbol find return true
			
			tmp = tmp->scope_next;
		}
		temp = temp->next;
	}
	return 0;
}



void Initialize(){

	HashInsert("print", 0, LIBFUNC, 0);
	HashInsert("input", 0, LIBFUNC, 0);
	HashInsert("objectmemberkeys", 0, LIBFUNC, 0);
	HashInsert("objecttotalmembers", 0, LIBFUNC, 0);
	HashInsert("objectcopy", 0, LIBFUNC, 0);
	HashInsert("tootalarguments", 0, LIBFUNC, 0);
	HashInsert("argument", 0, LIBFUNC, 0);
	HashInsert("typeof", 0, LIBFUNC, 0);
	HashInsert("strtonum", 0, LIBFUNC, 0);
	HashInsert("sqrt", 0, LIBFUNC, 0);
	HashInsert("cos", 0, LIBFUNC, 0);
	HashInsert("sin", 0, LIBFUNC, 0);


}



bool ScopeListInsert (struct SymbolTableEntry *sym_node, unsigned int scope) {

	struct ScopeListEntry *tmp = scope_head , *new_scope, *prev = NULL;
	struct SymbolTableEntry *parse;
	
	if(tmp == NULL){ // an einai to prwto
		new_scope = (struct ScopeListEntry*)malloc(sizeof(struct ScopeListEntry));
		new_scope->scope = scope ;
		new_scope->next = new_scope->prev = NULL;
		new_scope->symbols = sym_node;
		scope_head = new_scope; 
		return 1;
	}
	else {
		tmp = scope_head;
		while(tmp != NULL){
			if (tmp->scope == scope ){
				parse = tmp->symbols; 
				while(parse->scope_next != NULL) parse = parse->scope_next;
				parse->scope_next = sym_node;
				return 1;
			}
			tmp = tmp->next;
		}
		// an to scope den uparxei create it and sort insert O(n) lmaos
		new_scope = (struct ScopeListEntry*)malloc(sizeof(struct ScopeListEntry));
		new_scope->scope = scope ;
		new_scope->next = new_scope->prev = NULL;
		new_scope->symbols = sym_node;
		
		tmp = scope_head;
		while (tmp != NULL && tmp->scope < scope){
			prev = tmp; 
			tmp = tmp->next; 
		}
		if (tmp == NULL && prev == scope_head){ // an einai na mpei 2o
			prev->next = new_scope; 
			new_scope->prev = prev;
		}
		else if (tmp == NULL && prev != scope_head){ // an einai na mpei sto telos 
			prev->next = new_scope; 
			new_scope->prev = prev;
		}
		else{ // an einai na mpei kapou endiamesa
			prev->next = new_scope; 
			new_scope->prev = prev; 
			new_scope->next = tmp;
			tmp->prev = new_scope;
		}

	}
	return 0;
}




bool HashInsert(char *name, unsigned int line, enum SymbolType type, unsigned int scope){

	int pos = (int)*name % Buckets;
	
	ScopeListEntry *tmp = scope_head, *new_scope;
	SymbolTableEntry *new_sym, *parse;
	Function *new_func;
	Variable *new_var;

	//create the node
	new_sym = (struct SymbolTableEntry*)malloc(sizeof(struct SymbolTableEntry));
	new_sym->next = new_sym->scope_next = new_sym->formal_next = NULL;
	new_sym->isActive = true ;
	new_sym->type = type ;

	if(type == USERFUNC || type == LIBFUNC ) {
		new_func = (struct Function*)malloc(sizeof(struct Function));
		new_func->name = name; 
		new_func->scope = scope; 
		new_func->line=line;
		new_func->next = NULL;
		new_sym->value.funcVal = new_func;
	}
	else if (type == FORMAL){
		new_var = (struct Variable*)malloc(sizeof(struct Variable));
		new_var->name = name ;
		new_var->scope = scope;

	}
	else {
		new_var = (struct Variable*)malloc(sizeof(struct Variable));
		new_var->name = name;
		new_var->scope = scope;
		new_var->line = line;
		new_sym->value.varVal = new_var;
	}
	
	
	ScopeListInsert(new_sym,scope);
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


void PrintHash(){

	int i;
	struct SymbolTableEntry *tmp;


	for (i=0; i<Buckets; i++){
		if(HashTable[i] == NULL) continue;
		tmp = HashTable[i];
		printf("\nHASH NUM %d :",i );
		while (tmp != NULL){
			
			if (tmp->type == LIBFUNC) printf("\"%s\"\t [Library Function]\t (line %d)\t (scope %d)"
				,tmp->value.funcVal->name,tmp->value.funcVal->scope,tmp->value.funcVal->line);
			else if (tmp->type == USERFUNC) printf("\"%s\"\t [User Function]\t (line %d)\t (scope %d)"
				,tmp->value.funcVal->name,tmp->value.funcVal->scope,tmp->value.funcVal->line);
			else if (tmp->type == GLOBAL) printf("\"%s\"\t [Global Variable]\t (line %d)\t (scope %d)"
				,tmp->value.varVal->name,tmp->value.varVal->scope,tmp->value.varVal->line);
			else if (tmp->type == LOCAL) printf("\"%s\"\t [Local Variable]\t (line %d)\t (scope %d)"
				,tmp->value.varVal->name,tmp->value.varVal->scope,tmp->value.varVal->line);
			//else if (tmp->type == FORMAL) printf("\"%s\"\t [Formal Variable]\t (line %d)\t (scope %d)"
				//,tmp->value.varVal->name,tmp->value.varVal->scope,tmp->value.varVal->line);

			if (tmp ->next != NULL)printf("  ||  ");
			tmp = tmp->scope_next; 
		}
	}
}



void PrintScopeList(){

	struct ScopeListEntry *temp = scope_head;
	struct SymbolTableEntry *tmp ;

	while (temp != NULL){

		printf("\n-----------------------------"YEL " SCOPE %d "RESET"----------------------------- \n",temp->scope );
		tmp = temp->symbols;
		while (tmp != NULL){

			if (tmp->type == LIBFUNC) printf("\"%s\"\t [Library Function]\t (line %d)\t (scope %d)"
				,tmp->value.funcVal->name,tmp->value.funcVal->line,tmp->value.funcVal->scope);
			else if (tmp->type == USERFUNC) printf("\"%s\"\t [User Function]\t (line %d)\t (scope %d)"
				,tmp->value.funcVal->name,tmp->value.funcVal->line,tmp->value.funcVal->scope);
			else if (tmp->type == GLOBAL) printf("\"%s\"\t [Global Variable]\t (line %d)\t (scope %d)"
				,tmp->value.varVal->name,tmp->value.varVal->line,tmp->value.varVal->scope);
			else if (tmp->type == LOCAL) printf("\"%s\"\t [Local Variable]\t (line %d)\t (scope %d)"
				,tmp->value.varVal->name,tmp->value.varVal->line,tmp->value.varVal->scope);
			//else if (tmp->type == FORMAL) printf("\"%s\"\t [Formal Variable]\t (line %d)\t (scope %d)"
				//,tmp->value.varVal->name,tmp->value.varVal->scope,tmp->value.varVal->line);

			printf("\n");
			tmp = tmp->scope_next; 
		}
		temp = temp->next;
	}
}


int main (){


	Initialize();

	HashInsert("Lome",3,LIBFUNC,0);
	HashInsert("LOUKAS",1,LIBFUNC,1);
	HashInsert("maria",2,GLOBAL,9);
	HashInsert("manos",3,LIBFUNC,1);
	HashInsert("sakis",3,LOCAL,3);
	HashInsert("gus",3,USERFUNC,4);
	HashInsert("liokis",3,USERFUNC,1); // -1 den emfanizei giati to scope to exw kanei unsigned 
	HashInsert("mastoras",3,LOCAL,2);
	HideScope(1);
	PrintScopeList();
//	PrintHash();

	if (GeneralLookUp("prigisnt")) printf("YES\n");
	else printf("NO\n");
	printf("DONE\n");

	return 0;


}