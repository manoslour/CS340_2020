#include <stdio.h>

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
avm_table* avm_tablenew(void);
void avm_tabledestroy(avm_table* t);
avm_memcell* avm_tablegetelem(avm_memcell* key);
void avm_tablesetelem(avm_memcell* key, avm_memcell* value);