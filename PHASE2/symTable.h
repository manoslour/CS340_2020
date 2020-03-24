#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_

#include <stdio.h>

#define BUCKETS 512

enum SymbolType {GLOBAL, LOCAL, FORMAL, USERFUNC, LIBFUNC};

typedef struct Variable {
    const char* name;
    unsigned int scope;
    unsigned int line;
};

typedef struct Function {
    const char* name;
    // List of argumenst - TODO!!
    unsigned int scope;
    unsigned int line;
};

// Used to create a list of the symbols saved in each scope.
typedef struct ScopeListNode {
    unsigned int scope;
    symbolTableEntry *symbol_list;

    ScopeListNode *next;
};

typedef struct SymbolTableEntry {
    bool isActive;
    union{
        Variable *varVal;
        Function *funcVal;
    } value;
    enum SymbolType type;

    SymbolTableEntry *next; //  Points to the next symbol in the hash list. 
    ScopeListNode *scope_next; // Points to the next symbol in scope list
};

ScopeListNode *scopeListHead = NULL; // Global pointer to scope list's head.
SymbolTableEntry *SymbolTable[BUCKETS];
    
int hashFunc(char* name);

void insert(SymbolTableEntry *symbol);

int scopeLookup(unsigned int scope);

int totalLookup(unsigned int scope);

void hide(unsigned int scope);

char* generateFuncName();

void initLibFuncs();

void printScopeList();

#endif