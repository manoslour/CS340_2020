#include "stack.h"
#include "target.h"
#include "symTable.h"

extern quad *quads;
extern unsigned total;
extern unsigned currQuad;

unsigned ij_total = 0;
incomplete_jump* ij_head = (incomplete_jump*) 0;

instruction *instructions = (instruction*) 0;
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

extern funcStack *funcstack;

//-------------------------------------------------

unsigned consts_newnumber (double n){

	unsigned ret_index;

	if(numConsts == NULL)
		numConsts = (double*) malloc(sizeof(double*));
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
        stringConsts = (char**) malloc(sizeof(char*));
    else{
        char** new_array = (char**) realloc(stringConsts, (totalStringConsts + 1) * sizeof(char*));
        stringConsts = new_array;
    }

	ret_index = totalStringConsts;
    stringConsts[totalStringConsts++] = strdup(s);
    
	return ret_index;
}

unsigned libfuncs_newused (char* s){

	unsigned ret_index;

	if(namedLibfuncs == NULL)
		namedLibfuncs = (char**) malloc(sizeof(char*));
	else{
		char** new_array = (char**) realloc(namedLibfuncs, (totalNamedLibfuncs + 1) * sizeof(char*));
        namedLibfuncs = new_array;
	}

	ret_index = totalNamedLibfuncs;
	namedLibfuncs[totalNamedLibfuncs++] = strdup(s);
	
	return ret_index;
}

unsigned userfuncs_newfunc(symbol* sym){

	unsigned ret_index;

	if(userFuncs == NULL)
		userFuncs = (userfunc*) malloc(sizeof(userfunc*));
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

	assert(totalInstructions == currInstr);
	instruction* p = (instruction*) malloc(NEW_INSTR_SIZE);
	if(instructions){
		memcpy(p, instructions, CURR_INSTR_SIZE);
		free(instructions);
	}
	instructions = p;
	totalInstructions += EXPAND_INSTR_SIZE;
}

void emit_instr(instruction *t){

	if(currInstr == totalInstructions)
		expandInstr();

	instruction* i = instructions + currInstr++;
	i->opcode = t->opcode;
	i->result = t->result;
	i->arg1 = t->arg1;
	i->arg2 = t->arg2;
	i->srcLine = t->srcLine;
}

unsigned nextinstructionlabel(void){
	return currInstr;
}

unsigned currprocessedquad(){
	return currQuad;
}

//-------------------------------------------------

static void avm_initstack(void) {
    for(unsigned int i = 0; i < AVM_STACKSIZE; ++i){
        AVM_WIPEOUT(stack[i]);
        stack[i].type = undef_m;
    }
}

void avm_tableincrefcounter(avm_table* t){
    ++t->refCounter;
}

void avm_tabledecrefcounter(avm_table* t){
    assert(t->refCounter > 0);
    if(!--t->refCounter)
        avm_tabledestroy(t);
}

void avm_tablebucketsinit(avm_table_bucket** p){
    for(unsigned i = 0; i < AVM_TABLE_HASHSIZE; ++i)
        p[i] = (avm_table_bucket*) 0;
}

avm_table* avm_tablenew(void){
    avm_table* t = (avm_table*) malloc(sizeof(avm_table));
    AVM_WIPEOUT(*t);

    t->refCounter = t->total = 0;
    avm_tablebucketsinit(t->numIndexed);
    avm_tablebucketsinit(t->strIndexed);

    return t;
}

void avm_memcellclear(avm_memcell* m){
    //MUST SEE!!
}

void avm_tablebucketsdestroy(avm_table_bucket** p){
    for(unsigned i = 0; i < AVM_TABLE_HASHSIZE; ++i, ++p){
        for(avm_table_bucket* b = *p; b;){
            avm_table_bucket* del = b;
            b = b->next;
            avm_memcellclear(&del->key);
            avm_memcellclear(&del->value);
            free(del);
        }
        p[i] = (avm_table_bucket*) 0;
    }
}

void avm_tabledestroy(avm_table* t){
    avm_tablebucketsdestroy(t->strIndexed);
    avm_tablebucketsdestroy(t->numIndexed);
    free(t);
}

