#include "avm.h"

extern void execute_newtable (instruction*);
extern void execute_tablegetelem (instruction*);
extern void execute_tablesetelem (instruction*);

avm_memcell* avm_tablegetelem (avm_table* table, avm_memcell* index);
void avm_tablesetelem (avm_table* table, avm_memcell* index, avm_memcell* content);