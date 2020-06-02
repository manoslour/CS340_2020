#include "target.h"

FILE* fp;

extern quad *quads;
extern unsigned total;
extern unsigned currQuad;

unsigned currProcessedQuad = 0;

unsigned ij_total = 0;
incomplete_jump* ij_head = (incomplete_jump*) 0;

instruction* instructions = (instruction*) 0;
unsigned totalInstructions = 0;
unsigned currInstr = 0;

double* numConsts = (double*) 0;
unsigned totalNumConsts = 0;

char** stringConsts = (char**) 0;
unsigned totalStringConsts = 0;

char** namedLibfuncs = (char**) 0;
unsigned totalNamedLibfuncs = 0;

userfunc* userFuncs = (userfunc*) 0;
unsigned totalUserFuncs = 0;

funcStack* funcstack = (funcStack*) 0;

generator_func_t generators[] = {
    generate_ASSIGN,
    generate_ADD,
    generate_SUB,
    generate_MUL,
    generate_DIV,
    generate_MOD,
	generate_UMINUS,
	generate_AND, 
    generate_OR,
    generate_NOT,
    generate_IF_EQ,
    generate_IF_NOTEQ,
    generate_IF_LESSEQ,
    generate_IF_GREATEREQ,
    generate_IF_LESS,
    generate_IF_GREATER,
    generate_CALL,
    generate_PARAM,
    generate_RETURN,
    generate_GETRETVAL,
    generate_FUNCSTART,
    generate_FUNCEND,
    generate_NEWTABLE,
    generate_TABLEGETELM,
    generate_TABLESETELM,
    generate_JUMP,
    generate_NOP
};

//-------------------------------------------------

void avmbinaryfile(){
	fp = fopen("target.abc", "w+");
	fprintf(fp, "%d\n\n", magicnumber());
	arrays();
	printInstructions();
	fclose(fp);
}

unsigned magicnumber(){
	return 42069;
}

void arrays(){
	strings();
	numbers();
	userfunctions();
	libfunctions();
}

void strings(){
	fprintf(fp, "#String Consts Array\n%d\n",totalStringConsts);
	for(int i = 0; i < totalStringConsts; i++)
		fprintf(fp, "%d %s\n", i, stringConsts[i]);
	fprintf(fp, "\n");
}

void numbers(){
	fprintf(fp, "#Number Consts Array\n%d\n",totalNumConsts);
	for(int i = 0; i < totalNumConsts; i++)
		fprintf(fp, "%d %f\n", i, numConsts[i]);
	fprintf(fp, "\n");
}

void userfunctions(){
	fprintf(fp, "#Userfunc Consts Array\n%d\n",totalUserFuncs);
	for(int i = 0; i < totalUserFuncs; i++)
		fprintf(fp, "%d %d %d %s\n", i, userFuncs[i].address, userFuncs[i].localSize, userFuncs[i].id);
	fprintf(fp, "\n");
}

void libfunctions(){
	initLibfuncs();
	fprintf(fp, "#Libfunc Consts Array\n%d\n", totalNamedLibfuncs);
	for(int i = 0; i < totalNamedLibfuncs; i++)
		fprintf(fp, "%d %s\n", i, namedLibfuncs[i]);
	fprintf(fp, "\n");
}

void initLibfuncs(){
	libfuncs_newused("print");
	libfuncs_newused("input");
	libfuncs_newused("objectmemberkeys");
	libfuncs_newused("objecttotalmembers");
	libfuncs_newused("objectcopy");
	libfuncs_newused("tootalarguments");
	libfuncs_newused("argument");
	libfuncs_newused("typeof");
	libfuncs_newused("strtonum");
	libfuncs_newused("sqrt");
	libfuncs_newused("cos");
	libfuncs_newused("sin");
}

unsigned consts_newnumber (double n){

	unsigned ret_index;

	if(numConsts == NULL)
		numConsts = (double*) malloc(sizeof(double));
	else{
		double* new_array = (double*) realloc(numConsts, (totalNumConsts + 1) * sizeof(double));
		numConsts = new_array;
	}

	ret_index = totalNumConsts;
	numConsts[totalNumConsts++] = n;

	return ret_index;
}