void make_operand (expr* e, vmarg* arg){
	
	switch (e->type){
		case var_e :
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
			arg->val = e->boolConst;
			arg->type = bool_a;
			break;
		}
		case conststring_e :{
			arg->val = consts_newstring(e->strConst); 
			arg->type = string_a; 
			break;
		}
		case constnum_e: {
			arg->val = consts_newnumber(e->numConst); 
			arg->type = number_a; 
			break;
		}
		case nil_e: {
			arg->type = nil_a; 
			break;
		}
		case programfunc_e : {
			arg->type = userfunc_a ; 
			//arg->val = e->sym->taddress;
			/*or alternatively*/
			arg->val = userfuncs_newfunc(e->sym);
			break; 
		} 
		case libraryfunc_e: {
			arg->type = libfunc_a; 
			arg->val = libfuncs_newused(strdup(e->sym->name));
			break;
		}

		default: assert(0);
	}
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
}

void generate (vmopcode op, quad* quad ){
	instruction* t = (instruction*) malloc(sizeof(instruction));  
	t->opcode = op;
	// MUST SEE IF IT WANTS POINTER(t->arg1) OR POINTER_ADDRESS(&t->arg1)
	make_operand(quad->arg1, t->arg1);
	make_operand(quad->arg2, t->arg2);
	make_operand(quad->result, t->result);
	quad->taddress = nextinstructionlabel();
	emit_instr(t);
}

void generate_relational(vmopcode op, quad* quad){
	instruction* t = (instruction*) malloc(sizeof(instruction)); 
	t->opcode = op;
	make_operand(quad->arg1, t->arg1);
	make_operand(quad->arg2, t->arg2);

	t->result->type = label_a;

	// MUST SEE IF nextquad() CALL IS CORRECT OR EVEN WORKS
	if(quad->label < currprocessedquad())
		t->result->val = quads[quad->label].taddress;
	else
		add_incomplete_jump(nextinstructionlabel(), quad->label);
	
	quad->taddress = nextinstructionlabel();
	emit_instr(t);
}

void generate_ADD(quad* q) { generate(add_v, q); }
void generate_SUB(quad* q) { generate(sub_v, q); }
void generate_MUL(quad* q) { generate(mul_v, q); }
void generate_DIV(quad* q) { generate(div_v, q); }
void generate_MOD(quad* q) { generate(mod_v, q); }

void genetate_NEWTABLE (quad* q) { generate(newtable_v, q); }
void genetate_TABLEGETELM (quad* q) { generate(tablegetelem_v, q); }
void genetate_TABLESETELM (quad* q) { generate(tablesetelem_v, q); }
void genetate_ASSIGN (quad* q) { generate(assign_v, q);}

void genetate_NOP (quad* q){
	instruction* t = (instruction*) malloc(sizeof(instruction));  
	t->opcode = nop_v;
	emit_instr(t);
}

void generate_JUMP(quad* q) { generate_relational(jump_v, q); }
void generate_IF_EQ(quad* q) { generate_relational(jeq_v, q); }
void generate_IF_NOTEQ(quad* q) { generate_relational(jne_v, q); }
void generate_GREATER(quad* q) { generate_relational(jgt_v, q); }
void generate_GREATEREQ(quad* q) { generate_relational(jge_v, q); }
void generate_LESS(quad* q) { generate_relational(jlt_v, q); }
void generate_IFLESSEQ(quad* q) { generate_relational(jle_v, q); }

void generate_NOT (quad* q){
	q->taddress = nextinstructionlabel();
	instruction* t = (instruction*) malloc(sizeof(instruction));  

	t->opcode = jeq_v; 
	make_operand(q->arg1, t->arg1);
	make_booloperand(t->arg2, false); 
	t->result->type = label_a;
	t->result->val = nextinstructionlabel() + 3;
	emit_instr(t);

	t->opcode = assign_v; 
	make_booloperand(t->arg1, false); 
	reset_operand(t->arg2); 
	make_operand(q->result, t->result); 
	emit_instr(t);

	t->opcode = jump_v; 
	reset_operand(t->arg1);
	reset_operand(t->arg2);
	t->result->type = label_a; 
	t->result->val = nextinstructionlabel() + 2; 
	emit_instr(t);

	t->opcode = assign_v; 
	make_booloperand(t->arg1, true); 
	reset_operand(t->arg2); 
	make_operand(q->result, t->result); 
	emit_instr(t);
}

