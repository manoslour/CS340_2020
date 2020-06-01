#include <stdio.h>
#include <unistd.h>
#include "target.h"

#define AVM_STACKSIZE	4096
#define AVM_WIPEOUT(m) memset(&(m), 0, sizeof(m))	
#define AVM_STACKENV_SIZE	4
#define	AVM_MAX_INSTRUCTIONS (unsigned)	nop_v
#define AVM_TABLE_HASHSIZE	211
#define execute_add execute_arithmetic
#define execute_sub execute_arithmetic
#define execute_mul execute_arithmetic
#define execute_div execute_arithmetic
#define execute_mod execute_arithmetic





typedef enum avm_mem_cell_t	{
	number_m,
	string_m,
	bool_m,
	table_m,
	userfunc_m,
	libfunc_m,
	nil_m,
	undef_m
} avm_mem_cell_t;

typedef struct avm_table avm_table;
typedef struct avm_memcell	{

	avm_mem_cell_t type;
	union {
		double numVal;
		char* strVal;
		unsigned char boolVal;
		avm_table* tableVal;
		unsigned funcVal;
		char* libfuncVal;
	}data;
} avm_memcell;

typedef struct avm_table_bucket {
	avm_memcell key;
	avm_memcell value;
	avm_table_bucket* next;

} avm_table_bucket;

struct avm_table {
	unsigned refCounter;
	avm_table_bucket* strIndexed[AVM_TABLE_HASHSIZE];
	avm_table_bucket* numIndexed[AVM_TABLE_HASHSIZE];
	unsigned total;
};



avm_table* avm_tablenew (void);

void avmtabledestroy (avm_table* t);

void avm_tableincrefcounter (avm_table* t);

void avm_tabledecrefcounter (avm_table* t);

avm_tablebucketsinit (avm_table_bucket** p);

void avm_tablebucketdestroy (avm_table_bucket** p);

avm_memcell stack[AVM_STACKSIZE]; 
avm_memcell	ax, bx, cx;
avm_memcell	retval;
unsigned top, topsp;



static void avm_initstack(void);

double	consts_getnumber (unsigned index);
char*	consts_getstring (unsigned index);
char*	libfuncs_getused (unsigned index);

avm_memcell*	avm_translate_operand (vmarg * arg, avm_memcell *reg);

typedef void ( * execute_func_t) (instruction * ) ;

extern void execute_assign (instruction * );

extern void execute_add (instruction * );
extern void execute_sub (instruction * );
extern void execute_mul (instruction * );
extern void execute_div (instruction * );
extern void execute_mod (instruction * );

extern void execute_uminus (instruction * );


extern void execute_and (instruction * );
extern void execute_or (instruction * );
extern void execute_not (instruction * );



extern void execute_jeq (instruction * );
extern void execute_jne (instruction * );
extern void execute_jle (instruction * );
extern void execute_jge (instruction * );
extern void execute_jlt (instruction * );
extern void execute_jgt (instruction * );


extern void execute_call (instruction * );
extern void execute_pusharg (instruction * );
extern void execute_funcenter (instruction * );
extern void execute_funcexit (instruction * );


extern void execute_newtable (instruction * );
extern void execute_tablegetelem (instruction * );
extern void execute_tablesetelem (instruction * );

extern void execute_nop (instruction * );


execute_func_t executeFuncs[] = {
	execute_assign,
	execute_add,
	execute_sub,
	execute_mul,
	execute_div,
	execute_mod,
	execute_uminus,
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

void execute_cycle( void );



typedef void ( *memclear_func_t )( avm_memcell* );

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

void avm_memcellclear	(avm_memcell* m);

extern void memclear_string (avm_memcell* m);

extern void memclear_table (avm_memcell* m);

extern void avm_warning( char* format);

extern void avm_assign (avm_memcell* lv, avm_memcell* rv);

void execute_assign (instruction* instr);

extern void avm_error (char* format);

extern char* avm_tostring (avm_memcell*);

extern void	avm_calllibfunc(char* funcName);

extern void avm_callsaveenvironment (void);

void execute_call (instruction *instr);

void avm_dec_top (void);

void avm_push_envvalue (unsigned val);

unsigned avm_get_envvalue (unsigned i);

extern userfunc* avm_getfuncinfo (unsigned address);

typedef void (*library_func_t) (void);

library_func_t avm_getlibraryfunc (char* id); /*Typical hashing*/

unsigned avm_totalactuals (void);

avm_memcell* avm_getactual (unsigned i);


void avm_registerlibfunc(char *id, library_func_t addr);

typedef char* (*tostring_func_t) (avm_memcell*);

extern char* number_tostring (avm_memcell*);
extern char* string_tostring (avm_memcell*);
extern char* bool_tostring (avm_memcell*);
extern char* table_tostring (avm_memcell*);
extern char* userfunc_tostring (avm_memcell*);
extern char* libfunc_tostring (avm_memcell*);
extern char* nil_tostring (avm_memcell*);
extern char* undef_tostring (avm_memcell*);

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

typedef double (*arithmetic_func_t) (double x, double y);

double add_impl (double x, double y);
double sub_impl (double x, double y);
double mul_impl (double x, double y);
double div_impl (double x, double y);
double mod_impl (double x, double y);

arithmetic_func_t arithmeticFuncs[] = {

	add_impl,
	sub_impl,
	mul_impl,
	div_impl,
	mod_impl
};

void execute_arithmetic (instruction* instr);

typedef unsigned char (*tobool_func_t)(avm_memcell*);

unsigned char number_tobool (avm_memcell* m);
unsigned char string_tobool (avm_memcell* m);
unsigned char bool_tobool (avm_memcell* m);
unsigned char table_tobool (avm_memcell* m);
unsigned char userfunc_tobool (avm_memcell* m);
unsigned char libfunc_tobool (avm_memcell* m);
unsigned char nil_tobool (avm_memcell* m);
unsigned char undef_tobool (avm_memcell* m);

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

unsigned char avm_tobool (avm_memcell* m);

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


avm_memcell* avm_tablegetelem (avm_table* table, avm_memcell* index);

void avm_tablesetelem (avm_table* table, avm_memcell* index, avm_memcell* content);

void avm_initialize (void);

void libfunc_print (void);

void libfunc_totalarguments (void);

void libfunc_typeof (void);