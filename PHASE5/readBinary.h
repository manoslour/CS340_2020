#include "TARGET_CODE/target.h"

void printL();
void readBinary();
int get_instr_line(char* s);
void make_user_func(char* s);
void add_num_consts(char* s);
void add_lib_func(char* name);
void add_string_const(char* s);
int translate_instr_op(char* name);
void makeInstruction(char* s, int instrNo, int line);
void add_user_func(int address, int localsize, char* name);
void emit_avm_instruction(char* op, int rtype, int rval, int arg1type, int arg1val, int arg2type, int arg2val, int line);