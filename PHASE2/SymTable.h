
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
	struct symbol_list *next,*scope_next; // next shows the next symbol in the hash list, the scope_next shows the next symbol in scope_list gege?
};

/**
struct for scopes
*/
struct scope_list{
	unsigned int scope;
	struct symbol_list *symbols;
	struct scope_list *next, *prev;
};

/*Global pointer to the scope list's head*/
struct scope_list *scope_head = NULL;

struct symbol_list *HashTable[Buckets];

void print();
void ScopePrint();
void bubbleSort(struct scope_list *start); 

bool add_scope_sym(char *name, unsigned int line,enum type type, unsigned int scope, bool isActive);

bool ScopeListInsert(struct symbol_list *sym_node);