void generate_OR (quad* q){
	q->taddress = nextinstructionlabel(); 
	instruction* t = (instruction*) malloc(sizeof(instruction));  

	t->opcode = jeq_v; 
	make_operand(q->arg1, t->arg1); 
	make_booloperand(t->arg2, true); 
	t->result->type = label_a;
	t->result->val = nextinstructionlabel() + 4;
	emit_instr(t);

	make_operand(q->arg2, t->arg1); 
	t->result->val = nextinstructionlabel() + 3;
	emit_instr(t); 

	t->opcode = assign_v; 
	make_booloperand(t->arg1, false); 
	reset_operand(t->arg2); 
	make_operand(q->result, t->result); 
	emit_instr(t); 

	t->opcode = jump_v; 
	reset_operand(t->arg1); 
	reset_operand(t->arg2); 
	t->result->type = label_a; 
	t->result->val = nextinstructionlabel() + 2; 
	emit_instr(t);

	t->opcode = assign_v; 
	make_booloperand(t->arg1, true); 
	reset_operand(t->arg2);
	make_operand(q->result, t->result); 
	emit_instr(t);
}

void generate_PARAM (quad* q){
	q->taddress = nextinstructionlabel(); 
	instruction* t = (instruction*) malloc(sizeof(instruction));
	t->opcode = pusharg_v; 
	make_operand(q->arg1, t->arg1); 
	emit_instr(t); 
}

void generate_CALL(quad* q){
	q->taddress = nextinstructionlabel();
	instruction* t = (instruction*) malloc(sizeof(instruction));
	t->opcode = call_v; 
	make_operand(q->arg1, t->arg1); 
	emit_instr(t);

}

void generate_GETRETVAL (quad* q){
	q->taddress = nextinstructionlabel(); 
	instruction* t = (instruction*) malloc(sizeof(instruction)); 
	t->opcode = assign_v; 
	make_operand(q->result, t->result);
	make_retvaloperand(t->arg1); 
	emit_instr(t); 
}

void generate_FUNCSTART(quad* q){
	symbol* f = q->result->sym;
	f->taddress = nextinstructionlabel();
	q->taddress = nextinstructionlabel();

	userfuncs_newfunc(f);
	pushFunc(funcstack, f); // MUST SEE AGAIN!!

	instruction* t = (instruction*) malloc(sizeof(instruction));
	t->opcode = funcenter_v;
	make_operand(q->result, t->result);
	emit_instr(t);
}

void exec_generate(void){
	for(unsigned i = 0; i < total; ++i)
		(*generators[quads[i].op])(quads+i);
}

void add_incomplete_jump(unsigned instNo, unsigned iaddress){

	incomplete_jump* tmp = ij_head;
	incomplete_jump* newNode = (incomplete_jump*) malloc(sizeof(incomplete_jump));

	newNode->instrNo = instNo;
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

void printInstructions () {
	int i, tmp; 
	char *arg1,*arg2,*result,*opcode; 

	for(i = 0; i<totalInstructions; i++){
		opcode = strdup(translateopcode_v((instructions + i)->opcode));
		printf("\n%d:\t%14s\t",i, opcode); 
		if((instructions+i)->result == NULL){
			printf("%11s\t", "");
		}
		else {

		}
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
		case 6  : name = "and"; break;
		case 7  : name = "or"; break;
		case 8  : name = "not"; break;
		case 9  : name = "jeq"; break;
		case 10 : name = "jne"; break;
		case 11 : name = "jle"; break;
		case 12 : name = "jge"; break;
		case 13 : name = "jlt"; break;
		case 14 : name = "jgt"; break;
		case 15 : name = "call"; break;
		case 16 : name = "pusharg"; break;
		case 17 : name = "funcenter"; break;
		case 18 : name = "funcexit"; break;
		case 19 : name = "newtable"; break;
		case 20 : name = "tablegetelem"; break;
		case 21 : name = "tablesetelem"; break;
		case 22 : name = "nop"; break;
		case 23 : name = "jump"; break;
	}
	return name;
}