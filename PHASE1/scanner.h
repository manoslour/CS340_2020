#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define YY_DECL int alpha_yylex(void* yyval)

typedef struct alpha_token_t token;

token {
    unsigned int numLine;
    unsigned int numToken;
    char    *content;
    char    *type;
    token *alpha_yylex;
};

token *HEAD; //Gloabal pointer to the start of tokens list

void addToken(unsigned int numline, unsigned int numToken, char *content, char *type);

void printTokenList();