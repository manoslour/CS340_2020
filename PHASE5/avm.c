#include "avm.h"
#include <assert.h>
#include "readBinary.h"
#include "exec_arith.h"
#include "exec_funcs.h"
#include "exec_table.h"
#include "exec_assign.h"
#include "exec_equality.h"

avm_memcell stack[AVM_STACKSIZE]; 
avm_memcell	ax, bx, cx;
avm_memcell	retval;
unsigned top, topsp;

unsigned char executionFinished = 0 ;
unsigned pc = 0 ;
unsigned currLine = 0 ;
unsigned codeSize = 0 ;
instruction* code = (instruction*) 0 ;
unsigned totalActuals = 0 ;

//------------------------------------------
int currInst = 0;
unsigned magicNumber;
unsigned int totalStringconsts; 
unsigned int totalGlobals; 
unsigned int totalVars;
unsigned int totalUserFuncs;
unsigned int totalLibFuncs;

double* numConsts = (double*)0; 
unsigned currNumConst = 0;

char** stringConsts = (char**)0;
unsigned curStringConsts = 0;

char** namedLibFuncs = (char**)0; 
unsigned currLibFuncs = 0;

userfunc* userFuncs = (userfunc*)0;
unsigned currUserFunc = 0; 
//------------------------------------------

execute_func_t executeFuncs[] = {
	execute_assign,
	execute_add,
	execute_sub,
	execute_mul,
	execute_div,
	execute_mod,
	execute_uminus,
	execute_and,
	execute_or,
	execute_not,
	execute_jeq,
	execute_jne,
	execute_jle,
	execute_jge,
	execute_jlt,
	execute_jgt,
	execute_call,
	execute_pusharg,
	execute_funcenter,
	execute_funcexit,
	execute_newtable,
	execute_tablegetelem,
	execute_tablesetelem,
	//execute_nop
};

memclear_func_t memclearFuncs[] = {
	0, /* number */
	memclear_string,
	0, /* bool */
	memclear_table,
	0, /* userfunc */
	0, /* libfunc */
	0, /* nil */
	0 /* undef */
};

static void avm_initstack(void) {
	for (unsigned i = 0; i < AVM_STACKSIZE; i++) {
		AVM_WIPEOUT (stack[i]); 
		stack[i].type= undef_m;
	}
}

void avm_tableincrefcounter (avm_table* t) { 
	++t->refCounter; 
}

void avm_tabledecrefcounter (avm_table* t) {
	assert(t->refCounter > 0);
	if (!--t->refCounter)	
        avmtabledestroy(t);
}

void avm_tablebucketsinit (avm_table_bucket** p) {
	for (unsigned i = 0; i < AVM_TABLE_HASHSIZE; ++i) {
		p[i] = (avm_table_bucket*) 0;
	}
}

void avm_tablebucketsdestroy (avm_table_bucket** p) {
	for (unsigned i = 0; i < AVM_TABLE_HASHSIZE; ++i , ++p) {
		for (avm_table_bucket* b = *p; b;){
			avm_table_bucket* del = b;
			b = b->next;
			avm_memcellclear(&del->key); //& should leave?
			avm_memcellclear(&del->value); //& should leave?
			free (del);
		}
		p[i] = (avm_table_bucket*) 0;
	}
}

void avmtabledestroy (avm_table* t) {
	avm_tablebucketsdestroy (t->strIndexed);
	avm_tablebucketsdestroy (t->numIndexed);
	free(t);
}

avm_table* avm_tablenew (void) {
	avm_table* t = (avm_table*) malloc(sizeof(avm_table));
	AVM_WIPEOUT(*t);

	t->refCounter = t->total = 0;
	avm_tablebucketsinit (t->numIndexed);
	avm_tablebucketsinit (t->strIndexed);

	return t;
}

