#include "avm.h"
#include <assert.h>
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

execute_func_t executeFuncs[] = {
	execute_assign,
	execute_add,
	execute_sub,
	execute_mul,
	execute_div,
	execute_mod,
	//execute_uminus,
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
	execute_nop
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
	avm_tablebucketdestroy (t->strIndexed);
	avm_tablebucketdestroy (t->numIndexed);
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
	switch (arg->type){
		case global_a:	return &stack[AVM_STACKSIZE - 1 - arg->val];
		case local_a:	return &stack[topsp - arg->val];
		case formal_a:	return &stack[topsp + AVM_STACKENV_SIZE + 1 + arg->val];
		case retval_a:	return &retval;
        // Add assign_a?
		case number_a:	{
			reg->type = number_m;
			reg->data.numVal = consts_getnumber(arg->val);
			return reg;
		}
		case string_a:	{
			reg->type = string_m;
			reg->data.strVal = strdup(consts_getstring(arg->val));
			return reg;
		}
		case bool_a:   {
			reg->type = bool_m;
			reg->data.boolVal = arg->val;
			return reg;
		}
		case nil_a:	{
			reg->type = nil_m;
			return reg;
		}
		case userfunc_a:	{
			reg->type = userfunc_m;
			reg->data.funcVal = arg->val;
			return reg;
		}
		case libfunc_a:		{
			reg->type = libfunc_m;
			reg->data.libfuncVal = libfuncs_getused(arg->val);
			return reg;
		}
		default: assert(0);
	}
}

void execute_cycle (void){
	if (executionFinished)
		return ;
	else if (pc == AVM_ENDING_PC) {
			executionFinished = 1;
			return ;
		}
	else{

		assert(pc < AVM_ENDING_PC);
		instruction* instr = code + pc;
		assert (
				instr->opcode >= 0 &&
				instr->opcode <= AVM_MAX_INSTRUCTIONS
		);
		if (instr->srcLine)	
            currLine = instr->srcLine;

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

void avm_warning( char* format){/*ADD CODE*/}
void avm_error (char* format, char* s){/*ADD CODE*/}