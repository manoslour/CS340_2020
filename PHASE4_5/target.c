#include "target.h"
#include "symTable.h"



unsigned consts_newstring (char* s); 
unsigned consts_newnumber (double n);
unsigned libfuncs_newused (char* s); 
unsigned userfuncs_newfunc (symbol* sym); 



void make_operant (expr* e, vmarg* arg){
	
	switch (e->type){
		case var_e :
		case tableitem_e :
		case arithexpr_e :
		case boolexpr_e :
		case newtable_e :{

			assert(e->sym);
			arg->val = e->sym->offset; 

			switch (e->sym->offset){
				case programvar   : arg->type = global_a; break;
				case functionlocal: arg->type = local_a;  break;
				case formalarg	  : arg->type = formal_a; break;
				default : assert(0);

			}
			break; /* from case newtable_e */

		}
		/*CONSTANTS*/
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
			char *str ; 
			str = strdup(e->sym->name);

			arg->type = libfunc_a; 
			arg->val = libfuncs_newused(str);
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

void make_retvaloperand (vmarg *arg){ arg->type retval_a; }


void generate (void ){
	unsigned i = 0; 
	// pws tha paroume to total p einai sto symTable .c ? 
	for (i = 0; i<total; ++i )
		(*genetators[quads[i].op]) (quads+i) ;
}

void generate_ADD(quad q) {
	instruction t; 
	t->opcode = add_v; 
	make_operant(q->arg1, &t->arg1); 
	make_operant(q->arg2, &t->arg2);
	make_operant(q->result, &t->result); 

	quad->taddress = nextinstructionlabel(); // prepei na kanoume thn nextinstructionlabel();11!!!!!
	emit_t(t); // make emit }
}

void generate_SUB(quad q) {
	instruction t; 
	t->opcode = sub_v; 
	make_operant(q->arg1, &t->arg1); 
	make_operant(q->arg2, &t->arg2);
	make_operant(q->result, &t->result); 

	quad->taddress = nextinstructionlabel(); // prepei na kanoume thn nextinstructionlabel();11!!!!!
	emit_t(t); // make emit }
}

void generate_MUL(quad q) {
	instruction t; 
	t->opcode = mul_v; 
	make_operant(q->arg1, &t->arg1); 
	make_operant(q->arg2, &t->arg2);
	make_operant(q->result, &t->result); 

	quad->taddress = nextinstructionlabel(); // prepei na kanoume thn nextinstructionlabel();11!!!!!
	emit_t(t); // make emit }
}

void generate_DIV(quad q) {
	instruction t; 
	t->opcode = div_v; 
	make_operant(q->arg1, &t->arg1); 
	make_operant(q->arg2, &t->arg2);
	make_operant(q->result, &t->result); 

	quad->taddress = nextinstructionlabel(); // prepei na kanoume thn nextinstructionlabel();11!!!!!
	emit_t(t); // make emit }
}

void generate_MOD(quad q) {
	instruction t; 
	t->opcode = mod_v; 
	make_operant(q->arg1, &t->arg1); 
	make_operant(q->arg2, &t->arg2);
	make_operant(q->result, &t->result); 

	quad->taddress = nextinstructionlabel(); // prepei na kanoume thn nextinstructionlabel();11!!!!!
	emit_t(t); // make emit }
}

void genetate_NEWTABLE (quad q){
	instruction t; 
	t->opcode = newtable_v; 
	make_operant(q->arg1, &t->arg1); 
	make_operant(q->arg2, &t->arg2);
	make_operant(q->result, &t->result); 

	quad->taddress = nextinstructionlabel(); // prepei na kanoume thn nextinstructionlabel();11!!!!!
	emit_t(t); // make emit }
}

void genetate_TABLEGETELM (quad q){
	instruction t; 
	t->opcode = tablegetelem_v; 
	make_operant(q->arg1, &t->arg1); 
	make_operant(q->arg2, &t->arg2);
	make_operant(q->result, &t->result); 

	quad->taddress = nextinstructionlabel(); // prepei na kanoume thn nextinstructionlabel();11!!!!!
	emit_t(t); // make emit }
}

void genetate_TABLESETELM (quad q){
	instruction t; 
	t->opcode = tablesetelem_v; 
	make_operant(q->arg1, &t->arg1); 
	make_operant(q->arg2, &t->arg2);
	make_operant(q->result, &t->result); 

	quad->taddress = nextinstructionlabel(); // prepei na kanoume thn nextinstructionlabel();11!!!!!
	emit_t(t); // make emit }
}

void genetate_ASSIGN (quad q){
	instruction t; 
	t->opcode = assign_v; 
	make_operant(q->arg1, &t->arg1); 
	make_operant(q->arg2, &t->arg2);
	make_operant(q->result, &t->result); 

	quad->taddress = nextinstructionlabel(); // prepei na kanoume thn nextinstructionlabel();11!!!!!
	emit_t(t); // make emit }
}

void genetate_NOP (quad q){
	instruction t; 
	t->opcode = nop_v; 
	make_operant(q->arg1, &t->arg1); 
	make_operant(q->arg2, &t->arg2);
	make_operant(q->result, &t->result); 

	quad->taddress = nextinstructionlabel(); // prepei na kanoume thn nextinstructionlabel();11!!!!!
	emit_t(t); // make emit }
}

void generate_JUMP(quad q){
	instruction t; 
	t->opcode = jump_v; 
	make_operant(q->arg1, &t->arg1); 
	make_operant(q->arg2, &t->arg2);
	
	t->result->type = label_a; 

	if (q->label < currprocessedquad())
		t->result->value = quads[quad.label].taddress; 
	else 
		add_incomplete_jump(nextinstructionlabel(), quad.label);
	emit(t);
}
void generate_IF_EQ(quad q){
	instruction t; 
	t->opcode = jeq_v; 
	make_operant(q->arg1, &t->arg1); 
	make_operant(q->arg2, &t->arg2);
	
	t->result->type = label_a; 

	if (q->label < currprocessedquad())
		t->result->value = quads[quad.label].taddress; 
	else 
		add_incomplete_jump(nextinstructionlabel(), quad.label);
	emit(t);
}
void generate_IF_NOTEQ(quad q){
	instruction t; 
	t->opcode = jne_v; 
	make_operant(q->arg1, &t->arg1); 
	make_operant(q->arg2, &t->arg2);
	
	t->result->type = label_a; 

	if (q->label < currprocessedquad())
		t->result->value = quads[quad.label].taddress; 
	else 
		add_incomplete_jump(nextinstructionlabel(), quad.label);
	emit(t);
}
void generate_GREATER(quad q){
	instruction t; 
	t->opcode = jgt_v; 
	make_operant(q->arg1, &t->arg1); 
	make_operant(q->arg2, &t->arg2);
	
	t->result->type = label_a; 

	if (q->label < currprocessedquad())
		t->result->value = quads[quad.label].taddress; 
	else 
		add_incomplete_jump(nextinstructionlabel(), quad.label);
	emit(t);
}
void generate_GREATEREQ(quad q){
	instruction t; 
	t->opcode = jge_v; 
	make_operant(q->arg1, &t->arg1); 
	make_operant(q->arg2, &t->arg2);
	
	t->result->type = label_a; 

	if (q->label < currprocessedquad())
		t->result->value = quads[quad.label].taddress; 
	else 
		add_incomplete_jump(nextinstructionlabel(), quad.label);
	emit(t);
}
void generate_LESS(quad q){
	instruction t; 
	t->opcode = jlt_v; 
	make_operant(q->arg1, &t->arg1); 
	make_operant(q->arg2, &t->arg2);
	
	t->result->type = label_a; 

	if (q->label < currprocessedquad())
		t->result->value = quads[quad.label].taddress; 
	else 
		add_incomplete_jump(nextinstructionlabel(), quad.label);
	emit(t);
}
void generate_IFLESSEQ(quad q){
	instruction t; 
	t->opcode = jle_v; 
	make_operant(q->arg1, &t->arg1); 
	make_operant(q->arg2, &t->arg2);
	
	t->result->type = label_a; 

	if (q->label < currprocessedquad())
		t->result->value = quads[quad.label].taddress; 
	else 
		add_incomplete_jump(nextinstructionlabel(), quad.label);
	emit(t);
}

void generate_NOT (quad q){
	q.taddress = nextinstructionlabel();
	instruction *t; 

	t->opcode = jeq_v; 
	make_operant(q->arg1, &t->arg1);
	make_booloperand(&t->arg2, false ); 
	t->result->type = label_a;
	t->result->value = nextinstructionlabel() + 3;

	emit(t);

	t->opcode = assign_v; 
	make_booloperand(&t->arg1, false ); 
	reset_operand(&t->arg2); 
	make_operant(q->result, &t->result); 
	emit(t);

	t->opcode = jump_v; 
	reset_operand(&t->arg1);
	reset_operand(&t->arg2);
	t->result->type = label_a; 
	t->result->value = nextinstructionlabel() + 2; 
	emit(t); 

	t->opcode = assign_v; 
	make_booloperand(&t->arg1, true); 
	reset_operand(&t->arg2); 
	make_operant(q->result, &t->result); 
	emit(t);
}

void generate_OR (quad q){
	q->taddress = nextinstructionlabel(); 
	instruction *t; 

	t->opcode = jeq_v; 
	make_operant(q->arg1, &t->arg1); 
	make_booloperand(&t->arg2, true ); 
	t->result->type = label_a; 
	t->result->value = nextinstructionlabel() + 4;
	emit(t); 

	make_operant(quad->arg2, &t->arg1); 
	t->result->value = nextinstructionlabel() + 3;
	emit(t); 

	t->opcode = assign_v; 
	make_booloperand(&t->arg1, false ); 
	reset_operand(&t->arg2); 
	make_operant(q->result, &t->result); 
	emit(t); 

	t->opcode = jump_v; 
	reset_operand(&t->arg1); 
	reset_operand(&t->arg2); 
	t->result->type = label_a; 
	t->result->value = nextinstructionlabel() + 2; 
	emit(t); 

	t.opcode = assign_v; 
	make_booloperand(&t->arg1, true); 
	reset_operand(&t->arg2);
	make_operant(q->result, &t->result); 
	emit(t);

}

void generate_PARAM (quad q){
	q->taddress = nextinstructionlabel(); 
	instruction *t;
	t->opcode = pusharg_v; 
	make_operant(q->arg1, &t->arg1); 
	emit(t); 
}

void generate_CALL(quad q){
	q->taddress = nextinstructionlabel();
	instruction *t;
	t->opcode = call_v; 
	make_operant(q->arg1, &t->arg1); 
	emit(t);

}

void generate_GETRETVAL (quad q){
	q->taddress = nextinstructionlabel(); 
	instruction *t; 
	t->opcode = assign_v; 
	make_operant(q->result, &t->result);
	make_retvaloperand(&t->arg1); 
	emit(t); 
}



















































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