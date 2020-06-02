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

#define AVM_ENDING_PC codeSize
#define AVM_NUMACTUALS_OFFSET	+4
#define AVM_SAVEDPC_OFFSET		+3
#define	AVM_SAVEDTOP_OFFSET		+2
#define	AVM_SAVEDTOPSP_OFFSET	+1

typedef struct avm_table avm_table;

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
	struct avm_table_bucket* next;
} avm_table_bucket;

typedef struct avm_table {
	unsigned refCounter;
	avm_table_bucket* strIndexed[AVM_TABLE_HASHSIZE];
	avm_table_bucket* numIndexed[AVM_TABLE_HASHSIZE];
	unsigned total;
}avm_table;

avm_table* avm_tablenew (void);
void avmtabledestroy (avm_table* t);
avm_memcell* avm_tablegetelem (avm_table* table, avm_memcell* index);
void avm_tablesetelem (avm_table* table, avm_memcell* index, avm_memcell* content);
void avm_tableincrefcounter (avm_table* t);
void avm_tabledecrefcounter (avm_table* t);
void avm_tablebucketsinit (avm_table_bucket** p);
void avm_tablebucketsdestroy (avm_table_bucket** p);
static void avm_initstack(void);

double	consts_getnumber (unsigned index);
char*	consts_getstring (unsigned index);
char*	libfuncs_getused (unsigned index);

avm_memcell* avm_translate_operand (vmarg* arg, avm_memcell* reg);

typedef void (*execute_func_t) (instruction*) ;

void execute_assign (instruction * );

void execute_add (instruction*);
void execute_sub (instruction*);
void execute_mul (instruction*);
void execute_div (instruction*);
void execute_mod (instruction*);
void execute_uminus (instruction*);

void execute_and (instruction*);
void execute_or (instruction*);
void execute_not (instruction*);

void execute_jeq (instruction*);
void execute_jne (instruction*);
void execute_jle (instruction*);
void execute_jge (instruction*);
void execute_jlt (instruction*);
void execute_jgt (instruction*);

void execute_call (instruction*);
void execute_pusharg (instruction*);
void execute_funcenter (instruction*);
void execute_funcexit (instruction*);

void execute_newtable (instruction*);
void execute_tablegetelem (instruction*);
void execute_tablesetelem (instruction*);
void execute_nop (instruction*);

void execute_cycle( void );

typedef void (*memclear_func_t)(avm_memcell*);

void avm_memcellclear	(avm_memcell* m);
void memclear_string (avm_memcell* m);
void memclear_table (avm_memcell* m);
void avm_warning( char* format);
void avm_assign (avm_memcell* lv, avm_memcell* rv);
void execute_assign (instruction* instr);

char* avm_tostring (avm_memcell*);
void	avm_calllibfunc(char* funcName);
void avm_callsaveenvironment (void);

void execute_call (instruction *instr);
void avm_dec_top (void);
void avm_push_envvalue (unsigned val);
unsigned avm_get_envvalue (unsigned i);

userfunc* avm_getfuncinfo (unsigned address);
typedef void (*library_func_t) (void);
library_func_t avm_getlibraryfunc (char* id); /*Typical hashing*/
unsigned avm_totalactuals (void);
avm_memcell* avm_getactual (unsigned i);
void avm_registerlibfunc(char *id, library_func_t addr);
typedef char* (*tostring_func_t) (avm_memcell*);

char* number_tostring (avm_memcell*);
char* string_tostring (avm_memcell*);
char* bool_tostring (avm_memcell*);
char* table_tostring (avm_memcell*);
char* userfunc_tostring (avm_memcell*);
char* libfunc_tostring (avm_memcell*);
char* nil_tostring (avm_memcell*);
char* undef_tostring (avm_memcell*);

typedef double (*arithmetic_func_t) (double x, double y);

double add_impl (double x, double y);
double sub_impl (double x, double y);
double mul_impl (double x, double y);
double div_impl (double x, double y);
double mod_impl (double x, double y);

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
unsigned char avm_tobool (avm_memcell* m);

void avm_initialize (void);

void libfunc_print (void);
void libfunc_totalarguments (void);
void libfunc_typeof (void);

void avm_warning(char* format); //MUST SEE AND ADD PARAMS
void avm_error(char* format, char* s);