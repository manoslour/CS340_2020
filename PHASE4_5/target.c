#include "target.h"





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
			char *str = malloc(sizeof(e->sym->name + 1)); 
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
	for (i = 0; i<total; ++i )
		(*genetators[quads[i].op]) (quads+i) ;
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