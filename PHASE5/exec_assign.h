#ifndef _EXEC_ASSIGN_H_
#define _EXEC_ASSIGN_H_

#include "avm.h"

void execute_assign (instruction* instr);
void avm_assign (avm_memcell* lv, avm_memcell* rv);

#endif