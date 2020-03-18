#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFERSIZE 256
#define YY_DECL int alpha_yylex(void* yyval)

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define RESET "\x1B[0m"


struct alpha_token_t {
    unsigned int numLine;
    unsigned int numToken;
    char *content;
    char *type;
    char *extraType;
    char *subType;
    struct alpha_token_t *next;
};

struct errorToken {
    char *output;
    char* content;
    unsigned int numLine;
    struct errorToken *next;
};


struct alpha_token_t *HEAD = NULL; //Global pointer to the start of tokens list

struct errorToken *ERROR_HEAD = NULL; // GLobal pointer to the start of error_tokkens list

void addError(char *output, char *content, unsigned int numLine);

void addToken(unsigned int numLine, unsigned int numToken, char *content, char *type, char* extraType, char *subType);

void printTokenList();

void printErrorList();