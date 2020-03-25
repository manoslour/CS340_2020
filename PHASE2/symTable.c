#include "symTable.h"

ScopeListEntry *scope_head = NULL; //Global pointer to the scope list's head
SymbolTableEntry *HashTable[Buckets];
struct errorToken *ERROR_HEAD = NULL; // GLobal pointer to the start of error_tokkens list

char* generateName(int nameCount){
	char *name = "$";
	char buffer[100];
	char number = itoa(nameCount, buffer, 10);
	strcat(name, number);
	return name;
}

void addError(char *output, char *content, unsigned int numLine){
    struct errorToken *last;
    struct errorToken *newNode = (struct errorToken *)malloc(sizeof(struct errorToken));
    char *tmpOutput = strdup(output);
    char *tmpContent = strdup(content);

    newNode->output = tmpOutput;
    newNode->content = tmpContent;
    newNode->numLine = numLine;
    newNode->next = NULL;

    last = ERROR_HEAD;

    if(ERROR_HEAD == NULL){
        ERROR_HEAD = newNode;
    }else{
        while(last->next != NULL){
            last = last->next;
        }
        last->next = newNode;
    }
}

void printErrorList(){

    struct errorToken *tmp = ERROR_HEAD;
    printf("\n------------------\tERRORS - WARNINGS\t------------------\n\n");

    while(tmp != NULL){
        printf("%d:\t", tmp->numLine);
        printf("%s\t", tmp->output);
        printf("%s\n", tmp->content);

        tmp = tmp->next;
    }
    printf("\n");
}

bool insertFormal(struct SymbolTableEntry *funcname, struct SymbolTableEntry *formalEntry){

	struct SymbolTableEntry *tmp, *parse;

	//an pas na valeis var se function h formal se var h var anti gia formal efuges kai den kaneis tpt 
	if ((funcname->type != Libfunc && funcname->type != Userfunc) || formalEntry->type != Formal ) {
		printf("you are trying to add formals to a variable , not a function"); 
		return 0;
	}
	
	parse = funcname;
	if (parse->formal_next == NULL) parse->formal_next = formalEntry;
	else {
		while (parse->formal_next != NULL) parse = parse->formal_next;
		parse->formal_next = formalEntry; 
	}
	return 1;
}

void printFormals(){

	struct SymbolTableEntry *tmp, *parse;
	struct ScopeListEntry *temp = scope_head;

	while (temp != NULL){

		tmp = temp->symbols;
		while (tmp != NULL){

			if (tmp->type == Libfunc || tmp->type == Userfunc){
				parse = tmp;
				printf("Function \"%s\" has formals:\n", tmp->value.funcVal->name);
				while (parse->formal_next != NULL) {
					printf("\t\"%s\" [Formal] (line %d) (scope %d)\n", parse->formal_next->value.varVal->name, parse->formal_next->value.varVal->line, parse->formal_next->value.varVal->scope);
					parse = parse->formal_next;
				}
				printf("\n");
			}
			tmp = tmp->scope_next;
		}
		temp = temp->next;
	}
}

