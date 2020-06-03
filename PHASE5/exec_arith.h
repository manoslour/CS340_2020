#include "avm.h"

#define execute_add execute_arithmetic
#define execute_sub execute_arithmetic
#define execute_mul execute_arithmetic
#define execute_div execute_arithmetic
#define execute_mod execute_arithmetic

void execute_add (instruction*);
void execute_sub (instruction*);
void execute_mul (instruction*);
void execute_div (instruction*);
void execute_mod (instruction*);
//void execute_uminus (instruction*);

double add_impl (double x, double y);
double sub_impl (double x, double y);
double mul_impl (double x, double y);
double div_impl (double x, double y);
double mod_impl (double x, double y);

void execute_arithmetic (instruction* instr);
typedef double (*arithmetic_func_t) (double x, double y);