#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#define RED   "\x1B[31m"
#define YEL   "\x1B[33m"
#define RESET "\x1B[0m"

#define Buckets 256
#define EXPAND_SIZE 1024
#define CURR_SIZE (total * sizeof(quad))
#define NEW_SIZE (EXPAND_SIZE * sizeof(quad) + CURR_SIZE)	

typedef enum { 
	Global, 
	Local, 
	Formal, 
	Userfunc, 
	Libfunc 
}SymbolType;

typedef enum {
	assign,			add,			sub,
	mul,			divide,			mod,
	uminus,			and,			or,
	not,			if_eq,			if_noteq,
	if_lesseq,		if_greatereq,	if_less,
	if_greater,		call,			param,
	ret,			getretval,		funcstart,
	funcend,		tablecreate,	tablegetelem,
	tablesetelem 
}iopcode;

typedef enum {
	programvar,
	functionlocal,
	formalarg
}scopespace_t;

typedef enum {
    var_e,			tableitem_e,
	programfunc_e,	libraryfunc_e,
	arithexpr_e,	boolexpr_e,
	assignexpr_e,	newtable_e,
	constnum_e,		constbool_e,
    conststring_e,	nil_e
}expr_t;

typedef enum { 
	var_s, 
	programfunc_s, 
	libraryfunc_s 
}symbol_t;

typedef struct expr {
	expr_t type;
	struct symbol* sym;
	struct expr* index;
	double numConst;
	char* strConst;
	unsigned char boolConst;
	struct expr* next;
}expr;

typedef struct quad {
	iopcode op;
	struct expr* result;
	struct expr* arg1;
	struct expr* arg2;
	unsigned int label;
	unsigned int line;
}quad;

typedef struct Variable {
	unsigned int scope;
	unsigned int line ;
	unsigned int inFunc;
}Variable;

typedef struct Function {
	unsigned int scope ;
	unsigned int line;
}Function;

typedef struct symbol {
	char *name;
	bool isActive;
	union {
		Variable *varVal;
		Function *funcVal;
	} value;

	SymbolType type;
	symbol_t extratype;
	scopespace_t space;
	unsigned int offset;

	struct symbol *next, *scope_next, *formal_next; 
}symbol;

typedef struct ScopeListEntry {
	unsigned int scope;
	struct symbol *symbols;
	struct ScopeListEntry *next, *prev;
}ScopeListEntry;

struct errorToken {
    char *output;
    char* content;
    unsigned int numLine;
    struct errorToken *next;
};


void expand();

void initialize();

void printScopeList();
 
void printErrorList();

bool hide (int scope);

void exitscopespace();

void enterscopespace();

bool enable (int scope );

void inccurrscopeoffset();

scopespace_t currscopespace();

unsigned int currscopeoffset();

char* generateName(int nameCount);

void hideScope(unsigned int scope);

int findInFunc(char *name, unsigned int scope);

int scopeLookUp(char *name, unsigned int scope);

int generalLookUp(char *name, unsigned int scope);

symbol* lookup(char* name, unsigned int scope);

void addError(char *output, char *content, unsigned int numLine);

bool scopeListInsert (symbol *sym_node, unsigned int scope);

bool insertFormal(symbol *funcname, symbol *formalEntry);

void emit(iopcode op, expr* arg1, expr* arg2, expr* result, unsigned int label, unsigned int line);

symbol* hashInsert(char *name, unsigned int line, SymbolType type, unsigned int scope, unsigned int inFunc);

#endif