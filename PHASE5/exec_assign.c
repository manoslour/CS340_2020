#include "exec_assign.h"

extern avm_memcell stack[AVM_STACKSIZE]; 
extern avm_memcell	ax;
extern avm_memcell	retval;
extern unsigned top;
extern int pc ;
void execute_assign (instruction* instr){
		avm_memcell* lv = avm_translate_operand (instr->result, (avm_memcell*) 0);
		avm_memcell* rv = avm_translate_operand (instr->arg1, &ax);

        assert (rv); // Should do similar assertions here
		assert(lv && (&stack[AVM_STACKSIZE-1] >= lv && lv > &stack[top] || lv == &retval));

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
        printf("Warning, assigning from 'undef' content!"); //AVM_WARNING

    // Clear old cell contents
	avm_memcellclear(lv);
	memcpy ( lv, rv, sizeof(avm_memcell));

    // Now take care of copied values of reference counting
	if (lv->type == string_m)
		lv->data.strVal = strdup(rv->data.strVal);
	else if (lv->type == table_m)
		avm_tableincrefcounter(lv->data.tableVal);
}