unsigned consts_newstring (char* s){

	unsigned ret_index;

	if(stringConsts == NULL)
        stringConsts = (char**) malloc(sizeof(char));
    else{
        char** new_array = (char**) realloc(stringConsts, (totalStringConsts + 1) * sizeof(char));
        stringConsts = new_array;
    }

	ret_index = totalStringConsts;
    stringConsts[totalStringConsts++] = strdup(s);
    
	return ret_index;
}

unsigned libfuncs_newused (char* s){
	unsigned ret_index;

	if(namedLibfuncs == NULL)
		namedLibfuncs = (char**) malloc((sizeof(char) + 8) * 12);
	ret_index = totalNamedLibfuncs;
	namedLibfuncs[totalNamedLibfuncs++] = strdup(s);
	
	return ret_index;
}

unsigned userfuncs_newfunc(symbol* sym){

	if(totalUserFuncs){
		for(int i = 0; i < totalUserFuncs; i++){
			if(strcmp(sym->name, userFuncs[i].id) == 0){
				printf("Function %s already in userFuncs Array\n", sym->name);
				return i;
			}
		}
	}

	unsigned ret_index;
	if(userFuncs == NULL)
		userFuncs = (userfunc*) malloc(sizeof(userfunc));
	else{
		userfunc* new_array = (userfunc*) realloc(userFuncs, (totalUserFuncs + 1) * sizeof(userfunc));
		userFuncs = new_array;
	}

	ret_index = totalUserFuncs;

	userFuncs[totalUserFuncs].address = sym->offset; // Is offset the wanted field?
	userFuncs[totalUserFuncs].localSize = sym->totalLocals;
	userFuncs[totalUserFuncs].id = strdup(sym->name);

	totalUserFuncs++;
	return ret_index;
}

void expandInstr(){

	// printf("Entered expand instr\n");
	// printf("TOtalinstr = %d | currINstr = %d\n", totalInstructions, currInstr);

	// assert(totalInstructions == currInstr);

	// instruction* t = (instruction*) malloc(NEW_INSTR_SIZE);

	// printf("problema\n");
	// if(instructions){
	// 	printf("entered if\n");
	// 	memcpy(t, instructions, CURR_INSTR_SIZE);
	// 	free(instructions);
	// }
	// instructions = t;
	// totalInstructions += EXPAND_INSTR_SIZE;
}

void emit_instr(instruction *t){
	printf("Entered emit_instr\n");

	// EXPAND INSTRUCTIONS
	if (instructions == NULL)
		instructions = (instruction*) malloc(sizeof(instruction));
	else {
		instruction* new_instr = (instruction*) realloc(instructions, (totalInstructions + 1) * sizeof(instruction));
		instructions = new_instr; 
	} 
	totalInstructions ++;

	instruction* i =(instruction*) malloc(sizeof(instruction));
	i = instructions + currInstr ;
	i->opcode = t->opcode;
	i->result = t->result;
	i->arg1 = t->arg1;
	i->arg2 = t->arg2;
	i->srcLine = t->srcLine;

	currInstr++;
	printf("Exiting emit_instr\n");
}

unsigned nextinstructionlabel(void){
	return currInstr;
}

unsigned currprocessedquad(){
	return currProcessedQuad;
}

int isEmptyFunc(){
    if(funcstack == NULL)
        return 1;
    else
        return 0;
}

symbol* peekFunc(){
    if(isEmptyFunc()){
        printf("Stack is empty\n");
        return NULL;
    }
    else
        return funcstack->func;
}

void pushFunc(symbol* func){
    funcStack *newNode = (funcStack*) malloc(sizeof(funcStack));
    newNode->func = func;
    newNode->next = funcstack;
    funcstack = newNode;
    printf("%s pushed to stack\n", newNode->func->name);
}

symbol* popFunc(){
	peekFunc();
    symbol* func;
    if(isEmptyFunc()){
        return NULL;
	}
    else{
        func = peekFunc();
        funcstack = funcstack->next;
        return func;
    }
}

instruction* createInstruction (){

	instruction* t = malloc(sizeof(instruction)); 

	t->arg1 = malloc(sizeof(vmarg));
	t->arg2 = malloc(sizeof(vmarg));
	t->result = malloc(sizeof(vmarg));

	return t; 
}

//-------------------------------------------------