avm_memcell* avm_translate_operand (vmarg* arg, avm_memcell* reg){
	printf("Entered avm translate_operand\n");
	switch (arg->type){
		case global_a:	return &stack[AVM_STACKSIZE - 1 - arg->val];
		case local_a:	return &stack[topsp - arg->val];
		case formal_a:	return &stack[topsp + AVM_STACKENV_SIZE + 1 + arg->val];
		case retval_a:	return &retval;
        // Add assign_a?
		case number_a:	{
			printf("Making a number_a memcell\n");
			reg->type = number_m;
			reg->data.numVal = consts_getnumber(arg->val);
			return reg;
		}
		case string_a:	{
			printf("Making a string_a memcell\n");
			reg->type = string_m;
			reg->data.strVal = strdup(consts_getstring(arg->val));
			return reg;
		}
		case bool_a:   {
			printf("Making a bool_a memcell\n");
			reg->type = bool_m;
			reg->data.boolVal = arg->val;
			return reg;
		}
		case nil_a:	{
			printf("Making a nil_a memcell\n");
			reg->type = nil_m;
			return reg;
		}
		case userfunc_a:{
			printf("Making a userfunc_a memcell\n");
			reg->type = userfunc_m;
			reg->data.funcVal = arg->val;
			return reg;
		}
		case libfunc_a:		{
			printf("Making a libfunc memcell\n");
			reg->type = libfunc_m;
			reg->data.libfuncVal = libfuncs_getused(arg->val);
			return reg;
		}
		default: assert(0);
	}
}

void execute_cycle (void){
	printf("Entered execute_cycle\n");
	if (executionFinished)
		return ;
	else if (pc == AVM_ENDING_PC) {
			executionFinished = 1;
			return ;
		}
	else{
		printf("entered else\n");
		assert(pc < AVM_ENDING_PC);
		instruction* instr = code + pc;
		assert (
				instr->opcode >= 0 &&
				instr->opcode <= AVM_MAX_INSTRUCTIONS
		);
		if (instr->srcLine)	
            currLine = instr->srcLine;

		printf("instr->opcode = %d\n", instr->opcode);
		unsigned oldPC = pc ;
		(*executeFuncs[instr->opcode])(instr);
		
        if (pc == oldPC)	
            ++pc ;
	}
}

void avm_memcellclear(avm_memcell* m) {
	if (m->type != undef_m) {
		memclear_func_t f = memclearFuncs[m->type];
		if (f) 
            (*f)(m);
		m->type = undef_m;
	}
}

void memclear_string (avm_memcell* m) {
	assert(m->data.strVal);
	free(m->data.strVal);
}

void memclear_table (avm_memcell* m) {
	assert(m->data.tableVal);
	avm_tabledecrefcounter(m->data.tableVal);
}

double	consts_getnumber (unsigned index){return numConsts[index];}
char*	consts_getstring (unsigned index){return stringConsts[index];}
char*	libfuncs_getused (unsigned index){return namedLibFuncs[index];}

void execute_uminus(instruction* instr){}
void execute_and(instruction* instr){}
void execute_or(instruction* instr){}
void execute_not(instruction* instr){}

void printStack(){
	for(int i = AVM_STACKSIZE; i >= top; i--){
		printf("Memcell: %d->",i);
		switch (stack[i].type)
		{
		case number_m:
			printf("numval: %f", stack[i].data.numVal);
			break;
		case string_m: 
			printf("strval: %s", stack[i].data.strVal);
			break;
		case bool_m:
			printf("boolval: %c", stack[i].data.boolVal);
			break;
		case table_m:
			printf("%d", stack[i].data.tableVal->refCounter);
			break;
		case userfunc_m:
			printf("%d", stack[i].data.funcVal);
			break;
		case libfunc_m:
			printf("libfuncval: %s", stack[i].data.libfuncVal);
			break;
		case nil_m:
			printf("nill case");
			break;
		case undef_m:
			printf("emty mem_Cell");
			break;
	
		default: assert(0);

		}
		printf("\n");
	}
}

int main(){
	readBinary();
	avm_initstack();
	top = topsp = AVM_STACKSIZE - 1 - totalGlobals;
	codeSize = currInst;
	printf("top = %d\n", top);
	while(!executionFinished){
		execute_cycle();
	}

	return 0;
}

