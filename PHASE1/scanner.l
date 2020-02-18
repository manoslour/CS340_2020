%option noyywrap
%option yylineno

%{
    #include "scanner.h"
    int tokenNum = 0;
%}

IF      "if"
ELSE    "else"
WHILE   "while"
FOR     "for"
FUNCTION    "function"
RETURN      "return"
BREAK       "break"
CONTINUE    "continue"
AND     "and"
NOT     "not"
OR      "or"
LOCAL   "local"
TRUE    "true"
FALSE   "false"
NIL     "nil"

ASSIGN  "="
PLUS    "+"
MINUS   "-"
MULT    "*"
DIV     "/"
MOD     "%"
EQUAL   "=="
NOT_EQUAL     "!="
INCR    "++"
DECR    "--"
GREATER ">"
LESS    "<"
GREATER_EQ    ">="
LESS_EQ     "<="

LCURLY_BR  "{"
RCURLY_BR  "}"
L_BR  "["
R_BR  "]"
L_PAR   "("
R_PAR   ")"
SEMICOL ";"
COMMA   ","
COLON   ":"
DCOLON  "::"
DOT     "."
DDOT    ".."

INTEGER [0-9]+
REAL [0-9]+\.[0-9]+
WHITESPACE [\t\n\r ]
ID [a-zA-Z][a-zA-Z_0-9]*

%%

{IF}        {addToken(yylineno, ++tokenNum, yytext, "KEYWORD IF", "\t<-enumerated");}
{ELSE}      {addToken(yylineno, ++tokenNum, yytext, "KEYWORD ELSE", "\t<-enumerated");}
{WHILE}     {addToken(yylineno, ++tokenNum, yytext, "KEYWORD WHILE", "\t<-enumerated");}
{FOR}       {addToken(yylineno, ++tokenNum, yytext, "KEYWORD FOR", "\t<-enumerated");}
{FUNCTION}  {addToken(yylineno, ++tokenNum, yytext, "KEYWORD FUNCTION", "\t<-enumerated");}
{RETURN}    {addToken(yylineno, ++tokenNum, yytext, "KEYWORD RETURN", "\t<-enumerated");}
{BREAK}     {addToken(yylineno, ++tokenNum, yytext, "KEYWORD BREAK", "\t<-enumerated");}
{CONTINUE}  {addToken(yylineno, ++tokenNum, yytext, "KEYWORD CONTINUE", "\t<-enumerated");}
{AND}       {addToken(yylineno, ++tokenNum, yytext, "KEYWORD AND", "\t<-enumerated");}
{NOT}       {addToken(yylineno, ++tokenNum, yytext, "KEYWORD NOT", "\t<-enumerated");}
{OR}        {addToken(yylineno, ++tokenNum, yytext, "KEYWORD OR", "\t<-enumerated");}
{LOCAL}     {addToken(yylineno, ++tokenNum, yytext, "KEYWORD LOCAL", "\t<-enumerated");}
{TRUE}      {addToken(yylineno, ++tokenNum, yytext, "KEYWORD TRUE", "\t<-enumerated");}
{FALSE}     {addToken(yylineno, ++tokenNum, yytext, "KEYWORD FALSE", "\t<-enumerated");}
{NIL}       {addToken(yylineno, ++tokenNum, yytext, "KEYWORD NIL", "\t<-enumerated");}

{ASSIGN}    {addToken(yylineno, ++tokenNum, yytext, "OPERATOR ASSIGN", "\t<-enumerated");}
{PLUS}      {addToken(yylineno, ++tokenNum, yytext, "OPERATOR PLUS", "\t<-enumerated");}
{MINUS}     {addToken(yylineno, ++tokenNum, yytext, "OPERATOR MINUS", "\t<-enumerated");}
{MULT}      {addToken(yylineno, ++tokenNum, yytext, "OPERATOR MULT", "\t<-enumerated");}
{DIV}       {addToken(yylineno, ++tokenNum, yytext, "OPERATOR DIV", "\t<-enumerated");}
{MOD}       {addToken(yylineno, ++tokenNum, yytext, "OPERATOR MOD", "\t<-enumerated");}
{EQUAL}     {addToken(yylineno, ++tokenNum, yytext, "OPERATOR EQUAL", "\t<-enumerated");}
{NOT_EQUAL}   {addToken(yylineno, ++tokenNum, yytext, "OPERATOR NOT_EQUAL", "\t<-enumerated");}
{INCR}      {addToken(yylineno, ++tokenNum, yytext, "OPERATOR INCR", "\t<-enumerated");}
{DECR}      {addToken(yylineno, ++tokenNum, yytext, "OPERATOR DECR", "\t<-enumerated");}
{GREATER}   {addToken(yylineno, ++tokenNum, yytext, "OPERATOR GREATER", "\t<-enumerated");}
{LESS}      {addToken(yylineno, ++tokenNum, yytext, "OPERATOR LESS", "\t<-enumerated");}
{GREATER_EQ}    {addToken(yylineno, ++tokenNum, yytext, "OPERATOR GREATER_EQ", "\t<-enumerated");}
{LESS_EQ}       {addToken(yylineno, ++tokenNum, yytext, "OPERATOR LESS_EQ", "\t<-enumerated");}