void activateScope(unsigned int scope){

	ScopeListEntry *temp = scope_head;
	SymbolTableEntry *tmp;
	
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

void hideScope(unsigned int scope){
	
	ScopeListEntry *temp = scope_head;
	SymbolTableEntry *tmp;
	
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
int scopeLookUp(char *name, unsigned int scope){
	
	ScopeListEntry *temp = scope_head;
	SymbolTableEntry *tmp;

	while (temp != NULL){
		if(scope == temp->scope){
			tmp = temp->symbols;
			while (tmp != NULL) {
				
				if (tmp->type == Libfunc || tmp->type == Userfunc){
					if (!strcmp(tmp->value.funcVal->name, name)) return 2; // symbol find in a current scope return true
				} 
				
				else if(tmp->type == Global || tmp->type == Local || tmp->type == Formal ){
					if (!strcmp(tmp->value.varVal->name, name)) return 1; // symbol find in a current scope return true
				}
				
				tmp = tmp->scope_next;
			}
		} 
		temp = temp->next;
	} 
	return 0;
}

// Performs lookup from current 
int generalLookUp(char *name, unsigned int scope){
	
	int flag = 0;
	SymbolTableEntry *tmp;
	ScopeListEntry *temp = scope_head;

	while (temp != NULL){
		
		if (temp->scope == scope) {
			tmp = temp->symbols;
			flag = 1;
			while (tmp != NULL) {
				
				if (tmp->type == Libfunc || tmp->type == Userfunc)
					if (!strcmp(tmp->value.funcVal->name, name)) return scope;// symbol find in a current scope return true
				else 
					if (!strcmp(tmp->value.varVal->name, name)) return scope; // symbol find in a current scope return true
				
				tmp = tmp->scope_next;
			}
		}

		if (flag == 1){
			temp = temp->prev;
			scope--;
		}
		else temp = temp->next;
	}
	return -1;
}

void initialize(){

	hashInsert("print", 0, Libfunc, 0);
	hashInsert("input", 0, Libfunc, 0);
	hashInsert("objectmemberkeys", 0, Libfunc, 0);
	hashInsert("objecttotalmembers", 0, Libfunc, 0);
	hashInsert("objectcopy", 0, Libfunc, 0);
	hashInsert("tootalarguments", 0, Libfunc, 0);
	hashInsert("argument", 0, Libfunc, 0);
	hashInsert("typeof", 0, Libfunc, 0);
	hashInsert("strtonum", 0, Libfunc, 0);
	hashInsert("sqrt", 0, Libfunc, 0);
	hashInsert("cos", 0, Libfunc, 0);
	hashInsert("sin", 0, Libfunc, 0);
}

bool scopeListInsert (struct SymbolTableEntry *sym_node, unsigned int scope) {

	ScopeListEntry *tmp = scope_head , *new_scope, *prev = NULL;
	SymbolTableEntry *parse;
	
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

struct SymbolTableEntry *hashInsert(char *name, unsigned int line, enum SymbolType type, unsigned int scope){
	
	int pos = (int)*name % Buckets;
	
	ScopeListEntry *tmp = scope_head, *new_scope;
	SymbolTableEntry *new_sym, *parse;
	Function *new_func;
	Variable *new_var;

	//create the node
	new_sym = (struct SymbolTableEntry*)malloc(sizeof(struct SymbolTableEntry));
	new_sym->next =  NULL; 
	new_sym->scope_next =  NULL; 
	new_sym->formal_next = NULL;
	new_sym->isActive = true ;
	new_sym->type = type ;

	if(type == Userfunc || type == Libfunc ) {
		new_func = (struct Function*)malloc(sizeof(struct Function));
		new_func->name = (char*)malloc(strlen(name+1));
		strcpy((char*)new_func->name, name);
		new_func->scope = scope; 
		new_func->line=line;
		new_sym->value.funcVal = new_func;
	}
	else if (type == Formal){
		new_var = (struct Variable*)malloc(sizeof(struct Variable));
		new_var->name = (char*)malloc(strlen(name+1));
		strcpy((char*)new_var->name, name);
		new_var->scope = scope;
		new_var->line = line;
		new_sym->value.varVal = new_var;
		printf("\n\n TO POS STO HASH EINAI %d",pos);
	}
	else {
		new_var = (struct Variable*)malloc(sizeof(struct Variable));
		new_var->name = (char*)malloc(strlen(name+1));
		strcpy((char*)new_var->name, name);
		new_var->scope = scope;
		new_var->line = line;
		new_sym->value.varVal = new_var;
	}
	
	scopeListInsert(new_sym,scope);
	//if its the first
	if (HashTable[pos] == NULL){
		HashTable[pos] = new_sym;
		return new_sym; 
	}
	else {
		parse = HashTable[pos];
		while (parse->next != NULL) parse = parse->next;

		parse->next = new_sym;
		return new_sym;
	}
	return NULL;

}

void printHash(){

	int i;
	SymbolTableEntry *tmp;

	for (i=0; i<Buckets; i++){
		if(HashTable[i] == NULL) continue;
		tmp = HashTable[i];
		printf("\nHASH NUM %d :",i );
		while (tmp != NULL){
			
			if (tmp->type == Libfunc) printf("\"%s\"\t [Library Function]\t (line %d)\t (scope %d)"
				,tmp->value.funcVal->name,tmp->value.funcVal->scope,tmp->value.funcVal->line);
			else if (tmp->type == Userfunc) printf("\"%s\"\t [User Function]\t (line %d)\t (scope %d)"
				,tmp->value.funcVal->name,tmp->value.funcVal->scope,tmp->value.funcVal->line);
			else if (tmp->type == Global) printf("\"%s\"\t [Global Variable]\t (line %d)\t (scope %d)"
				,tmp->value.varVal->name,tmp->value.varVal->scope,tmp->value.varVal->line);
			else if (tmp->type == Local) printf("\"%s\"\t [Local Variable]\t (line %d)\t (scope %d)"
				,tmp->value.varVal->name,tmp->value.varVal->scope,tmp->value.varVal->line);
			else if (tmp->type == Formal) printf("\"%s\"\t [Formal Variable]\t (line %d)\t (scope %d)"
				,tmp->value.varVal->name,tmp->value.varVal->scope,tmp->value.varVal->line);

			if (tmp ->next != NULL)printf("  ||  ");
			tmp = tmp->scope_next; 
		}
	}
}

void printScopeList(){

	ScopeListEntry *temp = scope_head;
	SymbolTableEntry *tmp ;

	while (temp != NULL){

		printf("\n-----------------------------"YEL " SCOPE %d "RESET"----------------------------- \n",temp->scope );
		tmp = temp->symbols;
		while (tmp != NULL){

			if (tmp->type == Libfunc) printf("\"%s\"\t [Library Function]\t (line %d)\t (scope %d)"
				,tmp->value.funcVal->name,tmp->value.funcVal->line,tmp->value.funcVal->scope);
			else if (tmp->type == Userfunc) printf("\"%s\"\t [User Function]\t (line %d)\t (scope %d)"
				,tmp->value.funcVal->name,tmp->value.funcVal->line,tmp->value.funcVal->scope);
			else if (tmp->type == Global) printf("\"%s\"\t [Global Variable]\t (line %d)\t (scope %d)"
				,tmp->value.varVal->name,tmp->value.varVal->line,tmp->value.varVal->scope);
			else if (tmp->type == Local) printf("\"%s\"\t [Local Variable]\t (line %d)\t (scope %d)"
				,tmp->value.varVal->name,tmp->value.varVal->line,tmp->value.varVal->scope);
			else if (tmp->type == Formal) printf("\"%s\"\t [Formal Variable]\t (line %d)\t (scope %d)"
				,tmp->value.varVal->name,tmp->value.varVal->scope,tmp->value.varVal->line);

			printf("\n");
			tmp = tmp->scope_next; 
		}
		temp = temp->next;
	}
}


int main(){
	/*
	SymbolTableEntry *func1, *formal1, *formal2;
	initialize();
	func1 = hashInsert("doSomething", 0, Userfunc, 2);
	formal1 = hashInsert("x", 0, Formal, 3);
	formal2 = hashInsert("y", 0, Formal, 3);
	printScopeList();
	//insertFormal(func1, formal1);
	//insertFormal(func1, formal2);
	printf("H LOUKUP DINEI %d\n",scopeLookUp("x",3));
	
	//printFormals();
	*/

	printf("FUNCTION NAME = %s", generateName(3));
}