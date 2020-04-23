#include "symTable.h"






ScopeListEntry *scope_head = NULL; //Global pointer to the scope list's head
symbol *HashTable[Buckets];
struct errorToken *ERROR_HEAD = NULL; // GLobal pointer to the start of error_tokkens list

quad* quads = (quad*) 0;
unsigned total = 0;
unsigned int currQuad = 0;

unsigned int programVarOffset = 0;
unsigned int functionLocalOffset = 0;
unsigned int formalArgOffset = 0;
unsigned int scopeSpaceCounter = 1;

//----------------------------------------------------------------------------------------------

void expand(){

	assert(total == currQuad);
	quad* p = (quad*) malloc(NEW_SIZE);
	if(quads){
		memcpy(p, quads, CURR_SIZE);
		free(quads);
	}
	quads = p;
	total += EXPAND_SIZE;
}

void emit(iopcode op, expr* arg1, expr* arg2, expr* result, unsigned int label, unsigned int line){

	if(currQuad == total)
		expand();

	quad* p = quads + currQuad++;
	p->arg1 = arg1;
	p->arg2 = arg2;
	p->result = result;
	p->label = label;
	p->line = line;
}

expr* lvalue_expr(symbol* sym){
	assert(sym);
	expr* e = (expr*) malloc(sizeof(expr));
	memset(e, 0, sizeof(expr));

	e->next = (expr*) 0;
	e->sym = sym;

	switch (sym->type){
		case var_s:
			e->type = var_e;
			break;
		case programfunc_s:
			e->type = programfunc_e;
			break;
		case libraryfunc_s:
			e->type = programfunc_e;
			break;
		default:
			assert(0);
	}
	return e;
}

symbol* lookup(char* name, unsigned int scope){

	symbol *tmpSymbol;
	ScopeListEntry *tmpScope = scope_head;

	while (tmpScope->next != NULL && tmpScope->scope != scope){
		printf("Currently at scope %d\n", tmpScope->scope);
		tmpScope = tmpScope->next;
	}

	tmpSymbol = tmpScope->symbols;

	while(tmpSymbol != NULL){
		if(!strcmp(tmpSymbol->name, name)){
			printf("Symbol %s found in scope %d\n", tmpSymbol->name, tmpScope->scope);
			return tmpSymbol;
		}
		tmpSymbol = tmpSymbol->scope_next;
	}
	return NULL;
}

scopespace_t currscopespace(){
	if(scopeSpaceCounter == 1)
		return programvar;
	else if(scopeSpaceCounter % 2 == 0)
		return formalarg;
	else
		return functionlocal;
}

unsigned int currscopeoffset(){
	switch (currscopespace()){
		case programvar:
			return programVarOffset;
		case functionlocal:
			return functionLocalOffset;
		case formalarg:
			return formalArgOffset;
		default:
			assert(0);
	}
}

void inccurrscopeoffset(){
	switch (currscopespace()){
		case programvar:
			++programVarOffset;
			break;
		case functionlocal:
			++functionLocalOffset;
			break;
		case formalarg:
			++formalArgOffset;
			break;
		default:
			assert(0);
	}
}

void enterscopespace(){	
	++scopeSpaceCounter;
}

void exitscopespace(){
	assert(scopeSpaceCounter > 1);
	--scopeSpaceCounter;
}

//----------------------------------------------------------------------------------------------

char* generateName(int nameCount){
	char *name = malloc(5 * sizeof(char));
	sprintf(name, "$f%d", nameCount);
	return name;
}

void addError(char *output, char *content, unsigned int numLine){
    struct errorToken *last;
    struct errorToken *newNode = (struct errorToken *) malloc(sizeof(struct errorToken));
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
    printf("\n-----------------------------  "RED"ERRORS"RESET"   ------------------------------\n\n");

    while(tmp != NULL){
        printf("%d:\t", tmp->numLine);
        printf("%s\t", tmp->output);
        printf("%s\n", tmp->content);

        tmp = tmp->next;
    }
    printf("\n");
}

