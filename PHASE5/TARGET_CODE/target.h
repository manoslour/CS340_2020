#ifndef _TARGET_H_
#define _TARGET_H_

#include "symTable.h"

//--------------------------------------------------------------------------
#define EXPAND_INSTR_SIZE 1024
#define CURR_INSTR_SIZE (totalInstructions * sizeof(instruction))
#define NEW_INSTR_SIZE (EXPAND_INSTR_SIZE * sizeof(instruction) + CURR_INSTR_SIZE)
//--------------------------------------------------------------------------

typedef enum {
    assign_v,       add_v,          sub_v,
    mul_v,          div_v,          mod_v,
    uminus_v,       and_v,          or_v,
    not_v,          jeq_v,          jne_v,
    jle_v,          jge_v,          jlt_v,
    jgt_v,          call_v,         pusharg_v,
    funcenter_v,    funcexit_v,     newtable_v,
    tablegetelem_v, tablesetelem_v, jump_v, nop_v
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

typedef struct vmarg {
    vmarg_t type;
    int val;
}vmarg;

typedef struct instruction {
    vmopcode opcode;
    vmarg *result;
    vmarg *arg1;
    vmarg *arg2;
    unsigned srcLine;
}instruction;

typedef struct userfunc {
    unsigned address;
    unsigned localSize;
    char* id;
}userfunc;

typedef struct incomplete_jump {
    unsigned instrNo; // the jump instruction Number
    unsigned iaddress; // the i-code jump-target address; 
    struct incomplete_jump* next; 
}incomplete_jump;

typedef struct funcStack {
    symbol* func;
    struct funcStack* next;
}funcStack;

void expandInstr();
void emit_instr(instruction *t);
unsigned currprocessedquad();
typedef void (*generator_func_t) (quad*); 

void generate (vmopcode op, quad* quad);
void generate_relational(vmopcode op, quad* quad);
void exec_generate(void);

void generate_AND (quad*);
void generate_ADD (quad*);
void generate_SUB (quad*);
void generate_MUL (quad*);
void generate_DIV (quad*);
void generate_MOD (quad*);
void generate_UMINUS(quad* q);
void generate_NEWTABLE (quad*);
void generate_TABLEGETELM (quad*);
void generate_TABLESETELM (quad*);
void generate_ASSIGN (quad*);
void generate_NOP (quad*);
void generate_JUMP (quad*);
void generate_IF_EQ (quad*);
void generate_IF_NOTEQ (quad*);
void generate_IF_GREATER (quad*);
void generate_IF_GREATEREQ (quad*);
void generate_IF_LESS (quad*);
void generate_IF_LESSEQ (quad*);
void generate_NOT (quad*);
void generate_OR (quad*);
void generate_PARAM (quad*);
void generate_CALL (quad*);
void generate_GETRETVAL (quad*);
void generate_FUNCSTART (quad*);
void generate_RETURN (quad*);
void generate_FUNCEND (quad*);

void make_operand (expr* e, vmarg* arg);
void make_numberoperand (vmarg* arg, double val);
void make_booloperand (vmarg *arg, unsigned val);
void make_retvaloperand (vmarg *arg);

void add_incomplete_jump(unsigned instrNo, unsigned iaddress);
void patch_incomplete_jumps(void);
void printInstructions ();

char *translateopcode_v(vmopcode opcode);

unsigned consts_newstring (char* s); 
unsigned consts_newnumber (double n);
unsigned libfuncs_newused (char* s); 
unsigned userfuncs_newfunc (symbol* sym); 

int isEmptyFunc();
symbol* peekFunc();
void pushFunc(symbol* func);
symbol* popFunc();

returnlist* append(returnlist* list, int label);
void backpatch(returnlist* list, int label);

void printInstructions();
void print_array();

instruction* createInstruction ();

void avmbinaryfile();
unsigned magicnumber();
void arrays();
void strings();
void numbers();
void userfunctions();
void initLibfuncs();
void libfunctions();

#endif

