%{
    #include "parser.h"
    #include "symTable.h"
    #include <stdlib.h>
    #include <string.h>

    #define BUFFERSIZE 256

    extern YYSTYPE yylval;

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
AND     "&&"
NOT     "not"
OR      "||"
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
SEMICOLON ";"
COMMA   ","
COLON   ":"
DCOLON  "::"
DOT     "."
DDOT    ".."

INTEGER [0-9]+
REAL [0-9]+\.[0-9]+
NEWLINE [\n]+
WHITESPACE [\t\r ]+
ID [a-zA-Z][a-zA-Z_0-9]*
LINE_COMMENT "//".*

%x COMMENT
%x FLEX_STRING

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
{SEMICOLON}       {return SEMICOLON;}
{COMMA}         {return COMMA;}
{COLON}         {return COLON;}
{DCOLON}        {return DCOLON;}
{DOT}           {return DOT;}
{DDOT}          {return DDOT;}

{INTEGER}       {
                    yylval.intValue = atoi(yytext);
                    return INTEGER;
                }

{REAL}          {
                    yylval.realValue = atof(yytext);
                    return REAL;
                }

{ID}            {
                    yylval.stringValue = strdup(yytext);
                    return ID;
                }

{WHITESPACE}    {}
{NEWLINE}       {}
{LINE_COMMENT}  {}


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

                            //addToken(startLine, ++tokenNum, comm_content, "COMMENT", "BLOCK COMMENT", "<-enumerated");

                            BEGIN(INITIAL);

                        }else{

                            snprintf(start, sizeof(start), "%d", nestedStartLine);
                            snprintf(end, sizeof(end), "%d", yylineno);

                            comm_content = strcat(start, "-");
                            comm_content = strcat(comm_content, end);

                            //addToken(startLine, ++tokenNum, comm_content, "COMMENT", "NESTED COMMENT", "<-enumerated");
                        }
                    }
<COMMENT><<EOF>>    {
                        addError(RED"Error"RESET, "Unclosed COMMENT BLOCK at EOF", startLine);
                        //return -1;
                    }

"\""   {
            stringSize = strlen(yytext);
            buffer = (char *)malloc(stringSize * sizeof(char));
            strcpy(buffer, yytext);

            BEGIN(FLEX_STRING);
        }

<FLEX_STRING>[^\n\t\\\"]*    {
                            stringSize = stringSize + strlen(yytext);
                            buffer = realloc(buffer, stringSize);
                            strcat(buffer, yytext);
                        }
<FLEX_STRING>"\n"            {
                            flag=1;
                            stringSize = stringSize + strlen(yytext);
                            buffer = realloc(buffer, stringSize);
                            strcat(buffer, "\n");

                        }
<FLEX_STRING>"\\\""          {
                            stringSize = stringSize + strlen(yytext);
                            buffer = realloc(buffer, stringSize);
                            strcat(buffer, "\"");
                        }
<FLEX_STRING>"\\n"           {
                            stringSize = stringSize + strlen(yytext);
                            buffer = realloc(buffer, stringSize);
                            strcat(buffer, "\n");

                        }
<FLEX_STRING>"\\t"           {
                            stringSize = stringSize + strlen(yytext);
                            buffer = realloc(buffer, stringSize);
                            strcat(buffer, "\t");
                        }
<FLEX_STRING>"\\\\"          {
                            stringSize = stringSize + strlen(yytext);
                            buffer = realloc(buffer, stringSize);
                            strcat(buffer, "\\");
                        }
<FLEX_STRING>"\\".           {   addError(YEL"Warning\x1B[0m \tInvalid Escape Character", yytext, yylineno);}

<FLEX_STRING>"\""            {
                            stringSize = stringSize + strlen(yytext);
                            buffer = realloc(buffer, stringSize);
                            strcat(buffer, yytext);

                            yylval.stringValue = strdup(buffer);

                            BEGIN(INITIAL);

                            return STRING;
                        }
<FLEX_STRING><<EOF>>         {
                            addError(RED"Error"RESET, "Unclosed STRING at EOF", yylineno);
                            //return -1;
                        }

.                       {   addError(YEL"Warning\x1B[0m \tUndefined character", yytext, yylineno);}

%%
