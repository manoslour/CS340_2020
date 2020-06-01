#include "avm.h"
#include <assert.h>

unsigned char	executionFinished = 0 ;
unsigned 		pc = 0 ;
unsigned 		currLine = 0 ;
unsigned		codeSize = 0 ;
unsigned 		code = (instruction * ) 0 ;
unsigned		totalActuals = 0 ;


#define AVM_ENDING_PC codeSize
#define AVM_NUMACTUALS_OFFSET	+4
#define AVM_SAVEDPC_OFFSET		+3
#define	AVM_SAVEDTOP_OFFSET		+2
#define	AVM_SAVEDTOPSP_OFFSET	+1


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

	assert(t->refCounter);
	if (!--t->refCounter > 0)	avmtabledestroy(t);

}

avm_tablebucketsinit (avm_table_bucket** p) {

	for (unsigned i = 0; i < AVM_TABLE_HASHSIZE; ++i) {
		p[i] = (avm_table_bucket*) 0 ;
	}
}

void avm_tablebucketdestroy (avm_table_bucket** p) {

	for (unsigned i = 0; i < AVM_TABLE_HASHSIZE; ++i , ++p) {

		for (avm_table_bucket* b = *p; b;){
			avm_table_bucket* del = b;
			b = b->next;
			avm_memcellclear (&del->key);
			avm_memcellclear (&del->value);
			free (del);
		}
		
		p[i] = (avm_table_bucket*) 0;
	}
}

avm_table* avm_tablenew (void) {

	avm_table* t = (avm_table*) malloc (sizeof(avm_table));
	AVM_WIPEOUT(*t);

	t->refCounter = t->total = 0;
	avm_tablebucketsinit (t->numIndexed);
	avm_tablebucketsinit (t->strIndexed);

	return t;
}

