#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define RED   "\x1B[31m"
#define YEL   "\x1B[33m"
#define RESET "\x1B[0m"

#define Buckets 256

enum SymbolType {Global, Local, Formal, Userfunc, Libfunc}; 

typedef struct Variable{
	const char *name;
	unsigned int scope;
	unsigned int line ;
}Variable;

typedef struct Function{
	const char *name; 
	unsigned int scope ;
	unsigned int line;
	//struct SymbolTableEntry *next; 
	// Formal arguments list. TO-SEE AGAIN
}Function;

typedef struct SymbolTableEntry{
	bool isActive;
	union {
		Variable *varVal;
		Function *funcVal;
	} value;
	enum SymbolType type;

	struct SymbolTableEntry *next, *scope_next, *formal_next; 
	// TO-SEE AGAIN.
	// Kalytera na fygei to formal_next apo edw kai na meinei sto function.
	// Einai extra plhroforia poy den xreiazetai sto struct giati exei na kanei mono me functions
}SymbolTableEntry;

typedef struct ScopeListEntry{
	unsigned int scope;
	struct SymbolTableEntry *symbols;
	struct ScopeListEntry *next, *prev;
}ScopeListEntry;

struct errorToken {
    char *output;
    char* content;
    unsigned int numLine;
    struct errorToken *next;
};

void initialize();

void printScopeList();
 
void printErrorList();

bool hide (int scope);

bool enable (int scope );

char* generateName(int nameCount);

void hideScope(unsigned int scope);

int findVarScope(char *name, unsigned int scope);

int scopeLookUp(char *name, unsigned int scope);

int generalLookUp(char *name, unsigned int scope);

void addError(char *output, char *content, unsigned int numLine);

bool scopeListInsert (struct SymbolTableEntry *sym_node, unsigned int scope);

bool insertFormal(struct SymbolTableEntry *funcname, struct SymbolTableEntry *formalEntry);

struct SymbolTableEntry* hashInsert(char *name, unsigned int line, enum SymbolType type, unsigned int scope);

#endif