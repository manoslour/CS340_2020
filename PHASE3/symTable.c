#include "symTable.h"

ScopeListEntry *scope_head = NULL; //Global pointer to the scope list's head
symbol *HashTable[Buckets];
struct errorToken *ERROR_HEAD = NULL; // GLobal pointer to the start of error_tokkens list

quad* quads = (quad*) 0;
unsigned total = 0;
unsigned int currQuad = 0;

unsigned int tempcounter = 0;
extern unsigned int funcprefix;
extern unsigned int currentscope;

unsigned int programVarOffset = 0;
unsigned int functionLocalOffset = 0;
unsigned int formalArgOffset = 0;
unsigned int scopeSpaceCounter = 1;

//----------------------------------------------------------------------------------------------

void resettemp() {tempcounter = 0;}

unsigned int currscope() {return currentscope;}

char* newtempname(){
	char *tempname = malloc(100 * sizeof(char));
	sprintf(tempname, "_t%d", tempcounter++);
	return tempname;
}

symbol* newtemp(){

	symbol* sym;
	char* name = strdup(newtempname());
	sym = scopelookup(name, currscope());
	
	if(sym == NULL)
		// !!! MUST SEE AGAIN !!!
		return tempInsert(name, currscope());
	else
		return sym;
}

symbol* tempInsert(char *name, unsigned int scope){
	
	int pos = (int)*name % Buckets;
	
	ScopeListEntry *tmp = scope_head, *new_scope;
	symbol *new_sym, *parse;

	new_sym = (symbol*) malloc(sizeof(symbol));
	new_sym->next =  NULL; 
	new_sym->scope_next =  NULL; 
	new_sym->isActive = true;
	
	new_sym->name = strdup(name);
	
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
	p->op = op;
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
			e->type = libraryfunc_e;
			break;
		default:
			assert(0);
	}
	return e;
}

symbol* lookup(char* name, unsigned int scope){
	symbol *tmpSymbol, *found;
	ScopeListEntry *tmpScope = scope_head;

	while(tmpScope->next != NULL && tmpScope->scope != scope){
		//printf("Currently at scope %d\n", tmpScope->scope);
		tmpScope = tmpScope->next;
	}
	
	found = scopelookup(name, tmpScope->scope);
	
	if(found != NULL){
		//printf("Found in scope %d, sym_type = %d\n", tmpScope->scope, found->type);
		return found;
	}
	else{
		while (tmpScope != NULL){
			found = scopelookup(name, tmpScope->scope);
			if(found != NULL){
				//printf("Found in scope %d, sym_type = %d\n", tmpScope->scope, found->type);
				return found;
			}
			//printf("Not found in scope %d\n", tmpScope->scope);
			tmpScope = tmpScope->prev;
		}
	}

	//printf("Symbol not found\n");
	return NULL;
}

