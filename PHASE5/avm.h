#ifndef _AVM_H_
#define _AVM_H_

#include <stdio.h>
#include <unistd.h>
#include "TARGET_CODE/target.h"

#define AVM_STACKSIZE	4096
#define AVM_WIPEOUT(m) memset(&(m), 0, sizeof(m))	
#define AVM_STACKENV_SIZE	4
#define	AVM_MAX_INSTRUCTIONS (unsigned)	nop_v
#define AVM_TABLE_HASHSIZE	211

#define AVM_ENDING_PC codeSize

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
void avm_tableincrefcounter (avm_table* t);
void avm_tabledecrefcounter (avm_table* t);
void avm_tablebucketsinit (avm_table_bucket** p);
void avm_tablebucketsdestroy (avm_table_bucket** p);

static void avm_initstack(void);

double	consts_getnumber (unsigned index);
char*	consts_getstring (unsigned index);
char*	libfuncs_getused (unsigned index);

void execute_cycle( void );
typedef void (*memclear_func_t)(avm_memcell*);
typedef void (*execute_func_t) (instruction*) ;
avm_memcell* avm_translate_operand (vmarg* arg, avm_memcell* reg);

//---------------------------------------------
extern void execute_assign (instruction*);
extern void execute_add(instruction*);
extern void execute_sub(instruction*);
extern void execute_mul(instruction*);
extern void execute_div(instruction*);
extern void execute_mod(instruction*);
extern void execute_uminus(instruction*);
extern void execute_and(instruction*);
extern void execute_or(instruction*);
extern void execute_not(instruction*);
extern void execute_jeq(instruction*);
extern void execute_jne(instruction*);
extern void execute_jle(instruction*);
extern void execute_jge(instruction*);
extern void execute_jlt(instruction*);
extern void execute_jgt(instruction*);
extern void execute_call(instruction*);
extern void execute_pusharg(instruction*);
extern void execute_funcenter(instruction*);
extern void execute_funcexit(instruction*);
extern void execute_newtable(instruction*);
extern void execute_tablegetelem(instruction*);
extern void execute_tablesetelem(instruction*);
extern void execute_nop (instruction*);
//---------------------------------------------

void memclear_table (avm_memcell* m);
void memclear_string (avm_memcell* m);
void avm_memcellclear	(avm_memcell* m);
void printStack();

#endif