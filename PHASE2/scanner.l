%{
    #include "parser.h"
    #include "scanner.h"
    char *buffer;
    int flag = 0;
    int startLine, nestedStartLine;
    int nested = 0;
    int tokenNum = 0;
    size_t stringSize;
%}

%option noyywrap
%option yylineno

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
NEWLINE [\n]+
WHITESPACE [\t\r ]
ID [a-zA-Z][a-zA-Z_0-9]*
LINE_COMMENT "//".*

%x COMMENT
%x STRING

%%

{IF}        {return IF;}
{ELSE}      {return ELSE;}
{WHILE}     {return WHILE;}
{FOR}       {return FOR;}
{FUNCTION}  {return FUNCTION;}
{RETURN}    {return RETURN;}
{BREAK}     {return BREAK;}
{CONTINUE}  {return CONTINUE;}
{AND}       {return AND;}
{NOT}       {return NOT;}
{OR}        {return OR;}
{LOCAL}     {return LOCAL;}
{TRUE}      {return TRUE;}
{FALSE}     {return FALSE;}
{NIL}       {return NIL;}

{ASSIGN}    {return ASSIGN;}
{PLUS}      {return PLUS;}
{MINUS}     {return MINUS;}
{MULT}      {return MULT;}
{DIV}       {return DIV;}
{MOD}       {return MOD;}
{EQUAL}     {return EQUAL;}
{NOT_EQUAL}   {return NOT_EQUAL;}
{INCR}      {return INCR;}
{DECR}      {return DECR;}
{GREATER}   {return GREATER;}
{LESS}      {return LESS;}
{GREATER_EQ}    {return GREATER_EQ;}
{LESS_EQ}       {return LESS_EQ;}

{LCURLY_BR}        {return LCURLY_BR;}
{RCURLY_BR}        {return RCURLY_BR;}
{L_BR}        {return L_BR;}
{R_BR}        {return R_BR;}
{L_PAR}         {return L_PAR;}
{R_PAR}         {return R_PAR;}
{SEMICOL}       {return SEMICOL;}
{COMMA}         {return COMMA;}
{COLON}         {return COLON;}
{DCOLON}        {return DCOLON;}
{DOT}           {return DOT;}
{DDOT}          {return DDOT;}

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

{NEWLINE}       {return "\n";}

{LINE_COMMENT}  {addToken(yylineno, ++tokenNum, "", "COMMENT", "LINE_COMMENT", "<-enumerated");}


"/*"    {
            nested++;
            startLine = yylineno;
            BEGIN(COMMENT);
        }

<COMMENT>[^/*]* 
<COMMENT>"/"+[^*/]*
<COMMENT>"/"+"*"    {   nested++;
                        nestedStartLine = yylineno;
                    }
<COMMENT>"*"+[^*/]*
<COMMENT>"*"+"/"    {   
                        nested--;

                        char start[BUFFERSIZE];
                        char end[BUFFERSIZE];
                        char* comm_content = malloc(sizeof(char) * BUFFERSIZE);

                        if(nested == 0){

                            snprintf(start, sizeof(start), "%d", startLine);
                            snprintf(end, sizeof(end), "%d", yylineno);
                            
                            comm_content = strcat(start, "-");
                            comm_content = strcat(comm_content, end);

                            addToken(startLine, ++tokenNum, comm_content, "COMMENT", "BLOCK COMMENT", "<-enumerated");

                            BEGIN(INITIAL);
                            
                        }else{

                            snprintf(start, sizeof(start), "%d", nestedStartLine);
                            snprintf(end, sizeof(end), "%d", yylineno);

                            comm_content = strcat(start, "-");
                            comm_content = strcat(comm_content, end);

                            addToken(startLine, ++tokenNum, comm_content, "COMMENT", "NESTED COMMENT", "<-enumerated");
                        }
                    }
<COMMENT><<EOF>>    {
                        addError(RED"Error"RESET, "Unclosed COMMENT BLOCK at EOF", startLine);
                        //printf("Comment Error: UNCLOSED COMMENT BLOCK at EOF in line: %d \n",startLine);
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
<STRING>"\\".           {   
                            addError(YEL"Warning\x1B[0m \tInvalid Escape Character", yytext, yylineno);
                            //printf("Warning Invalid Escape Character %s\n", yytext);
                        }                        
<STRING>"\""            { 
                            
                            stringSize = stringSize + strlen(yytext);
                            buffer = realloc(buffer, stringSize);
                            strcat(buffer, yytext);
                            if(flag == 1) {
                                int line = yylineno-1;
                                addToken(line, ++tokenNum, buffer, "STRING", buffer, "<-char*");
                            }
                            else{
                               addToken(yylineno, ++tokenNum, buffer, "STRING", buffer, "<-char*"); 
                            }
                            BEGIN(INITIAL);
                        }
<STRING><<EOF>>         {
                            addError(RED"Error"RESET, "Unclosed STRING at EOF", yylineno);
                            //printf("String Error: UNCLOSED STRING at EOF in line: %d \n",yylineno);
                            return -1;
                        }

.               {   addError(YEL"Warning\x1B[0m \tUndefined character", yytext, yylineno);
                    //printf("Undefined Character: %s in line: %d\n", yytext, yylineno);
                }

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

void addError(char *output, char *content, unsigned int numLine){
    struct errorToken *last;
    struct errorToken *newNode = (struct errorToken *)malloc(sizeof(struct errorToken));
    char *tmpOutput = strdup(output);
    char *tmpContent = strdup(content);

    newNode->output = tmpOutput;
    newNode->content = tmpContent;
    newNode->numLine = numLine;
    newNode->next = NULL;

    last = ERROR_HEAD;

    if(ERROR_HEAD == NULL){
        ERROR_HEAD = newNode;
    }else{
        while(last->next != NULL){
            last = last->next;
        }
        last->next = newNode;
    }
}

void printTokenList(){

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

    printf("\n");
}

void printErrorList(){

    struct errorToken *tmp = ERROR_HEAD;
    printf("\n------------------\tERRORS - WARNINGS\t------------------\n\n");

    while(tmp != NULL){
        printf("%d:\t", tmp->numLine);
        printf("%s\t", tmp->output);
        printf("%s\n", tmp->content);

        tmp = tmp->next;
    }
    printf("\n");
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
    printTokenList();
    printErrorList();

    return 0;
}