{LCURLY_BR}        {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION LCURLY_BRC", "\t<-enumerated");}
{RCURLY_BR}        {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION RCURLY_BRC", "\t<-enumerated");}
{L_BR}        {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION L_BRCK", "\t<-enumerated");}
{R_BR}        {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION R_BRCK", "\t<-enumerated");}
{L_PAR}         {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION L_PAR", "\t<-enumerated");}
{R_PAR}         {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION R_PAR", "\t<-enumerated");}
{SEMICOL}       {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION SEMICOL", "\t<-enumerated");}
{COMMA}         {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION COMMA", "\t<-enumerated");}
{COLON}         {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION COLON", "\t<-enumerated");}
{DCOLON}        {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION DCOLON", "\t<-enumerated");}
{DOT}           {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION DOT", "\t<-enumerated");}
{DDOT}          {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION DDOT", "\t<-enumerated");}

{INTEGER}       {   int bufsize = BUFFERSIZE;
                    char* fullType = malloc(sizeof(char) * bufsize);
                    strcpy(fullType, "INT_CONST\t");
                    if(sizeof(yytext) >= bufsize){
                        bufsize += BUFFERSIZE;
                        fullType = realloc(fullType, bufsize);
                    }
                    strcat(fullType, yytext);
                    addToken(yylineno, ++tokenNum, yytext, fullType, "\t<-integer");
                }

{REAL}          {   int bufsize = BUFFERSIZE;
                    char* fullType = malloc(sizeof(char) * bufsize);
                    strcpy(fullType, "REAL_CONST\t");
                    if(sizeof(yytext) >= bufsize){
                        bufsize += BUFFERSIZE;
                        fullType = realloc(fullType, bufsize);
                    }
                    strcat(fullType, yytext);
                    addToken(yylineno, ++tokenNum, yytext, fullType, "\t<-real");
                }

{ID}            {   int bufsize = BUFFERSIZE;
                    char* fullType = malloc(sizeof(char) * bufsize);
                    strcpy(fullType, "IDENT\t");
                    if(sizeof(yytext) >= bufsize){
                        bufsize += BUFFERSIZE;
                        fullType = realloc(fullType, bufsize);
                    }
                    strcat(fullType, "\"");
                    strcat(fullType, yytext);
                    strcat(fullType, "\"");
                    addToken(yylineno, ++tokenNum, yytext, fullType, "\t<-char*");
                }

{WHITESPACE}    {}

.               {printf("Undefined Character: %s in line: %d\n", yytext, yylineno);}

%%

void addToken(unsigned int numLine, unsigned int numToken, char *content, char *type, char *extraType){

    struct alpha_token_t *last;
    struct alpha_token_t *newToken = (struct alpha_token_t *)malloc(sizeof(struct alpha_token_t));
    char *tmpContent = strdup(content);
    char *tmpType = strdup(type);
    char *tmpExtraType = strdup(extraType);

    newToken->numLine = numLine;
    newToken->numToken = numToken;
    newToken->content = tmpContent;
    newToken->type = tmpType;
    newToken->extraType = tmpExtraType;
    newToken->next = NULL;

    last = HEAD;
    
    if(HEAD == NULL){
        HEAD = newToken;
    }
    else{
        while(last->next != NULL){
            last = last->next;
        }
        last->next = newToken;
    }
}

void printTokens(){

    struct alpha_token_t *tmp = HEAD;
    printf("\n------------------\tLEXICAL ANALYSIS\t------------------\n\n");

    while(tmp != NULL){
        printf("%d:\t", tmp->numLine);
        printf("#%d\t", tmp->numToken);
        printf("%s\t ", tmp->content);
        printf("%s\t", tmp->type);
        printf("%s\n", tmp->extraType);

        tmp = tmp->next;
    }
}

int main(int argc, char** argv)
{
    if(argc > 1){
        if(!(yyin = fopen(argv[1], "r"))){
            fprintf(stderr,"Cannot read file: %s\n", argv[1]);
            return 1;
        }
    }
    else{
        yyin = stdin;
    }

    alpha_yylex(HEAD);
    printTokens();

    return 0;
}