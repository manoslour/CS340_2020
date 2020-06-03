#include "exec_table.h"

extern avm_memcell stack[AVM_STACKSIZE]; 
extern avm_memcell	ax, bx;
extern avm_memcell	retval;
extern unsigned top;
extern char* avm_tostring (avm_memcell*);
extern void avm_assign (avm_memcell* lv, avm_memcell* rv);

void execute_newtable (instruction* instr)	{
	avm_memcell* lv = avm_translate_operand (instr->result, (avm_memcell*) 0);
	assert (lv && (&stack[AVM_STACKSIZE-1] >= lv && lv > &stack[top] || lv == &retval));

	avm_memcellclear (lv);

	lv->type = table_m;
	lv->data.tableVal = avm_tablenew();
	avm_tableincrefcounter (lv->data.tableVal);
}

void execute_tablegetelem (instruction* instr)	{
    
	avm_memcell* lv = avm_translate_operand (instr->result, (avm_memcell*) 0);
	avm_memcell* t = avm_translate_operand (instr->arg1, (avm_memcell*) 0);
	avm_memcell *i = avm_translate_operand (instr->arg2, &ax);

	assert (lv && (&stack[AVM_STACKSIZE-1] >=lv && lv > &stack[top] || lv == &retval ));
	assert (t && (&stack[AVM_STACKSIZE-1] >= t && t > &stack[top] ));
    assert(i);

	avm_memcellclear(lv);
	lv->type = nil_m; /* Default value */

	if (t->type != table_m)	{
		//avm_error("illegal use of type %s as table!", typeStrings[t->type]);
	}
	else {
		avm_memcell* content = avm_tablegetelem(t->data.tableVal, i);
		if (content)
			avm_assign (lv, content);
		else {
			char* ts = avm_tostring (t);
			char* is = avm_tostring (i);
			//avm_warning ("%s[%s] not found!", ts, is);
			free (ts);
			free (is);
		}
	}
}

void execute_tablesetelem (instruction* instr)	{

	avm_memcell* t = avm_translate_operand (instr->result, (avm_memcell*) 0);
	avm_memcell* i = avm_translate_operand (instr->arg1, &ax);
	avm_memcell *c = avm_translate_operand (instr->arg2, &bx);

	assert(t && (&stack[AVM_STACKSIZE-1] >= t && t > &stack[top]));
	assert(i && c);

	if (t->type != table_m){
		//avm_error("illegal use of type %s as table", typeStrings[t->type]);
	}
	else
		avm_tablesetelem(t->data.tableVal, i, c);
}

avm_memcell* avm_tablegetelem (avm_table* table, avm_memcell* index){/* ADD CODE*/}
void avm_tablesetelem (avm_table* table, avm_memcell* index, avm_memcell* content){/* ADD CODE */}
