#include "target.h"
#include "symTable.h"

extern quad *quads;
incomplete_jump* ij_head = (incomplete_jump*) 0;
unsigned ij_total = 0;
unsigned consts_newstring (char* s); 
unsigned consts_newnumber (double n);
unsigned libfuncs_newused (char* s); 
unsigned userfuncs_newfunc (symbol* sym); 

//-------------------------------------------------

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
	if(quad->label <currprocessedquad())
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