void make_operand (expr* e, vmarg* arg){
	
	printf("Entered make_operand\n");

	switch (e->type){
		case var_e :
		case assignexpr_e:
		case tableitem_e :
		case arithexpr_e :
		case boolexpr_e :
		case newtable_e :{
			assert(e->sym);
			arg->val = e->sym->offset; 
			switch (e->sym->space){
				case programvar   : arg->type = global_a; break;
				case functionlocal: arg->type = local_a;  break;
				case formalarg	  : arg->type = formal_a; break;
				default : assert(0);
			}
			break; /* from case newtable_e */
		}

		/* Constants */

		case constbool_e: {
			printf("constbool_e case \n");
			arg->val = e->boolConst;
			arg->type = bool_a;
			break;
		}
		case conststring_e :{
			printf("conststring_e case \n");
			arg->val = consts_newstring(e->strConst); 
			arg->type = string_a; 
			break;
		}
		case constnum_e: {
			printf("constnum_e case \n");
			arg->val = consts_newnumber(e->numConst); 
			arg->type = number_a; 
			break;
		}
		case nil_e: {
			printf("nil_e case \n");
			arg->type = nil_a; 
			break;
		}
		case programfunc_e : {
			printf("programfunc_e case \n");
			arg->type = userfunc_a ; 
			arg->val = userfuncs_newfunc(e->sym);
			break; 
		} 
		case libraryfunc_e: {
			printf("libraryfunc_e case \n");
			arg->type = libfunc_a; 
			arg->val = libfuncs_newused(strdup(e->sym->name));
			break;
		}
		default: assert(0);
	}
}

void generate (vmopcode op, quad* quad ){
	printf("Entered generate\n");
	instruction* t = createInstruction();  
	t->opcode = op;
	t->srcLine = quad->line;

	if(quad->arg1)
		make_operand(quad->arg1, t->arg1);
	else
		t->arg1 = NULL;
	if(quad->arg2)	
		make_operand(quad->arg2, t->arg2);
	else
		t->arg2 = NULL;
	if(quad->result)	
		make_operand(quad->result, t->result);
	else
		t->result = NULL;

	quad->taddress = nextinstructionlabel();
	emit_instr(t);
}

void make_numberoperand (vmarg* arg, double val){
	arg->val = consts_newnumber(val); 
	arg->type = number_a;
}

void make_booloperand (vmarg *arg, unsigned val){
	arg->val = val; 
	arg->type = bool_a;
}

void make_retvaloperand (vmarg *arg){ 
	arg->type = retval_a;
	arg->val = -1; 
}


void generate_relational(vmopcode op, quad* quad){
	instruction* t = (instruction*) malloc(sizeof(instruction)); 
	t->opcode = op;
	t->arg1 = malloc(sizeof(vmarg));
	t->arg2 = malloc(sizeof(vmarg));
	t->result = malloc(sizeof(vmarg));

	if(quad->arg1)
		make_operand(quad->arg1, t->arg1);
	else
		t->arg1->type = t->arg1->val = -1;
	if(quad->arg2)
		make_operand(quad->arg2, t->arg2);
	else
		t->arg2->type = t->arg2->val = -1;

	t->result->type = label_a;

	if(quad->label < currprocessedquad())
		t->result->val = quads[quad->label].taddress;
	else
		add_incomplete_jump(nextinstructionlabel(), quad->label);
	
	quad->taddress = nextinstructionlabel();
	t->srcLine = quad->line;

	emit_instr(t);
}

void generate_ADD(quad* q) { generate(add_v, q); }
void generate_SUB(quad* q) { generate(sub_v, q); }
void generate_MUL(quad* q) { generate(mul_v, q); }
void generate_DIV(quad* q) { generate(div_v, q); }
void generate_MOD(quad* q) { generate(mod_v, q); }
void generate_UMINUS(quad* q) { /*DOES NOTHING*/ }
void generate_NEWTABLE (quad* q) { generate(newtable_v, q); }
void generate_TABLEGETELM (quad* q) { generate(tablegetelem_v, q); }
void generate_TABLESETELM (quad* q) { generate(tablesetelem_v, q); }
void generate_ASSIGN (quad* q) { generate(assign_v, q); }

