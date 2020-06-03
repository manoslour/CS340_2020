#include "exec_assign.h"

extern avm_memcell stack[AVM_STACKSIZE]; 
extern avm_memcell	ax;
extern avm_memcell	retval;
extern unsigned top;

void execute_assign (instruction* instr){

		avm_memcell* lv = avm_translate_operand (instr->result, (avm_memcell*) 0);
		avm_memcell* rv = avm_translate_operand (instr->arg1, &ax);

		assert(lv && (&stack[AVM_STACKSIZE-1] >= lv && lv > &stack[top] || lv == &retval));
        assert (rv); // Should do similar assertions here

		avm_assign(lv, rv);
}

void avm_assign(avm_memcell* lv, avm_memcell* rv){
    // Same cells? destructive to assign!
	if (lv == rv)
        return;
    // Same tables? no need to assign
	if (lv->type == table_m &&
        rv->type == table_m &&
        lv->data.tableVal == rv->data.tableVal )
            return;
    // From undefined r-value? warn!
	if (rv->type == undef_m )
        avm_warning("assigning from 'undef' content! ");
    // Clear old cell contents
	avm_memcellclear(lv);

	memcpy ( lv, rv, sizeof(avm_memcell));
    // Now take care of copied values of reference counting
	if (lv->type == string_m)
		lv->data.strVal = strdup(rv->data.strVal);
	else if (lv->type == table_m)
		avm_tableincrefcounter(lv->data.tableVal);
}