void avmtabledestroy (avm_table* t) {

	avm_tablebucketdestroy (t->strIndexed);
	avm_tablebucketdestroy (t->numIndexed);
	free(t);
}
avm_memcell* avm_translate_operand (vmarg * arg, avm_memcell *reg){
	switch (arg->type){

		case global_a:	return &stack[AVM_STACKSIZE - 1 - arg->val];

		case local_a:	return &stack[topsp - arg->val];

		case formal_a:	return &stack[topsp + AVM_STACKENV_SIZE + 1 + arg->val];

		case retval_a:	return &retval;

		case number_a:	{

			reg->type = number_m;
			reg->data.numVal = consts_getnumber(arg->val);
			return reg;
		}

		case string_a:	{

			reg->type = string_m;
			reg->data.strVal = strdup (consts_getstring(arg->val));
			return reg;
		}

		case bool_a:	{

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

		default: 	assert(0);


	}

}


void execute_cycle (void){

	if (executionFinished)
		return ;

	else if (pc == AVM_ENDING_PC) {
			executionFinished = 1;
			return ;
		}

	else	{

		assert(pc < AVM_ENDING_PC);
		instruction* instr = code + pc;
		assert (
				instr->opcode >= 0 &&
				instr->opcode <= AVM_MAX_INSTRUCTIONS
		);
		if ( instr->srcLine)	currLine = instr->srcLine;

		unsigned oldPC = pc ;
		( *executeFuncs[instr->opcode])(instr);

		if (pc == oldPC)	++pc ;
	}

}

void avm_memcellclear	(avm_memcell* m) {
	if (m->type != undef_m) {
		memclear_func_t f = memclearFuncs [m->type];
		if (f) (*f) (m);
		m->type = undef_m;
	}
}

extern void memclear_string (avm_memcell* m) {

	assert (m->data.strVal);
	free (m->data.strVal);
}

extern void memclear_table (avm_memcell* m) {

	assert (m->data.tableVal);
	avm_tabledecrefcounter (m->data.tableVal);
}

extern void avm_warning( char* format){

}

extern void avm_assign (avm_memcell* lv, avm_memcell* rv){

	if ( lv == rv )	return;

	if ( lv->type == table_m && rv->type == table_m && lv->data.tableVal == rv->data.tableVal )	return;

	if ( rv->type == undef_m )	avm_warning("assigning from 'undef' content! ");
	avm_memcellclear(lv);

	memcpy ( lv, rv, sizeof(avm_memcell));


	if (lv->type == string_m)

		lv->data.strVal = strdup(rv->data.strVal);

	else if (lv->type == table_m)

		avm_tableincrefcounter(lv->data.tableVal);

	}

void execute_assign (instruction* instr){

		avm_memcell* lv = avm_translate_operand (&instr->result, (avm_memcell*) 0);
		avm_memcell* rv = avm_translate_operand (&instr->arg1, &ax);

		assert (lv && (&stack[N-1] >= lv && lv > &stack[top] || lv == &retval ));
		assert (rv);

		avm_assign(lv, rv);
}

extern void avm_error (char* format){

}


void avm_dec_top (void){

		if (!top) { /* Stack overflow */

			avm_error("Stack overflow");
			executionFinished = 1;
		}
		else {

			--top;

		}

}

void avm_push_envvalue (unsigned val){

	stack[top].type = number_m;
	stack[top].data.numVal = val;
	avm_dec_top();

}

extern void avm_callsaveenvironment (void){

	avm_push_envvalue (totalActuals);
	avm_push_envvalue (pc+1);
	avm_push_envvalue (top + totalActuals + 2);
	avm_push_envvalue (topsp);



}

void execute_call (instruction *instr){

	avm_memcell *func = avm_translate_operand (&instr->result, &ax);
	assert (func);
	avm_callsaveenvironment();

	switch (func->type) {

		case userfunc_m:	{

			pc= func->data.funcVal;
			assert (pc < AVM_ENDING_PC);
			assert (code[pc].opcode == funcenter_v);
			break;
		}

		case string_m:	{

			avm_calllibfunc (func->data.strVal);
			break;
		}

		case libfunc_m:	{

			avm_calllibfunc (func->data.libfuncVal);
			break;
		}

		default:	{

			char* s = avm_tostring(func);
			avm_error ("call: cannot bind '%s' to function! ", s);
			free(s);
			executionFinished = 1;
		}

	}

}


void execute_funcenter (instruction* instr) {

	avm_memcell* func = avm_translate_operand (&instr->result, &ax);
	assert (func);
	assert (pc == func->data.funcVal);	/*func address should match pc*/

	/*callee actions*/

	totalActuals = 0;
	userfunc* funcInfo = avm_getfuncinfo (pc);
	topsp = top;
	top = top - funcInfo->localSize;
}

unsigned avm_get_envvalue (unsigned i) {

	assert (stack[i].type = number_m);
	unsigned val = (unsigned) stack[i].data.numVal;
	assert (stack[i].data.numVal == ((double) val ));
	return val;
}

void execute_funcexit (instruction* unused) {

	unsigned oldTop = top;
	top = avm_get_envvalue (topsp + AVM_SAVEDTOP_OFFSET);
	pc = avm_get_envvalue (topsp + AVM_SAVEDPC_OFFSET);
	topsp = avm_get_envvalue (topsp + AVM_SAVEDTOPSP_OFFSET);

	while ( ++oldTop <= top)  /* Intentionally ignoring first */

		avm_memcellclear(&stack[oldTop]);

}

void avm_calllibfunc (char* id)	{

	library_func_t f = avm_getlibraryfunc (id);
	if (!f) {
		avm_error("unsupported lib func '%s' called! ", id);
		executionFinished = 1;
	}
	else {
		/*enter and exit function are called manually*/
		topsp = top ; /* Enter function sequence. No stack locals*/
		totalActuals = 0;
		(*f)();  /* call library function */
		if (!executionFinished) /*an error may naturally occur inside*/
			execute_funcexit ((instruction*) 0); /*return sequence*/
	}
}

unsigned avm_totalactuals (void)	{

	return avm_get_envvalue (topsp + AVM_NUMACTUALS_OFFSET);
}

avm_memcell* avm_getactual (unsigned i)	{

	assert (i < avm_totalactuals());
	return &stack[topsp + AVM_STACKENV_SIZE + 1+ i];
}

/* implementation of the lib function print
 it displays every argument at the console */

void libfunc_print (void)	{

	unsigned n = avm_totalactuals();
	for (unsigned i = 0 ; i < n ; ++i ) {
		char* s = avm_getactual(i);
		puts (s);
		free (s);
	}
}

void execute_pusharg (instruction* instr)	{

	avm_memcell* arg = avm_translate_operand (&instr->arg1, &ax);
	avm_assign (&stack[top], arg); /* stack[top] = arg */
	++totalActuals;
	avm_dec_top();
}

char* avm_tostring (avm_memcell* m) {

	assert (m->type >= 0 && m->type == undef_m ); /* <= ??? sl 24/36 - lect 15 */
	return (*tostringFuncs[m->type]) (m);
}

double add_impl (double x, double y)	{
	return x+y;
}
double sub_impl (double x, double y)	{
	return x-y;
}
double mul_impl (double x, double y)	{
	return x*y;
}
double div_impl (double x, double y)	{ /* error check ? */
	return x/y;
}
double mod_impl (double x, double y)	{ /* error check ? */
	return ((unsigned) x) % ((unsigned) y);
}

void execute_arithmetic (instruction* instr) {

	avm_memcell* lv = avm_translate_operand (&instr->result, (avm_memcell*) 0);
	avm_memcell* rv1 = avm_translate_operand (&instr->arg1, &ax);
	avm_memcell* rv2 = avm_translate_operand (&instr->arg2, &bx);
	assert (lv && (&stack[N-1] >= lv > &stack[top] || lv==&retval));
	assert (rv1 && rv2);

	if (rv1->type != number_m || rv2->type != number_m) {
		avm_error("not a number in arithmetic!");
		executionFinished = 1;
	}
	else {
		arithmetic_func_t op = arithmeticFuncs[instr->opcode - add_v];
		avm_memcellclear(lv);
		lv->type = number_m;
		lv->data.numVal = (*op) (rv1->data.numVal, rv2->data.numVal);
		}
}

unsigned char number_tobool (avm_memcell* m)	{
	return m->data.numVal != 0;
}
unsigned char string_tobool (avm_memcell* m)	{
		return m->data.strVal[0] != 0;
}
unsigned char bool_tobool (avm_memcell* m)	{
	return m->data.boolVal;
}
unsigned char table_tobool (avm_memcell* m)	{
	return 1;
}
unsigned char userfunc_tobool (avm_memcell* m)	{
	return 1;
}
unsigned char libfunc_tobool (avm_memcell* m)	{

}
unsigned char nil_tobool (avm_memcell* m)	{
	return 0;
}
unsigned char undef_tobool (avm_memcell* m)	{
	assert (0);
	return 0;
}

unsigned char avm_tobool (avm_memcell* m) {

	assert (m->type >= 0 && m->type < undef_m	);
	return (*toboolFuncs[m->type]) (m);
}

void execute_jeq (instruction* instr) {

	assert (instr->result.type == label_a);
	avm_memcell* rv1 = avm_translate_operand(&instr->arg1, &ax);
	avm_memcell* rv2 = avm_translate_operand(&instr->arg2, &bx);

	unsigned char result = 0;

	if (rv1->type == undef_m || rv2->type == undef_m)
		avm_error(" undef involved in equality!");
	else if (rv1->type== nil_m || rv2->type == nil_m)
		result = rv1->type == nil_m && rv2->type == nil_m;
	else if (rv1->type== nil_m || rv2->type == nil_m)
		result = (avm_tobool (rv1) == avm_tobool (rv2));
	else if (rv1->type != rv2->type) {
		avm_error(" %s == %s is illegal!", typeStrings[rv1->type],typeStrings[rv2->type]);
	}
	else {

			/*  Equality check with dispatching */
	}

	if(!executionFinished && result)
		pc = instr->result.val;

}

void libfunc_typeof (void) {
	unsigned n = avm_totalactuals();
	if (n != 1)
		avm_error ("one argument (not %d) expected in 'typeof'", n);
	else {
		/* That's how a libfunc returns a result
			It has to only set the 'retval' register
		 */
		avm_memcellclear (&retval);
		retval.type = string_m;
		retval.data.strVal = strdup (typeStrings[avm_getactual(0)->type]);
	}

}

void execute_newtable (instruction* instr)	{

	avm_memcell* lv = avm_translate_operand (&instr->result, (avm_memcell*) 0);
	assert (lv && (&stack[N-1] >= lv && lv > &stack[top] || lv == &retval));

	avm_memcellclear (lv);

	lv->type = table_m;
	lv->data.tableVal = avm_table_new ();
	avm_tableincrefcounter (lv->data.tableVal);
}

void execute_tablegetelem (instruction* instr)	{

	avm_memcell* lv = avm_translate_operand (&instr->result, (avm_memcell*) 0);
	avm_memcell* t = avm_translate_operand (&instr->arg1, (avm_memcell*) 0);
	avm_memcell *i = avm_translate_operand (&instr->arg2, &ax);

	assert (lv && (&stack[N-1] >=lv && lv > &stack[top] || lv == &retval ));
	assert (t && (&stack[N-1] >= t && t > &stack[top] ));

	avm_memcellclear (lv);
	lv->type = nil_m; /* Default value */

	if (t->type != table_m)	{
		avm_error("illegal use of type %s as table!", typeStrings[t->type]);
	}
	else {
		avm_memcell* content = avm_tablegetelem(t->data.tableVal, i);
		if (content)
			avm_assign (lv, content);
		else {
			char* ts = avm_tostring (t);
			char* is = avm_tostring (i);
			avm_warning ("%s[%s] not found!", ts, is);
			free (ts);
			free (is);
		}
	}
}

void execute_tablesetelem (instruction* instr)	{

	avm_memcell* t = avm_translate_operand (&instr->result, (avm_memcell*) 0);
	avm_memcell* i = avm_translate_operand (&instr->arg1, &ax);
	avm_memcell *c = avm_translate_operand (&instr->arg2, &bx);

	assert (t && (&stack[N-1] >= t && t > &stack[top]));
	assert (i && c);

	if (t->type != table_m)
		avm_error("illegal use of type %s as table", typeStrings[t->type]);
	else
		avm_tablesetelem(t->data.tableVal, i, c);
}

void avm_initialize (void)	{

	avm_initstack();
	avm_registerlibfunc ("print", libfunc_print);
	//avm_registerlibfunc ("input", libfunc_input);
	//avm_registerlibfunc ("objectmemberkeys", libfunc_objectmemberkeys);
	//avm_registerlibfunc ("objecttotalmembers", libfunc_objecttotalmembers);
	//avm_registerlibfunc ("objectcopy", libfunc_objectcopy);
	avm_registerlibfunc ("totalarguments", libfunc_totalarguments);
	//avm_registerlibfunc ("argument", libfunc_argument);
	avm_registerlibfunc ("typeof", libfunc_typeof);
	//avm_registerlibfunc ("strtonum", libfunc_strtonum);
	//avm_registerlibfunc ("sqrt", libfunc_sqrt);
	//avm_registerlibfunc ("cos", libfunc_cos);
	//avm_registerlibfunc ("sin", libfunc_sin);

}

void libfunc_totalarguments (void) {

	/* Get topsp of previouw activation record*/

	unsigned p_topsp = avm_get_envvalue (topsp + AVM_SAVEDTOPSP_OFFSET);
	avm_memcellclear (&retval);

	if (!p_topsp) { 	/* if 0, no previous activ record */
		avm_error("totalarguments called outside a function");
		retval.type = nil_m;
	}
	else {
		/* Extract the # of actual args for the previous activation record*/
		retval.type = number_m;
		retval.data.numVal = avm_get_envvalue (p_topsp + AVM_NUMACTUALS_OFFSET);
	}

}
