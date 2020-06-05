#include "exec_funcs.h"

extern avm_memcell stack[AVM_STACKSIZE]; 
extern avm_memcell	ax, bx, cx;
extern avm_memcell	retval;
extern unsigned top, topsp;
extern unsigned char executionFinished;
extern unsigned pc;
extern unsigned currLine;
extern unsigned codeSize;
extern instruction* code;
extern unsigned totalActuals;
extern void avm_assign (avm_memcell* lv, avm_memcell* rv);

extern unsigned currLibFuncs ;
extern char** namedLibFuncs; 
tostring_func_t tostringFuncs[] = {
	number_tostring,
	string_tostring,
	bool_tostring,
	table_tostring,
	userfunc_tostring,
	libfunc_tostring,
	nil_tostring,
	undef_tostring
};

typedef void (*library_func_t) (void);


char *typeStrings[] = {
	"number",
	"string",
	"bool",
	"table",
	"userfunc",
	"libfunc",
	"nil",
	"undef"
};

library_func_t exexuteLibFunc[] = {
	libfunc_print,
	//libfunc_input,
	//libfunc_objectmemberkeys,
	//libfunc_objecttotalmembers,
	//libfunc_objectcopy,
	libfunc_totalarguments,
	//libfunc_argument,
	libfunc_typeof,
	//libfunc_strtonum,
	//libfunc_sqrt,
	//libfunc_cos,
	//libfunc_sin	
};

char* number_tostring (avm_memcell* cell){
	assert(cell);
	assert(cell->type == number_m);
	char* buffer = (char*) malloc(50 * sizeof(char));
	sprintf(buffer, "%f", cell->data.numVal);
	return buffer;
}

char* string_tostring (avm_memcell* cell){
	assert(cell);
	assert(cell->type == string_m);
	char* buffer = (char*) malloc(100 * sizeof(char));
	buffer = strdup(cell->data.strVal);
	return buffer;	
}

char* bool_tostring (avm_memcell* cell){
	assert(cell);
	assert(cell->type = bool_m);
	int val = cell->data.boolVal;
	if(val == 0)
		return "TRUE";
	else
		return "FALSE";	
}

char* table_tostring (avm_memcell* cell){/*ADD CODE*/}

char* userfunc_tostring (avm_memcell* cell){
	assert(cell);
	assert(cell->type == userfunc_m);
	char* buffer = (char*) malloc(100 * sizeof(char));
	sprintf(buffer, "%d", cell->data.funcVal);
	return buffer;
}

char* libfunc_tostring (avm_memcell* cell){
	assert(cell);
	assert(cell->type == libfunc_m);
	char* buffer = strdup(cell->data.libfuncVal);
	return buffer;
}

char* nil_tostring (avm_memcell* cell){
	assert(cell);
	assert(cell->type == nil_m);
	return "nil";	
}
char* undef_tostring (avm_memcell* cell){
	assert(cell);
	assert(cell->type == undef_m);
	return "undef";	
}

void execute_call (instruction *instr){
	printf("\n-----Entered execute_call-----\n");
	avm_memcell *func = avm_translate_operand (instr->arg1, &ax);
	assert (func);
	printf("\nCalling avm_callsaveenviroment in execute_call\n");
	avm_callsaveenvironment();
	printf("\nFinished avm_callsavenviroment in execute_call\n");

	switch (func->type) {
		case userfunc_m:{
			pc = func->data.funcVal;
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
			printf("Error, call: cannot bind '%s' to function! ", s); //AVM_ERROR
			free(s);
			executionFinished = 1;
		}
	}
}

void avm_dec_top (void){
		if (!top) { /* Stack overflow */
			printf("Error, Stack overflow\n"); //AVM_ERROR
			executionFinished = 1;
		}
		else
		    --top;
}

void avm_push_envvalue (unsigned val){
	printf("Entered avm_push_envalue\n");
	stack[top].type = number_m;
	stack[top].data.numVal = val;
	avm_dec_top();
}

void avm_callsaveenvironment (void){
	printf("Entered avm_callsaveenviroment\n");
	avm_push_envvalue(totalActuals);
	avm_push_envvalue(pc+1);
	avm_push_envvalue(top + totalActuals + 2);
	avm_push_envvalue(topsp);
}

userfunc* avm_getfuncinfo (unsigned address){
    //needs implementation
}

void execute_funcenter (instruction* instr) {
	avm_memcell* func = avm_translate_operand(instr->result, &ax);
	assert(func);
	assert(pc == func->data.funcVal);	/*func address should match pc*/
	/*callee actions*/
	totalActuals = 0;
	userfunc* funcInfo = avm_getfuncinfo (pc);
	topsp = top;
	top = top - funcInfo->localSize;
}

unsigned avm_get_envvalue (unsigned i) {
	printf("Entered avm_get_envaluate\n");
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

	while (++oldTop <= top)  /* Intentionally ignoring first */
		avm_memcellclear(&stack[oldTop]);
}

library_func_t avm_getlibraryfunc(char* id){
    for(int i=0; i<currLibFuncs; i++){
		if(!strcmp(id, namedLibFuncs[i])) 
			return exexuteLibFunc[i];
	}
}

void avm_calllibfunc (char* id)	{
	printf("Entered avm_calllibfunc\n");
	library_func_t f = avm_getlibraryfunc(id);
	if (!f) {
		printf("Error, unsupported lib func '%s' called!\n", id);
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
	return &stack[topsp + AVM_STACKENV_SIZE + 1 + i];
}

/* implementation of the lib function print
 it displays every argument at the console */

void libfunc_print (void)	{
	printf("Entered print\n");
	unsigned n = avm_totalactuals();
	for (unsigned i = 0; i < n; ++i ) {
		char* s = avm_tostring(avm_getactual(i)); 
		puts (s);
		free (s);
	}
}

void execute_pusharg (instruction* instr){
	avm_memcell* arg = avm_translate_operand (instr->arg1, &ax);
    assert(arg);
	avm_assign(&stack[top], arg); /* stack[top] = arg */
	++totalActuals;
	avm_dec_top();
}

void libfunc_typeof (void) {
	printf("Entered typeof()\n");
	printStack();
	unsigned n = avm_totalactuals();
	if (n != 1)
		printf("Error, one argument (not %d) expected in 'typeof'\n", n); //AVM_ERROR
	else {
		/* That's how a libfunc returns a result
			It has to only set the 'retval' register
		*/
		//printf("to retval esai %d\n",&retval );
		avm_memcellclear(&retval);
		retval.type = string_m;
		retval.data.strVal = strdup(typeStrings[avm_getactual(0)->type]);
	}
}

void avm_registerlibfunc(char *id, library_func_t addr){
    //
}

void avm_initialize (void)	{
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
		printf("Error, totalarguments called outside a function\n"); //AVM_ERROR
		retval.type = nil_m;
	}
	else {
		/* Extract the # of actual args for the previous activation record*/
		retval.type = number_m;
		retval.data.numVal = avm_get_envvalue (p_topsp + AVM_NUMACTUALS_OFFSET);
	}
}

char* avm_tostring (avm_memcell* m) {
	assert (m->type >= 0 && m->type <= undef_m ); 
    return (*tostringFuncs[m->type]) (m);
}