%option noyywrap
%option yylineno

%{
    #include "scanner.h"
    int startLine;
    int nested = 0;
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
LINE_COMMENT "//".*

%x COMMENT

%%

{IF}        {addToken(yylineno, ++tokenNum, yytext, "KEYWORD\tIF", "\t<-enumerated");}
{ELSE}      {addToken(yylineno, ++tokenNum, yytext, "KEYWORD\tELSE", "\t<-enumerated");}
{WHILE}     {addToken(yylineno, ++tokenNum, yytext, "KEYWORD\tWHILE", "\t<-enumerated");}
{FOR}       {addToken(yylineno, ++tokenNum, yytext, "KEYWORD\tFOR", "\t<-enumerated");}
{FUNCTION}  {addToken(yylineno, ++tokenNum, yytext, "KEYWORD\tFUNCTION", "\t<-enumerated");}
{RETURN}    {addToken(yylineno, ++tokenNum, yytext, "KEYWORD\tRETURN", "\t<-enumerated");}
{BREAK}     {addToken(yylineno, ++tokenNum, yytext, "KEYWORD\tBREAK", "\t<-enumerated");}
{CONTINUE}  {addToken(yylineno, ++tokenNum, yytext, "KEYWORD\tCONTINUE", "\t<-enumerated");}
{AND}       {addToken(yylineno, ++tokenNum, yytext, "KEYWORD\tAND", "\t<-enumerated");}
{NOT}       {addToken(yylineno, ++tokenNum, yytext, "KEYWORD\tNOT", "\t<-enumerated");}
{OR}        {addToken(yylineno, ++tokenNum, yytext, "KEYWORD\tOR", "\t<-enumerated");}
{LOCAL}     {addToken(yylineno, ++tokenNum, yytext, "KEYWORD\tLOCAL", "\t<-enumerated");}
{TRUE}      {addToken(yylineno, ++tokenNum, yytext, "KEYWORD\tTRUE", "\t<-enumerated");}
{FALSE}     {addToken(yylineno, ++tokenNum, yytext, "KEYWORD\tFALSE", "\t<-enumerated");}
{NIL}       {addToken(yylineno, ++tokenNum, yytext, "KEYWORD\tNIL", "\t<-enumerated");}

{ASSIGN}    {addToken(yylineno, ++tokenNum, yytext, "OPERATOR\tASSIGN", "\t<-enumerated");}
{PLUS}      {addToken(yylineno, ++tokenNum, yytext, "OPERATOR\tPLUS", "\t<-enumerated");}
{MINUS}     {addToken(yylineno, ++tokenNum, yytext, "OPERATOR\tMINUS", "\t<-enumerated");}
{MULT}      {addToken(yylineno, ++tokenNum, yytext, "OPERATOR\tMULT", "\t<-enumerated");}
{DIV}       {addToken(yylineno, ++tokenNum, yytext, "OPERATOR\tDIV", "\t<-enumerated");}
{MOD}       {addToken(yylineno, ++tokenNum, yytext, "OPERATOR\tMOD", "\t<-enumerated");}
{EQUAL}     {addToken(yylineno, ++tokenNum, yytext, "OPERATOR\tEQUAL", "\t<-enumerated");}
{NOT_EQUAL}   {addToken(yylineno, ++tokenNum, yytext, "OPERATOR\tNOT_EQUAL", "\t<-enumerated");}
{INCR}      {addToken(yylineno, ++tokenNum, yytext, "OPERATOR\tINCR", "\t<-enumerated");}
{DECR}      {addToken(yylineno, ++tokenNum, yytext, "OPERATOR\tDECR", "\t<-enumerated");}
{GREATER}   {addToken(yylineno, ++tokenNum, yytext, "OPERATOR\tGREATER", "\t<-enumerated");}
{LESS}      {addToken(yylineno, ++tokenNum, yytext, "OPERATOR\tLESS", "\t<-enumerated");}
{GREATER_EQ}    {addToken(yylineno, ++tokenNum, yytext, "OPERATOR\tGREATER_EQ", "\t<-enumerated");}
{LESS_EQ}       {addToken(yylineno, ++tokenNum, yytext, "OPERATOR\tLESS_EQ", "\t<-enumerated");}

{LCURLY_BR}        {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION\tLCURLY_BRC", "\t<-enumerated");}
{RCURLY_BR}        {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION\tRCURLY_BRC", "\t<-enumerated");}
{L_BR}        {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION\tL_BRCK", "\t<-enumerated");}
{R_BR}        {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION\tR_BRCK", "\t<-enumerated");}
{L_PAR}         {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION\tL_PAR", "\t<-enumerated");}
{R_PAR}         {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION\tR_PAR", "\t<-enumerated");}
{SEMICOL}       {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION\tSEMICOL", "\t<-enumerated");}
{COMMA}         {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION\tCOMMA", "\t<-enumerated");}
{COLON}         {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION\tCOLON", "\t<-enumerated");}
{DCOLON}        {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION\tDCOLON", "\t<-enumerated");}
{DOT}           {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION\tDOT", "\t<-enumerated");}
{DDOT}          {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION\tDDOT", "\t<-enumerated");}

{INTEGER}       {   int bufsize = BUFFERSIZE;
                    char* fullType = malloc(sizeof(char) * bufsize);
                    strcpy(fullType, "INT_CONST\t");
                    if(yyleng >= bufsize){
                        bufsize += BUFFERSIZE;
                        fullType = realloc(fullType, bufsize);
                    }
                    strcat(fullType, yytext);
                    addToken(yylineno, ++tokenNum, yytext, fullType, "\t<-integer");
                }

{REAL}          {   int bufsize = BUFFERSIZE;
                    char* fullType = malloc(sizeof(char) * bufsize);
                    strcpy(fullType, "REAL_CONST\t");
                    if(yyleng >= bufsize){
                        bufsize += BUFFERSIZE;
                        fullType = realloc(fullType, bufsize);
                    }
                    strcat(fullType, yytext);
                    addToken(yylineno, ++tokenNum, yytext, fullType, "\t<-real");
                }

{ID}            {   int bufsize = BUFFERSIZE;
                    char* fullType = malloc(sizeof(char) * bufsize);
                    strcpy(fullType, "IDENT\t");
                    if(yyleng >= bufsize){
                        bufsize += BUFFERSIZE;
                        fullType = realloc(fullType, bufsize);
                    }
                    strcat(fullType, "\"");
                    strcat(fullType, yytext);
                    strcat(fullType, "\"");
                    addToken(yylineno, ++tokenNum, yytext, fullType, "\t<-char*");
                }

{WHITESPACE}    {}

{LINE_COMMENT}  {addToken(yylineno, ++tokenNum, "", "COMMENT\tLINE_COMMENT", "\t<-enumerated");}


"/*"    {
            nested++;
            startLine = yylineno;
            BEGIN(COMMENT);
        }

<COMMENT>[^/*]* 
<COMMENT>"/"+[^*/]*
<COMMENT>"/"+"*"    {   printf("Entered nested++\n");
                        nested++; }
<COMMENT>"*"+[^*/]*
<COMMENT>"*"+"/"    {   
                        nested--;
                        if(nested == 0){
                            
                            char start[sizeof(startLine)];
                            char end[sizeof(yylineno)];
                            snprintf(start, sizeof(start), "%d", startLine);
                            snprintf(end, sizeof(end), "%d", yylineno);
                            char* comm_content = strcat(start, "-");
                            comm_content = strcat(comm_content, end);
                            addToken(startLine, ++tokenNum, comm_content, "COMMENT\tBLOCK COMMENT", "<-enumerated");

                            BEGIN(INITIAL);
                        }
                    }
<COMMENT><<EOF>>    {
                        printf("Comment Error: UNCLOSED COMMENT BLOCK at EOF in line: %d \n",yylineno);
                        return -1;
                    }

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