void generate_NOP (quad* q){
	instruction* t = createInstruction();
	t->opcode = nop_v;
	emit_instr(t);
}
void generate_JUMP(quad* q) { generate_relational(jump_v, q); }
void generate_IF_EQ(quad* q) { generate_relational(jeq_v, q); }
void generate_IF_NOTEQ(quad* q) { generate_relational(jne_v, q); }
void generate_IF_GREATER(quad* q) { generate_relational(jgt_v, q); }
void generate_IF_GREATEREQ(quad* q) { generate_relational(jge_v, q); }
void generate_IF_LESS(quad* q) { generate_relational(jlt_v, q); }
void generate_IF_LESSEQ(quad* q) { generate_relational(jle_v, q); }

void generate_NOT (quad* q){
	q->taddress = nextinstructionlabel();
	instruction* t =  createInstruction();

	t->opcode = jeq_v; 
	t->srcLine = q->line;

	make_operand(q->arg1, t->arg1);
	make_booloperand(t->arg2, 0); 
	t->result->type = label_a;
	t->result->val = nextinstructionlabel() + 3;
	emit_instr(t);

	instruction* t2 = createInstruction();
	t2->srcLine = q->line;
	t2->opcode = assign_v; 
	make_booloperand(t2->arg1, 0);
	t2->arg2 = NULL;//reset_operant
	make_operand(q->result, t2->result); 
	emit_instr(t2);

	instruction* t3  = createInstruction();
	t3->srcLine = q->line;
	t3->opcode = jump_v; 
	t3->arg1 = NULL;//reset_operant
	t3->arg2 = NULL;//reset_operant
	t3->result->type = label_a; 
	t3->result->val = nextinstructionlabel() + 2; 
	emit_instr(t3);

	instruction* t4  = createInstruction();
	t4->srcLine = q->line;
	t4->opcode = assign_v;
	make_booloperand(t4->arg1, 1); 
	t4->arg2 = NULL; //reset_operant
	make_operand(q->result, t4->result); 
	emit_instr(t4);
}

void generate_OR (quad* q){
	q->taddress = nextinstructionlabel(); 
	instruction* t = createInstruction();
	t->srcLine = q->line;
	t->opcode = jeq_v; 
	make_operand(q->arg1, t->arg1); 
	make_booloperand(t->arg2, 1); 
	t->result->type = label_a;
	t->result->val = nextinstructionlabel() + 4;
	emit_instr(t);

	instruction* t1 = createInstruction();
	t1->srcLine = q->line;
	make_operand(q->arg2, t1->arg1);
	make_booloperand(t1->arg2, 1); 
	t1->result->type = label_a;
	t1->result->val = nextinstructionlabel() + 3;
	emit_instr(t1); 

	instruction* t2 = createInstruction();
	t2->srcLine = q->line;
	t2->opcode = assign_v; 
	make_booloperand(t2->arg1, 0); 
	t2->arg2 = NULL; // reset_operand; 
	make_operand(q->result, t2->result); 
	emit_instr(t2); 

	instruction* t3 = createInstruction();
	t3->srcLine = q->line;
	t3->opcode = jump_v; 
	t3->arg1 = NULL; // reset_operand;
	t3->arg2 = NULL; // reset_operand;
	t3->result->type = label_a; 
	t3->result->val = nextinstructionlabel() + 2; 
	emit_instr(t3);

	instruction* t4 = createInstruction();
	t4->srcLine = q->line;
	t4->opcode = assign_v; 
	make_booloperand(t4->arg1, 1); 
	t4->arg2 = NULL; //reset_operand
	make_operand(q->result, t4->result); 
	emit_instr(t4);
}

