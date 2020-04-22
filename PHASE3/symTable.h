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
#define EXPAND_SIZE 1024
#define CURR_SIZE (total * sizeof(quad))
#define NEW_SIZE (EXPAND_SIZE * sizeof(quad) + CURR_SIZE)	

enum SymbolType { Global, Local, Formal, Userfunc, Libfunc };

enum iopcode {
	assign,			add,			sub,
	mul,			divide,			mod,
	uminus,			and,			or,
	not,			if_eq,			if_noteq,
	if_lesseq,		if_greatereq,	if_less,
	if_greater,		call,			param,
	ret,			getretval,		funcstart,
	funcend,		tablecreate,	tablegetelem,
	tablesetelem 
};

enum scopespace_t {
	programvar,
	functionlocal,
	formalarg
};

enum symbol_t { var_s, programfunc_s, libraryfunc_s };

struct expr;

struct quad {
	enum iopcode op;
	struct expr* result;
	struct expr* arg1;
	struct expr* arg2;
	unsigned int label;
	unsigned int line;
};

struct quad* quads = (struct quad*) 0;
unsigned total = 0;
unsigned int currQuad = 0;

struct symbol {
	enum symbol_t type;
	char* name;
	enum scopespace_t space;
	unsigned int offset;
	unsigned int scope;
	unsigned int line
};

unsigned int programVarOffset = 0;
unsigned int functionLocalOffset = 0;
unsigned int formalArgOffset = 0;
unsigned int scopeSpaceCounter = 1;

typedef struct Variable {
	const char *name;
	unsigned int scope;
	unsigned int line ;
	unsigned int inFunc;
}Variable;

typedef struct Function {
	const char *name; 
	unsigned int scope ;
	unsigned int line;
	//struct SymbolTableEntry *next; 
	// Formal arguments list. TO-SEE AGAIN
}Function;

typedef struct SymbolTableEntry {
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

typedef struct ScopeListEntry {
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

enum scopespace_t currscopespace();

void initialize();

void printScopeList();
 
void printErrorList();

bool hide (int scope);

bool enable (int scope );

char* generateName(int nameCount);

void hideScope(unsigned int scope);

int findInFunc(char *name, unsigned int scope);

int scopeLookUp(char *name, unsigned int scope);

int generalLookUp(char *name, unsigned int scope);

void addError(char *output, char *content, unsigned int numLine);

bool scopeListInsert (struct SymbolTableEntry *sym_node, unsigned int scope);

bool insertFormal(struct SymbolTableEntry *funcname, struct SymbolTableEntry *formalEntry);

struct SymbolTableEntry* hashInsert(char *name, unsigned int line, enum SymbolType type, unsigned int scope, unsigned int inFunc);

#endif