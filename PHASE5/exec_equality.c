#include "exec_equality.h"

extern avm_memcell stack[AVM_STACKSIZE]; 
extern avm_memcell	ax, bx, cx;
extern avm_memcell	retval;
extern unsigned top, topsp;

extern unsigned char executionFinished;
extern unsigned pc;
extern char *typeStrings[];

tobool_func_t toboolFuncs[] = {
	number_tobool,
	string_tobool,
	bool_tobool,
	table_tobool,
	userfunc_tobool,
	libfunc_tobool,
	nil_tobool,
	undef_tobool
};

unsigned char number_tobool (avm_memcell* m)  { return m->data.numVal != 0; }
unsigned char string_tobool (avm_memcell* m)  { return m->data.strVal[0] != 0; }
unsigned char bool_tobool (avm_memcell* m)    { return m->data.boolVal; }
unsigned char table_tobool (avm_memcell* m)	  { return 1; }
unsigned char userfunc_tobool (avm_memcell* m){ return 1; }
unsigned char libfunc_tobool (avm_memcell* m) { return 1; }
unsigned char nil_tobool (avm_memcell* m)	  { return 0; }
unsigned char undef_tobool (avm_memcell* m)	  { assert(0); return 0; }

unsigned char avm_tobool (avm_memcell* m) {
	assert (m->type >= 0 && m->type < undef_m);
	return (*toboolFuncs[m->type]) (m);
}

void execute_jeq (instruction* instr) {

	assert(instr->result->type == label_a);
	avm_memcell* rv1 = avm_translate_operand(instr->arg1, &ax);
	avm_memcell* rv2 = avm_translate_operand(instr->arg2, &bx);

	unsigned char result = 0;

	if (rv1->type == undef_m || rv2->type == undef_m)
		printf("Error, undef involved in equality!"); //AVM_ERROR
	else if (rv1->type== nil_m || rv2->type == nil_m)
		result = rv1->type == nil_m && rv2->type == nil_m;
	else if (rv1->type== nil_m || rv2->type == nil_m)
		result = (avm_tobool (rv1) == avm_tobool (rv2));
	else if (rv1->type != rv2->type) {
		printf("Error, %s == %s is illegal!", typeStrings[rv1->type], typeStrings[rv2->type]); //AVM_ERROR
	}
	else {
			/*  Equality check with dispatching */
	}
	if(!executionFinished && result)
		pc = instr->result->val;
}

extern void execute_jne (instruction* instr){/* ADD CODE */}
extern void execute_jle (instruction* instr){/* ADD CODE */}
extern void execute_jge (instruction* instr){/* ADD CODE */}
extern void execute_jlt (instruction* instr){/* ADD CODE */}
extern void execute_jgt (instruction* instr){/* ADD CODE */}