void generate_AND (quad* q){
	q->taddress = nextinstructionlabel(); 
	instruction* t = createInstruction();
	t->srcLine = q->line;
	t->opcode = jeq_v; 
	make_operand(q->arg1, t->arg1); 
	make_booloperand(t->arg2, 0); 
	t->result->type = label_a;
	t->result->val = nextinstructionlabel() + 4;
	emit_instr(t);

	instruction* t1 = createInstruction();
	t1->srcLine = q->line;
	make_operand(q->arg2, t1->arg1);
	make_booloperand(t1->arg2, 0); 
	t1->result->type = label_a;
	t1->result->val = nextinstructionlabel() + 3;
	emit_instr(t1); 

	instruction* t2 = createInstruction();
	t2->srcLine = q->line;
	t2->opcode = assign_v; 
	make_booloperand(t2->arg1, 1); 
	t2->arg2 = NULL; // reset_operand; 
	make_operand(q->result, t2->result); 
	emit_instr(t2); 

	instruction* t3 = createInstruction();
	t3->srcLine = q->line;
	t3->opcode = jump_v; 
	t3->arg1 = NULL; // reset_operand;
	t3->arg2 = NULL; // reset_operand;
	t3->result->type = label_a; 
	t3->result->val = nextinstructionlabel() + 2; 
	emit_instr(t3);

	instruction* t4 = createInstruction();
	t4->srcLine = q->line;
	t4->opcode = assign_v; 
	make_booloperand(t4->arg1, 0); 
	t4->arg2 = NULL; //reset_operand
	make_operand(q->result, t4->result); 
	emit_instr(t4);
}

void generate_PARAM (quad* q){
	printf("Entered generate_PARAM\n");
	q->taddress = nextinstructionlabel(); 
	instruction* t = createInstruction();
	t->srcLine = q->line;
	t->opcode = pusharg_v;

	t->arg2 = NULL;
	t->result = NULL;
	
	if(q->arg1)
		make_operand(q->arg1, t->arg1); 
	emit_instr(t); 
}

void generate_CALL(quad* q){
	q->taddress = nextinstructionlabel();
	instruction* t = createInstruction();
	t->srcLine = q->line;
	t->opcode = call_v;

	t->arg2 = NULL;
	t->result = NULL;
	
	if(q->arg1)
		make_operand(q->arg1, t->arg1);
	emit_instr(t);
}

void generate_GETRETVAL (quad* q){
	printf("Entered gen_GETRETVAL\n");
	q->taddress = nextinstructionlabel(); 
	instruction* t= createInstruction();
	t->srcLine = q->line;
	t->opcode = assign_v;

	t->arg2 = NULL;

	if(q->result) 
		make_operand(q->result, t->result);
	make_retvaloperand(t->arg1);
	emit_instr(t); 
}

void generate_FUNCSTART(quad* q){
	symbol* f = q->result->sym;
	f->taddress = nextinstructionlabel();
	q->taddress = nextinstructionlabel();

	userfuncs_newfunc(f);
	pushFunc(f);
	symbol* b = peekFunc();

	instruction* t = createInstruction();
	t->srcLine = q->line;
	t->opcode = funcenter_v;
	t->arg1 = t->arg2 = NULL;
	make_operand(q->result, t->result);
	emit_instr(t);
}

void generate_RETURN(quad* q){
	printf("Entered get_RETURN\n");

	q->taddress = nextinstructionlabel();	
	instruction* t = createInstruction();
	t->srcLine = q->line;
	t->opcode = assign_v;
	t->arg2 = NULL;
	make_retvaloperand(t->result);
	if(q->arg1)
		make_operand(q->arg1, t->arg1);
	else
		t->arg1 = NULL;
	emit_instr(t);

	symbol* f = peekFunc();
	f->returnList = append(f->returnList, nextinstructionlabel());

	instruction* t1 = createInstruction();
	t1->srcLine = q->line;
	t1->opcode = jump_v;
	t1->arg1 = NULL; // reset_operand;
	t1->arg2 = NULL; // reset_operand;
	t1->result->type = label_a;
	emit_instr(t1);
}

void generate_FUNCEND(quad* q){

	symbol* f = popFunc();
	backpatch(f->returnList, nextinstructionlabel());
	q->taddress = nextinstructionlabel();
	printf("q->taddress = %d\n", q->taddress);

	instruction* t = createInstruction();
	t->srcLine = q->line;
	t->opcode = funcexit_v;
	t->arg1 = t->arg2 = NULL;
	make_operand(q->result, t->result);
	emit_instr(t);	
}

void backpatch(returnlist* list, int label){
	printf("Entered backpatch\n");
	printf("list->label = %d\n", list->label);
	returnlist *tmp = list;
	while(tmp != NULL){
		instructions[tmp->label].result->val = label;
		tmp = tmp->next;
	}
}

