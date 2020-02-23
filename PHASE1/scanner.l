%option noyywrap
%option yylineno

%{
    #include "scanner.h"
    char *buffer;
    int flag = 0;
    int startLine;
    int nested = 0;
    int tokenNum = 0;
    size_t stringSize;
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
%x STRING

%%

{IF}        {addToken(yylineno, ++tokenNum, yytext, "KEYWORD", "IF", "<-enumerated");}
{ELSE}      {addToken(yylineno, ++tokenNum, yytext, "KEYWORD", "ELSE", "<-enumerated");}
{WHILE}     {addToken(yylineno, ++tokenNum, yytext, "KEYWORD", "WHILE", "<-enumerated");}
{FOR}       {addToken(yylineno, ++tokenNum, yytext, "KEYWORD", "FOR", "<-enumerated");}
{FUNCTION}  {addToken(yylineno, ++tokenNum, yytext, "KEYWORD", "FUNCTION", "<-enumerated");}
{RETURN}    {addToken(yylineno, ++tokenNum, yytext, "KEYWORD", "RETURN", "<-enumerated");}
{BREAK}     {addToken(yylineno, ++tokenNum, yytext, "KEYWORD", "BREAK", "<-enumerated");}
{CONTINUE}  {addToken(yylineno, ++tokenNum, yytext, "KEYWORD", "CONTINUE", "<-enumerated");}
{AND}       {addToken(yylineno, ++tokenNum, yytext, "KEYWORD", "AND", "<-enumerated");}
{NOT}       {addToken(yylineno, ++tokenNum, yytext, "KEYWORD", "NOT", "<-enumerated");}
{OR}        {addToken(yylineno, ++tokenNum, yytext, "KEYWORD", "OR", "<-enumerated");}
{LOCAL}     {addToken(yylineno, ++tokenNum, yytext, "KEYWORD", "LOCAL", "<-enumerated");}
{TRUE}      {addToken(yylineno, ++tokenNum, yytext, "KEYWORD", "TRUE", "<-enumerated");}
{FALSE}     {addToken(yylineno, ++tokenNum, yytext, "KEYWORD", "FALSE", "<-enumerated");}
{NIL}       {addToken(yylineno, ++tokenNum, yytext, "KEYWORD", "NIL", "<-enumerated");}

{ASSIGN}    {addToken(yylineno, ++tokenNum, yytext, "OPERATOR", "ASSIGN", "<-enumerated");}
{PLUS}      {addToken(yylineno, ++tokenNum, yytext, "OPERATOR", "PLUS", "<-enumerated");}
{MINUS}     {addToken(yylineno, ++tokenNum, yytext, "OPERATOR", "MINUS", "<-enumerated");}
{MULT}      {addToken(yylineno, ++tokenNum, yytext, "OPERATOR", "MULT", "<-enumerated");}
{DIV}       {addToken(yylineno, ++tokenNum, yytext, "OPERATOR", "DIV", "<-enumerated");}
{MOD}       {addToken(yylineno, ++tokenNum, yytext, "OPERATOR", "MOD", "<-enumerated");}
{EQUAL}     {addToken(yylineno, ++tokenNum, yytext, "OPERATOR", "EQUAL", "<-enumerated");}
{NOT_EQUAL}   {addToken(yylineno, ++tokenNum, yytext, "OPERATOR", "NOT_EQUAL", "<-enumerated");}
{INCR}      {addToken(yylineno, ++tokenNum, yytext, "OPERATOR", "INCR", "<-enumerated");}
{DECR}      {addToken(yylineno, ++tokenNum, yytext, "OPERATOR", "DECR", "<-enumerated");}
{GREATER}   {addToken(yylineno, ++tokenNum, yytext, "OPERATOR", "GREATER", "<-enumerated");}
{LESS}      {addToken(yylineno, ++tokenNum, yytext, "OPERATOR", "LESS", "<-enumerated");}
{GREATER_EQ}    {addToken(yylineno, ++tokenNum, yytext, "OPERATOR", "GREATER_EQ", "<-enumerated");}
{LESS_EQ}       {addToken(yylineno, ++tokenNum, yytext, "OPERATOR", "LESS_EQ", "<-enumerated");}

{LCURLY_BR}        {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION", "LCURLY_BRC", "<-enumerated");}
{RCURLY_BR}        {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION", "RCURLY_BRC", "<-enumerated");}
{L_BR}        {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION", "L_BRCK", "<-enumerated");}
{R_BR}        {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION", "R_BRCK", "<-enumerated");}
{L_PAR}         {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION", "L_PAR", "<-enumerated");}
{R_PAR}         {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION", "R_PAR", "<-enumerated");}
{SEMICOL}       {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION", "SEMICOL", "<-enumerated");}
{COMMA}         {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION", "COMMA", "<-enumerated");}
{COLON}         {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION", "COLON", "<-enumerated");}
{DCOLON}        {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION", "DCOLON", "<-enumerated");}
{DOT}           {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION", "DOT", "<-enumerated");}
{DDOT}          {addToken(yylineno, ++tokenNum, yytext, "PUNCTUATION", "DDOT", "<-enumerated");}

{INTEGER}       {addToken(yylineno, ++tokenNum, yytext, "INT_CONST", yytext, "<-integer");}

{REAL}          {addToken(yylineno, ++tokenNum, yytext, "REAL_CONST", yytext, "<-real");}

{ID}            {   int bufsize = BUFFERSIZE;
                    char* fullType = malloc(sizeof(char) * bufsize);
                    if(yyleng >= bufsize){
                        bufsize += BUFFERSIZE;
                        fullType = realloc(fullType, bufsize);
                    }
                    strcat(fullType, "\"");
                    strcat(fullType, yytext);
                    strcat(fullType, "\"");
                    addToken(yylineno, ++tokenNum, yytext, "ID", fullType, "<-char*");
                }

{WHITESPACE}    {}

{LINE_COMMENT}  {addToken(yylineno, ++tokenNum, "", "COMMENT", "LINE_COMMENT", "<-enumerated");}


"/*"    {
            nested++;
            startLine = yylineno;
            BEGIN(COMMENT);
        }

<COMMENT>[^/*]* 
<COMMENT>"/"+[^*/]*
<COMMENT>"/"+"*"    {   nested++; }
<COMMENT>"*"+[^*/]*
<COMMENT>"*"+"/"    {   
                        nested--;
                        if(nested == 0){
                            char start[250];
                            char end[250];
                            snprintf(start, sizeof(start), "%d", startLine);
                            snprintf(end, sizeof(end), "%d", yylineno);
                            char* comm_content = malloc(sizeof(char) * BUFFERSIZE);
                            comm_content = strcat(start, "-");
                            comm_content = strcat(comm_content, end);
                            addToken(startLine, ++tokenNum, comm_content, "COMMENT", "BLOCK COMMENT", "<-enumerated");

                            BEGIN(INITIAL);
                        }
                    }
<COMMENT><<EOF>>    {
                        printf("Comment Error: UNCLOSED COMMENT BLOCK at EOF in line: %d \n",yylineno);
                        return -1;
                    }

"\""   {    
            stringSize = strlen(yytext);
            buffer = (char *)malloc(stringSize * sizeof(char));
            strcpy(buffer, yytext);

            BEGIN(STRING);
        }

<STRING>[^\n\t\\\"]*    {
                            stringSize = stringSize + strlen(yytext);
                            buffer = realloc(buffer, stringSize);
                            strcat(buffer, yytext);
                        }
<STRING>"\n"            {   
                            
                            flag=1;
                            stringSize = stringSize + strlen(yytext);
                            buffer = realloc(buffer, stringSize);
                            strcat(buffer, "\n"); 

                        }
<STRING>"\\\""          {
                            stringSize = stringSize + strlen(yytext);
                            buffer = realloc(buffer, stringSize);
                            strcat(buffer, "\"");
                        }
<STRING>"\\n"           {   
                            stringSize = stringSize + strlen(yytext);
                            buffer = realloc(buffer, stringSize);
                            strcat(buffer, "\n"); 

                        }        
<STRING>"\\t"           {
                            stringSize = stringSize + strlen(yytext);
                            buffer = realloc(buffer, stringSize);
                            strcat(buffer, "\t");
                        }
<STRING>"\\\\"          {
                            stringSize = stringSize + strlen(yytext);
                            buffer = realloc(buffer, stringSize);
                            strcat(buffer, "\\");
                        }
<STRING>"\\".           {   printf("Warning: Invalid Escape Character %s\n", yytext);}                        
<STRING>"\""            { 
                            
                            stringSize = stringSize + strlen(yytext);
                            buffer = realloc(buffer, stringSize);
                            strcat(buffer, yytext);
                            if(flag==1) {
                                int line=yylineno-1;
                                addToken(line, ++tokenNum, buffer, "STRING", buffer, "<-char*");
                            }
                            else{
                               addToken(yylineno, ++tokenNum, buffer, "STRING", buffer, "<-char*"); 
                            }
                            BEGIN(INITIAL);
                        }
<STRING><<EOF>>         {
                            printf("String Error: UNCLOSED STRING at EOF in line: %d \n",yylineno);
                            return -1;
                        }

.               {printf("Undefined Character: %s in line: %d\n", yytext, yylineno);}

%%

void addToken(unsigned int numLine, unsigned int numToken, char *content, char *type, char *extraType, char *subType){

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
    newToken->subType = subType;
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
        if(!strcmp(tmp->type, "STRING")) printf("%s", tmp->content);    
        else {
            printf("\"");
            printf("%s", tmp->content);
            printf("\"\t");
        }
        printf("%s\t", tmp->type);
        printf("%s\t", tmp->extraType);
        printf("%s\n", tmp->subType);

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
