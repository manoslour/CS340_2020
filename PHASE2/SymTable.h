
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define Buckets 256
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"
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
 
bool Hide (int scope );

bool Enable (int scope );

bool HashInsert(char *name, unsigned int line, enum type type, unsigned int scope, bool isActive);


bool add_scope_sym(char *name, unsigned int line,enum type type, unsigned int scope, bool isActive);

bool ScopeListInsert(struct symbol_list *sym_node);

void initialize();