returnlist* append(returnlist *list, int label){
	printf("Enterd append\n");
	returnlist* newNode = (returnlist*) malloc(sizeof(returnlist));
	newNode->label = label;
	newNode->next = list;
	list = newNode;
	printf("Exiting append\n");
	return list;
}

void exec_generate(void){

	for(unsigned i = 0; i < currQuad; ++i){
		printf("---------------------------------EXEC GENERATE---------------------------------");
		printf("\nmake generatee with op  = %d, curr quad : %d\n", quads[i].op, currProcessedQuad+1);
		(*generators[quads[i].op])(quads+i);
		currProcessedQuad++;
	}
	patch_incomplete_jumps();
}

void add_incomplete_jump(unsigned instrNo, unsigned iaddress){

	incomplete_jump* tmp = ij_head;
	incomplete_jump* newNode = (incomplete_jump*) malloc(sizeof(incomplete_jump));

	newNode->instrNo = instrNo;
	newNode->iaddress = iaddress;
	newNode->next = NULL;

	if(ij_head == NULL){
		ij_head = newNode;
	}
	else{
		while(tmp->next != NULL)
			tmp = tmp->next;
		tmp->next = newNode;
	}
}

void patch_incomplete_jumps(void){
	 
	incomplete_jump* tmp = ij_head;
	while (tmp != NULL){
		if (tmp->iaddress == currQuad) // Allaksa apo total se currQuad. Must see again!
			instructions[tmp->instrNo].result->val = currInstr; // Nomizw thelei currInstr anti gia totalINstr. Must see again!
		else
			instructions[tmp->instrNo].result->val = quads[tmp->iaddress].taddress; 
		tmp = tmp->next; 
	}
}

char *translateopcode_v(vmopcode opcode){
	char *name; 
	switch (opcode){
		case 0  : name = "assign"; break;
		case 1  : name = "add"; break;
		case 2  : name = "sub"; break;
		case 3  : name = "mul"; break;
		case 4  : name = "div"; break;
		case 5  : name = "mod"; break;
		case 6	: name = "uminus"; break;
		case 7  : name = "and"; break;
		case 8  : name = "or"; break;
		case 9  : name = "not"; break;
		case 10  : name = "jeq"; break;
		case 11 : name = "jne"; break;
		case 12 : name = "jle"; break;
		case 13 : name = "jge"; break;
		case 14 : name = "jlt"; break;
		case 15 : name = "jgt"; break;
		case 16 : name = "call"; break;
		case 17 : name = "pusharg"; break;
		case 18 : name = "funcenter"; break;
		case 19 : name = "funcexit"; break;
		case 20 : name = "newtable"; break;
		case 21 : name = "tablegetelem"; break;
		case 22 : name = "tablesetelem"; break;
		case 23 : name = "jump"; break;
		case 24 : name = "nop"; break;
	}
	return name;
}

void printInstructions() {

	fprintf(fp, "Instr#\t\topcode\t\t\tresult\t\targ1\t\targ2\t\tline\n");
	fprintf(fp, "--------------------------------------------------------------------\n");

	for(int i = 0; i < currInstr; i++){

		fprintf(fp, "%-6d\t\t", i);
		fprintf(fp, "%-6s\t\t\t", translateopcode_v(instructions[i].opcode));

		if(	instructions[i].result == NULL || instructions[i].result->type == -1 || instructions[i].result->val == -1)
			fprintf(fp, "%-6s\t\t", "");
		else 
			fprintf(fp, "%d|%-3d\t\t", instructions[i].result->type, instructions[i].result->val);
		if(instructions[i].arg1 == NULL || (instructions[i].arg1->type == -1 || instructions[i].arg1->val == -1))
			fprintf(fp, "%-6s\t\t", "");
		else 
			fprintf(fp, "%d|%-3d\t\t", instructions[i].arg1->type, instructions[i].arg1->val);
		
		if(instructions[i].arg2 == NULL || (instructions[i].arg2->type == -1 || instructions[i].arg2->val == -1))
			fprintf(fp, "%-6s\t\t", "");
		else 
			fprintf(fp, "%d|%-3d\t\t", instructions[i].arg2->type, instructions[i].arg2->val);

		fprintf(fp, "%-6d\n", instructions[i].srcLine);
	}
}