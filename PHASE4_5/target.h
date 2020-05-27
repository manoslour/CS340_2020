#include <stdio.h>
#include <unistd.h>

#define AVM_STACKSIZE 4096
#define AVM_WIPEOUT(m) memset(&(m), 0, sizeof(m))
#define AVM_TABLE_HASHSIZE 211

typedef enum {
    assign_v,       add_v,          sub_v,
    mul_v,          div_v,          mod_v,
    uminus_v,       and_v,          or_v,
    not_v,          jeq_v,          jne_v,
    jle_v,          jge_v,          jlt_v,
    jgt_v,          call_v,         pusharg_v,
    funcenter_v,    funcexit_v,     newtable_v,
    tablegetelem_v, tablesetelem_v, nop_v    
}vmopcode;

typedef enum {
    label_a,
    global_a,
    formal_a,
    local_a,
    number_a,
    string_a,
    bool_a,
    nil_a,
    userfunc_a,
    libfunc_a,
    retval_a
}vmarg_t;

typedef enum {
    number_m,
    string_m,
    bool_m,
    table_m,
    userfunc_m,
    libfunc_m,
    nil_m,
    undef_m
}avm_memcell_t;

typedef struct vmarg {
    vmarg_t type;
    unsigned val;
}vmarg;

typedef struct instruction {
    vmopcode opcode;
    vmarg result;
    vmarg arg1;
    vmarg arg2;
    unsigned srcLine;
}instruction;

typedef struct userfunc {
    unsigned address;
    unsigned localSize;
    char* id;
}userfunc;

typedef struct avm_table{
    unsigned refCounter;
    avm_table_bucket* strIndexed[AVM_TABLE_HASHSIZE];
    avm_table_bucket* numIndexed[AVM_TABLE_HASHSIZE];
    unsigned total;
}avm_table;

typedef struct avm_memcell {
    avm_memcell_t type;
    union {
        double numVal;
        char* strVal;
        unsigned char boolVal;
        avm_table* tableVal;
        unsigned funcVal;
        char* libfuncVal;
    }data;
}avm_memcell;

typedef struct avm_table_bucket {
    avm_memcell key;
    avm_memcell value;
    struct avm_table_bucket* next;
}avm_table_bucket;


typedef struct instruction
{
    vmopcode opcode;
    vmarg result; 
    vmarg arg1; 
    vmarg arg2; 
    unsigned srcLine;  
}instruction;

typedef struct incomplete_jump 
{
    unsigned instNo; // the jump instruction Number
    unsigned iaddress; // the i-code jump-target address; 
    incomplete_jump* next; 
}incomplete_jump;


incomplete_jump* ij_head = (incomplete_jump*) 0;
unsigned ij_total = 0;



extern void generate_ADD (quad*);
extern void generate_SUB (quad*);
extern void generate_MUL (quad*);
extern void generate_DIV (quad*);
extern void generate_MOD (quad*);
extern void generate_NEWTABLE (quad*);
extern void generate_TABLEGETELM (quad*);
extern void generate_TABLESETELM (quad*);
extern void generate_ASSIGN (quad*);
extern void generate_NOP (quad*);
extern void generate_JUMP (quad*);
extern void generate_IF_EQ (quad*);
extern void generate_IF_NOTEQ (quad*);
extern void generate_GREATER (quad*);
extern void generate_GREATEREQ (quad*);
extern void generate_LESS (quad*);
extern void generate_LESSEQ (quad*);
extern void generate_NOT (quad*);
extern void generate_OR (quad*);
extern void generate_PARAM (quad*);
extern void generate_CALL (quad*);
extern void generate_GETRETVAL (quad*);
extern void generate_FUNCSTART (quad*);
extern void generate_RETURN (quad*);
extern void generate_FUNCEND (quad*);

















double* numConsts;
unsigned totalNumConsts;
char** stringConsts;
unsigned totalStringConsts;
char** namedLibfuncs;
unsigned totalNamedLibfuncs;
userfunc* userFuncs;
unsigned totalUserFuncs;

avm_memcell stack[AVM_STACKSIZE];

static void avm_initstack(void);
void avm_tableincrefcounter(avm_table* t);
void avm_tabledecrefcounter(avm_table* t);
void avm_tablebucketsinit(avm_table_bucket** p);
avm_table* avm_tablenew(void);
void avm_memcellclear(avm_memcell* m);
void avm_tablebucketsdestroy(avm_table_bucket** p);
void avm_tabledestroy(avm_table* t);
avm_memcell* avm_tablegetelem(avm_memcell* key);
void avm_tablesetelem(avm_memcell* key, avm_memcell* value);
void make_operant (expr* e, vmarg* arg);
void make_numberoperand (vmarg* arg, double val);
void make_booloperand (vmarg *arg, unsigned val);
void make_retvaloperand (vmarg *arg);

void add_incomplete_jump(unsigned instNo, unsigned iaddress);