void hideScope(unsigned int scope){
	
	ScopeListEntry *temp = scope_head;
	symbol *tmp;
	
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

/*int scopeLookUp(char *name, unsigned int scope){
	
	symbol *tmpSymbol;
	ScopeListEntry *tmpScope = scope_head;

	while (tmpScope != NULL){

		if(tmpScope->scope == scope){

			tmpSymbol = tmpScope->symbols;
			
			while (tmpSymbol != NULL) {
				
				if (tmpSymbol->type == Libfunc && tmpSymbol->isActive == 1){
					if (!strcmp(tmpSymbol->name, name))
						return 1; // Libfunc found
				} 
				if (tmpSymbol->type == Userfunc && tmpSymbol->isActive == 1){
					if (!strcmp(tmpSymbol->name, name)) 
						return 2; // Userfunc found
				}
				if(tmpSymbol->type == Global && tmpSymbol->isActive == 1){
					if (!strcmp(tmpSymbol->name, name)) 
						return 3; // Global Variable found
				}
				if(tmpSymbol->type == Local && tmpSymbol->isActive == 1){
					if (!strcmp(tmpSymbol->name, name)) 
						return 4; // Local Variable found
				}
				if(tmpSymbol->type == Formal && tmpSymbol->isActive == 1){
					if (!strcmp(tmpSymbol->name, name))
						return 5; // Formal Variable found
				}

				tmpSymbol = tmpSymbol->scope_next;
			}
		} 
		tmpScope = tmpScope->next;
	} 
	return 0;
}
*/
/*int generalLookUp(char *name, unsigned int scope){

	int result = 0;
	symbol *tmpSymbol;
	ScopeListEntry *tmpScope = scope_head; 

	while (tmpScope->next != NULL && tmpScope->scope != scope){
		//printf("Currently at scope %d\n", tmpScope->scope);
		tmpScope = tmpScope->next;
	}

	// If tmpScope->scope < scope, given scope doesnt exist yet. Works as expected.
	//printf("Arrived at given scope[%d|%d]\n", tmpScope->scope, scope);
	result = scopeLookUp(name, tmpScope->scope);

	if(result != 0){
		//printf("Found in scope %d, result = %d\n", tmpScope->scope, result);
		return result;
	}
	else{
		while (tmpScope != NULL){
			result = scopeLookUp(name, tmpScope->scope);
			if(result != 0){
				//printf("Found in scope %d, result = %d\n", tmpScope->scope, result);
				break;
			}
			//printf("Not found in scope %d, result = %d\n", tmpScope->scope, result);
			tmpScope = tmpScope->prev;
		}
	}

	return result;
}*/

void initialize(){

	hashInsert("print", 0, 0, libraryfunc_s, 0, 0);
	hashInsert("input", 0 , 0, libraryfunc_s, 0, 0);
	hashInsert("objectmemberkeys", 0, 0, libraryfunc_s, 0, 0);
	hashInsert("objecttotalmembers", 0, 0, libraryfunc_s, 0, 0);
	hashInsert("objectcopy", 0, 0, libraryfunc_s, 0, 0);
	hashInsert("tootalarguments", 0, 0, libraryfunc_s, 0, 0);
	hashInsert("argument", 0, 0, libraryfunc_s, 0, 0);
	hashInsert("typeof", 0, 0, libraryfunc_s, 0, 0);
	hashInsert("strtonum", 0, 0, libraryfunc_s, 0, 0);
	hashInsert("sqrt", 0, 0, libraryfunc_s, 0, 0);
	hashInsert("cos", 0, 0, libraryfunc_s, 0, 0);
	hashInsert("sin", 0, 0, libraryfunc_s, 0, 0);
}

bool scopeListInsert (symbol *sym_node, unsigned int scope) {

	ScopeListEntry *tmp = scope_head , *new_scope, *prev = NULL;
	symbol *parse;

	if(scope_head == NULL){
		new_scope = (ScopeListEntry*) malloc(sizeof(ScopeListEntry));
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

		new_scope = (ScopeListEntry*) malloc(sizeof(ScopeListEntry));
		new_scope->scope = scope ;
		new_scope->next = new_scope->prev = NULL;
		new_scope->symbols = sym_node;
		
		tmp = scope_head;
		while (tmp != NULL && tmp->scope < scope){
			prev = tmp; 
			tmp = tmp->next; 
		}
		if (tmp == NULL && prev == scope_head){
			prev->next = new_scope; 
			new_scope->prev = prev;
		}
		else if (tmp == NULL && prev != scope_head){
			prev->next = new_scope; 
			new_scope->prev = prev;
		}
		else{
			prev->next = new_scope; 
			new_scope->prev = prev; 
			new_scope->next = tmp;
			tmp->prev = new_scope;
		}
	}
	return 0;
}

symbol* hashInsert(char *name, unsigned int scope, unsigned int line, symbol_t type, scopespace_t space, unsigned int offset){
	
	int pos = (int)*name % Buckets;
	
	ScopeListEntry *tmp = scope_head, *new_scope;
	symbol *new_sym, *parse;
	

	new_sym = (symbol*) malloc(sizeof(symbol));
	new_sym->next =  NULL; 
	new_sym->scope_next =  NULL; 
	new_sym->isActive = true;
	
	new_sym->name = strdup(name);
	new_sym->scope = scope;
	new_sym->line = line; 
	new_sym->type = type;
	new_sym->space = space;
	new_sym->offset = offset; 
	
	scopeListInsert(new_sym,scope);
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
void printScopeList(){

	ScopeListEntry *temp = scope_head;
	symbol *tmp ;

	while (temp != NULL){

		printf("\n-----------------------------  "YEL"SCOPE #%d"RESET"  ----------------------------- \n\n",temp->scope );
		tmp = temp->symbols;
		while (tmp != NULL){
			if (tmp->type == var_s) printf("\"%s\"\t [Variable]\t (line %d)\t (scope %d)"
				,tmp->name,tmp->line,tmp->scope);
			else if (tmp->type == programfunc_s)printf("\"%s\"\t [Program Function]\t (line %d)\t (scope %d)"
				,tmp->name,tmp->line,tmp->scope);
			else if (tmp->type == libraryfunc_s)printf("\"%s\"\t [Library Function]\t (line %d)\t (scope %d)"
				,tmp->name,tmp->line,tmp->scope);

			printf("\n");
			tmp = tmp->scope_next; 
		}
		temp = temp->next;
	}
	printf("\n");
}