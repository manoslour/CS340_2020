
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define Buckets 256

/**
enumetate for the type of the symbol 
*/
enum type {Global, Local, Formal, Userfunc, Libfunc}; 


/**
struct for symbols
*/
struct symbol_list{
	char *name ;
	unsigned int line; 
	enum type type ;
	unsigned int scope ;
	bool isActive;
	struct symbol_list *next;
};

/**
struct for scopes
*/
struct scope_list{
	unsigned int scope;
	struct symbol_list *symbols;
	struct scope_list *next, *prev;
};

struct scope_list *scope_head = NULL;

struct symbol_list *HashTable[Buckets];

void print();

bool add_scope_sym(char *name, unsigned int line,enum type type, unsigned int scope, bool isActive);