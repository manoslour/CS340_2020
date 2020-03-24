
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
enum SymbolType {GLOBAL, LOCAL, FORMAL, USERFUNC, LIBFUNC}; 

typedef struct Variable{
	const char *name;
	unsigned int scope;
	unsigned int line ;
}Variable;


typedef struct Function{
	const char *name; 
	unsigned int scope ;
	unsigned int line;
	struct SymbolTableEntry *next;
}Function;


/**
struct for symbols
*/
typedef struct SymbolTableEntry{
	bool isActive;
	union {
		Variable *varVal;
		Function *funcVal;
	} value;
	enum SymbolType type;
	struct SymbolTableEntry *next,*scope_next, *formal_next; // next shows the next symbol in the hash list, the scope_next shows the next symbol in scope_list gege?
}SymbolTableEntry;


/**
struct for scopes
*/
typedef struct ScopeListEntry{
	unsigned int scope;
	struct SymbolTableEntry *symbols;
	struct ScopeListEntry *next, *prev;
}ScopeListEntry;


/*Global pointer to the scope list's head*/
struct ScopeListEntry *scope_head = NULL;
struct SymbolTableEntry *HashTable[Buckets];

void PrintHash();

void scopePrint();
 
bool hide (int scope );

bool enable (int scope );

bool hashInsert(char *name, unsigned int line, enum SymbolType type, unsigned int scope);

bool ScopeListInsert (struct SymbolTableEntry *sym_node, unsigned int scope);

void Initialize();
