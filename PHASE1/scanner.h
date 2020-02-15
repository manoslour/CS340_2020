#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define YY_DECL int alpha_yylex(void* yyval)

struct alpha_token_t{

    unsigned int numline;
    unsigned int numToken;
    char* content;
    char* type;
    char* extraType;

    struct alpha_token_t *next;
};

struct alpha_token_t *HEAD = NULL; // Global pointer to head of tokens list

void insertToken(unsigned int numline, unsigned int numToken, char* content, char* type, char* extraType);

void printTokenList();
