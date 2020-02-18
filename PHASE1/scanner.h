#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFERSIZE 1024
#define YY_DECL int alpha_yylex(void* yyval)

struct alpha_token_t {
    unsigned int numLine;
    unsigned int numToken;
    char *content;
    char *type;
    char *extraType;
    struct alpha_token_t *next;
};

struct alpha_token_t *HEAD = NULL; //Global pointer to the start of tokens list

void addToken(unsigned int numLine, unsigned int numToken, char *content, char *type, char* extraType);

void printTokenList();