symbol* scopelookup(char* name, unsigned int scope){

	symbol *tmpSymbol;
	ScopeListEntry *tmpScope = scope_head;

	while (tmpScope->next != NULL && tmpScope->scope != scope){
		//printf("Currently at scope %d\n", tmpScope->scope);
		tmpScope = tmpScope->next;
	}
	
	//printf("Current scope [%d] | Given scope [%d]\n", tmpScope->scope, scope);
	if(tmpScope->scope != scope){
		//printf("Scope [%d] doesnt exist yet\n", scope);
		return NULL;
	}

	tmpSymbol = tmpScope->symbols;
	//printf("scopeLookUp: Entering scope's %d symbol list\n", tmpScope->scope);

	while(tmpSymbol != NULL){
		if(!strcmp(tmpSymbol->name, name)){
			//printf("Symbol %s found in scope %d\n", tmpSymbol->name, tmpScope->scope);
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

void resetformalargsoffset() {formalArgOffset = 0;}

void resetfunctionlocalsoffset() {functionLocalOffset = 0;}

void restorecurrscopeoffset(unsigned int n){
	switch(currscopespace()){
		case programvar:
			programVarOffset = n;
			break;
		case functionlocal:
			functionLocalOffset = n;
			break;
		case formalarg:
			formalArgOffset = n;
			break;
		default:
			assert(0);
	}
}

unsigned int nextquadlabel() {return currQuad;}

void patchlabel(unsigned int quadNo, unsigned int label){
	assert(quadNo < currQuad);
	quads[quadNo].label = label;
}

void printQuads(){
	int i;
	char *arg1, *arg2, *result, *opcode;
	printf("\nQuad#\t\topcode\t\tresult\t\targ1\t\targ2\t\tlabel");
	printf("\n-------------------------------------------------------------------------------------");
	for (i = 0; i < currQuad; i++){

		opcode = strdup(translateopcode((quads+i)->op));

		if ((quads+i)->result == NULL ) { result = "";} 
		else { result = strdup((quads+i)->result->sym->name);} 
		if ((quads+i)->arg1 == NULL ) { arg1 = "";}
		else { arg1 = strdup((quads+i)->arg1->sym->name);}
		if ((quads+i)->arg2 == NULL ) { arg2 = "";}
		else {
			if ( (quads+i)->arg2->sym == NULL) { arg2 = strdup((quads+i)->arg2->strConst);}
            else { arg2 = strdup((quads+i)->arg2->sym->name);}
		}
		printf("\n%d:\t%14s\t%11s\t%11s\t%9s\t%10d", i+1, opcode, result, arg1, arg2, (quads+i)->label);
	}
  	printf("\n\n\n");
}

char* translateopcode(iopcode opcode){
	char* name;
	switch(opcode){
		case 0:		name = "assign"; break;
		case 1:		name = "add"; break;
		case 2:		name = "sub"; break;
		case 3:		name = "mul"; break;
		case 4:		name = "divide"; break;
		case 5:		name = "mod"; break;
		case 6:		name = "uminus"; break;
		case 7:		name = "and"; break;
		case 8:		name = "or"; break;
		case 9:		name = "not"; break;
		case 10:	name = "if_eq"; break;
		case 11:	name = "if_noteq"; break;
		case 12:	name = "if_lesseq"; break;
		case 13:	name = "if_greatereq"; break;
		case 14:	name = "if_less"; break;
		case 15:	name = "if_greater"; break;
		case 16:	name = "call"; break;
		case 17:	name = "param"; break;
		case 18:	name = "ret"; break;
		case 19:	name = "getretval"; break;
		case 20:	name = "funcstart"; break;
		case 21:	name = "funcend"; break;
		case 22:	name = "tablecreate"; break;
		case 23:	name = "tablegetelem"; break;
		case 24:	name = "tablesetelem"; break;
	}
}

expr* member_item (expr* lv, char* name, unsigned int line){
	lv = emit_iftableitem(lv, line); //Emit code if r-value use of table item
	expr* ti = newexpr(tableitem_e); //Make a new expression
	ti->sym = lv->sym;
	ti->index = newexpr_conststring(name); //Cosnt string index
	return ti;
}

expr* newexpr(expr_t t){
	expr* e = (expr*) malloc(sizeof(expr));
	memset(e, 0, sizeof(expr));
	e->type = t;
	return e;
}

expr* newexpr_conststring(char* s){
	expr* e = newexpr(conststring_e);
	e->strConst = strdup(s);
	return e;
}

expr* newexpr_constnum(double i){
	expr* e = newexpr(constnum_e);
	e->numConst = i;
	return e;
}

expr* emit_iftableitem(expr* e, unsigned int line){
	if(e->type != tableitem_e){
		return e;
	}
	else{
		expr* result = newexpr(var_e);
		result->sym = newtemp();
		emit(tablegetelem, e, e->index, result, -1, line);
		return result;
	}
}

expr* make_call(expr* lv, expr* reserved_elist, unsigned int line){
	expr* func = emit_iftableitem(lv, line);
	printf("Entered make_call\n");
	while(reserved_elist){
		emit(param, reserved_elist, NULL, NULL, -1, line);
		reserved_elist = reserved_elist->next;
	}
	emit(call, func, NULL, NULL, -1, line);
	expr* result = newexpr(var_e);
	result->sym = newtemp();
	emit(getretval, NULL, NULL, result, -1, line);
	return result;
}

//----------------------------------------------------------------------------------------------

char* newtempfuncname(){
	char *name = malloc(5 * sizeof(char));
	sprintf(name, "$f%d", funcprefix++);
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
	
	symbol *tmp;
	ScopeListEntry *temp = scope_head;
	
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