#ifndef _EXEC_TABLE_H_
#define _EXEC_TABLE_H_

#include "avm.h"

void execute_newtable (instruction*);
void execute_tablegetelem (instruction*);
void execute_tablesetelem (instruction*);

avm_memcell* avm_tablegetelem (avm_table* table, avm_memcell* index);
void avm_tablesetelem (avm_table* table, avm_memcell* index, avm_memcell* content);

#endif