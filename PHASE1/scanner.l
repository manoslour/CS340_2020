
%option noyywrap
%option yylineno

%{
    #include "scanner.h"
    unsigned int tokenNum = 0;
    int nested;
    int startLine;
    char* buffer;
    size_t stringSize;
    int flaf = 0;
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
N_EQUAL     "!="
INCR    "++"
DECR    "--"
GREATER     ">"
LESS    "<"
GREATER_EQ    ">="
LESS_EQ     "<="

LC_BRC  "{"
RC_BRC  "}"
L_BRCK  "["
R_BRCK  "]"
L_PAR   "("
R_PAR   ")"
SEMICOL ";"
COMMA   ","
COLON   ":"
DCOLON  "::"
DOT     "."
DDOT    ".."

ID          [a-zA-Z][a-zA-Z_0-9]*
DIGIT       [0-9]+
INTEGER     (0[xX][0-9A-Fa-f]+)|([0-9]+)
REAL        [0-9]*\.[0-9](e|E)?[0-9]*
NEWLINE         [\n]
WHITESPACE      [\r\t \v]+
LINE_COMMENT    "//".*

%x COMMENT
%x STRING

%%

            
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
<STRING>"\\".           {printf("Warning: Invalid Escape Character %s\n", yytext);}                        
<STRING>"\""            { 
                            
                            stringSize = stringSize + strlen(yytext);
                            buffer = realloc(buffer, stringSize);
                            strcat(buffer, yytext);
                            if(flag==1) {
                                int line=yylineno-1;
                                insertToken(line, ++tokenNum, buffer, "STRING", "");
                            }
                            else{
                               insertToken(yylineno, ++tokenNum, buffer, "STRING", ""); 
                            }
                            BEGIN(INITIAL);
<STRING><<EOF>>         {
                            printf("String Error: UNCLOSED STRING at EOF in line: %d \n",yylineno);
                            return -1;
                        }



"/*"    {
            nested = 1;
            startLine = yylineno;
            BEGIN(COMMENT);
        }
<COMMENT>[^/*]* 
<COMMENT>"/"+[^*/]*
<COMMENT>"/"+"*"    { nested++; }
<COMMENT>"*"+[^*/]*
<COMMENT>"*"+"/"    {   
                        nested--;
                        if(nested == 0){
                            
                            char start[100];
                            char end[100];
                            snprintf(start, sizeof(start), "%d", startLine);
                            snprintf(end, sizeof(end), "%d", yylineno);
                            char* comm_content = strcat(start, "-");
                            comm_content = strcat(comm_content, end);
                            insertToken(startLine, ++tokenNum, comm_content, "COMMENT", "BLOCK COMMENT");

                            BEGIN(INITIAL);
                        }
                    }
<COMMENT><<EOF>>    {
                        printf("Comment Error: UNCLOSED COMMENT BLOCK at EOF in line: %d \n",yylineno);
                        return -1;
                    }

{IF}        {insertToken(yylineno, ++tokenNum, yytext, "KEYWORD", "IF");}
{ELSE}      {insertToken(yylineno, ++tokenNum, yytext, "KEYWORD", "ELSE");}
{WHILE}     {insertToken(yylineno, ++tokenNum, yytext, "KEYWORD", "WHILE");}
{FOR}       {insertToken(yylineno, ++tokenNum, yytext, "KEYWORD", "FOR");}
{FUNCTION}  {insertToken(yylineno, ++tokenNum, yytext, "KEYWORD", "FUNCTION");}
{RETURN}    {insertToken(yylineno, ++tokenNum, yytext, "KEYWORD", "RETURN");}
{BREAK}     {insertToken(yylineno, ++tokenNum, yytext, "KEYWORD", "BREAK");}
{CONTINUE}  {insertToken(yylineno, ++tokenNum, yytext, "KEYWORD", "CONTINUE");}
{AND}       {insertToken(yylineno, ++tokenNum, yytext, "KEYWORD", "AND");}
{NOT}       {insertToken(yylineno, ++tokenNum, yytext, "KEYWORD", "NOT");}
{OR}        {insertToken(yylineno, ++tokenNum, yytext, "KEYWORD", "OR");}
{LOCAL}     {insertToken(yylineno, ++tokenNum, yytext, "KEYWORD", "LOCAL");}
{TRUE}      {insertToken(yylineno, ++tokenNum, yytext, "KEYWORD", "TRUE");}
{FALSE}     {insertToken(yylineno, ++tokenNum, yytext, "KEYWORD", "FALSE");}
{NIL}       {insertToken(yylineno, ++tokenNum, yytext, "KEYWORD", "NIL");}

{ASSIGN}    {insertToken(yylineno, ++tokenNum, yytext, "OPERATOR", "ASSIGN");}
{PLUS}      {insertToken(yylineno, ++tokenNum, yytext, "OPERATOR", "PLUS");}
{MINUS}     {insertToken(yylineno, ++tokenNum, yytext, "OPERATOR", "MINUS");}
{MULT}      {insertToken(yylineno, ++tokenNum, yytext, "OPERATOR", "MULT");}
{DIV}       {insertToken(yylineno, ++tokenNum, yytext, "OPERATOR", "DIV");}
{MOD}       {insertToken(yylineno, ++tokenNum, yytext, "OPERATOR", "MOD");}
{EQUAL}     {insertToken(yylineno, ++tokenNum, yytext, "OPERATOR", "EQUAL");}
{N_EQUAL}   {insertToken(yylineno, ++tokenNum, yytext, "OPERATOR", "N_EQUAL");}
{INCR}      {insertToken(yylineno, ++tokenNum, yytext, "OPERATOR", "INCR");}
{DECR}      {insertToken(yylineno, ++tokenNum, yytext, "OPERATOR", "DECR");}
{GREATER}   {insertToken(yylineno, ++tokenNum, yytext, "OPERATOR", "GREATER");}
{LESS}      {insertToken(yylineno, ++tokenNum, yytext, "OPERATOR", "LESS");}
{GREATER_EQ}    {insertToken(yylineno, ++tokenNum, yytext, "OPERATOR", "GREATER_EQ");}
{LESS_EQ}       {insertToken(yylineno, ++tokenNum, yytext, "OPERATOR", "LESS_EQ");}

{LC_BRC}        {insertToken(yylineno, ++tokenNum, yytext, "PUNCTUATION", "LC_BRC");}
{RC_BRC}        {insertToken(yylineno, ++tokenNum, yytext, "PUNCTUATION", "RC_BRC");}
{L_BRCK}        {insertToken(yylineno, ++tokenNum, yytext, "PUNCTUATION", "L_BRCK");}
{R_BRCK}        {insertToken(yylineno, ++tokenNum, yytext, "PUNCTUATION", "R_BRCK");}
{L_PAR}         {insertToken(yylineno, ++tokenNum, yytext, "PUNCTUATION", "L_PAR");}
{R_PAR}         {insertToken(yylineno, ++tokenNum, yytext, "PUNCTUATION", "R_PAR");}
{SEMICOL}       {insertToken(yylineno, ++tokenNum, yytext, "PUNCTUATION", "SEMICOL");}
{COMMA}         {insertToken(yylineno, ++tokenNum, yytext, "PUNCTUATION", "COMMA");}
{COLON}         {insertToken(yylineno, ++tokenNum, yytext, "PUNCTUATION", "COLON");}
{DCOLON}        {insertToken(yylineno, ++tokenNum, yytext, "PUNCTUATION", "DCOLON");}
{DOT}           {insertToken(yylineno, ++tokenNum, yytext, "PUNCTUATION", "DOT");}
{DDOT}          {insertToken(yylineno, ++tokenNum, yytext, "PUNCTUATION", "DDOT");}

{ID}        {insertToken(yylineno, ++tokenNum, yytext, "ID", "");}
{DIGIT}     {insertToken(yylineno, ++tokenNum, yytext, "DIGIT", "");}
{INTEGER}   {insertToken(yylineno, ++tokenNum, yytext, "INTEGER", "");}
{REAL}      {insertToken(yylineno, ++tokenNum, yytext, "REAL", "");}
{LINE_COMMENT}  {insertToken(yylineno, ++tokenNum, "\"\"", "COMMENT", "LINE COMMENT");}
{WHITESPACE}    {}
{NEWLINE}       {}
.           {printf("Undefined Character: %s in line: %d\n", yytext, yylineno);}
%%

void insertToken(unsigned int numline, unsigned int numToken, char* content, char* type, char* extraType){

    struct alpha_token_t *last;
    struct alpha_token_t *newNode = (struct alpha_token_t*)malloc(sizeof(struct alpha_token_t));
    char* tmpContent;
    tmpContent = strdup(content);

    newNode->numline = numline;
    newNode->numToken = numToken;
    newNode->content = tmpContent;
    newNode->type = type;
    newNode->extraType = extraType;
    newNode->next = NULL;

    last = HEAD;

    if(HEAD == NULL){
        HEAD = newNode;
    }
    else{

        while(last->next != NULL){
            last = last->next;
        }

        last->next = newNode;
    }
}

void printTokenList(){

    printf("\n------------ LEXICAL ANALYSIS ------------\n\n");

    struct alpha_token_t *tmp;
    tmp = HEAD;

    while(tmp != NULL){

        printf("%d:\t", tmp->numline);
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
    printTokenList();

    return 0;
}
