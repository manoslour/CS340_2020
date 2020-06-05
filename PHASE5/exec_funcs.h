#ifndef _EXEC_FUNCS_H_
#define _EXEC_FUNCS_H_

#include "avm.h"

#define AVM_NUMACTUALS_OFFSET	+4
#define AVM_SAVEDPC_OFFSET		+3
#define	AVM_SAVEDTOP_OFFSET		+2
#define	AVM_SAVEDTOPSP_OFFSET	+1

void execute_call (instruction*);
void execute_pusharg (instruction*);
void execute_funcenter (instruction*);
void execute_funcexit (instruction*);

char* avm_tostring (avm_memcell*);
void avm_calllibfunc(char* funcName);
void avm_callsaveenvironment (void);

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

void avm_initialize (void);

char* number_tostring (avm_memcell*);
char* string_tostring (avm_memcell*);
char* bool_tostring (avm_memcell*);
char* table_tostring (avm_memcell*);
char* userfunc_tostring (avm_memcell*);
char* libfunc_tostring (avm_memcell*);
char* nil_tostring (avm_memcell*);
char* undef_tostring (avm_memcell*);

void libfunc_print(void);
//void libfunc_input(void);
//void libfunc_objectmemberkeys();
//void libfunc_objecttotalmembers();
//void libfunc_objectcopy();
void libfunc_totalarguments();
//void libfunc_argument();
void libfunc_typeof(void);
//void libfunc_strtonum();
//void libfunc_sqrt();
//void libfunc_cos();
//void libfunc_sin();

#endif