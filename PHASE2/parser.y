%{
    #include <stdio.h>
	#include "symTable.h"

    int yyerror(char* yaccProvidedMessage);
    extern int yylex(void);

    extern int yylineno;
    extern char *yytext;
    extern FILE *yyin;
%}

%defines

%union{
    int intValue;
    double realValue;
    char* stringValue;
}

%token <intValue> INTEGER
%token <realValue> REAL
%token <stringValue> ID
%token <stringValue> STRING
%token IF ELSE WHILE FOR FUNCTION RETURN BREAK CONTINUE AND NOT OR LOCAL TRUE FALSE NIL
%token ASSIGN PLUS MINUS MULT DIV MOD EQUAL NOT_EQUAL INCR DECR GREATER LESS GREATER_EQ LESS_EQ
%token LCURLY_BR RCURLY_BR L_BR R_BR L_PAR R_PAR SEMICOLON COMMA COLON DCOLON DOT DDOT

%right ASSIGN
%left OR
%left AND
%nonassoc EQUAL NOT_EQUAL 
%nonassoc GREATER GREATER_EQ LESS LESS_EQ
%left PLUS MINUS
%left MULT DIV MOD
%right NOT INCR DECR UMINUS
%left DOT DDOT
%left L_BR R_BR
%left L_PAR R_PAR

%start program

%%

program:	stmts {printf("promgram: stmts at line %d --> %s\n", yylineno, yytext);}
			|
			;

stmts:		stmt
			|stmts stmt
			;

stmt:		expr SEMICOLON
			|ifstmt
			|whilestmt
			|forstmt
			|BREAK SEMICOLON
			|CONTINUE SEMICOLON
			|block
			|funcdef
			|SEMICOLON
			;

expr:		assignexpr	
			|expr op expr
			|term
			;

op:			PLUS
			|MINUS
			|MULT
			|DIV
			|MOD
			|GREATER
			|GREATER_EQ
			|LESS
			|LESS_EQ
			|EQUAL
			|NOT_EQUAL
			|AND
			|OR
			;

term:		L_PAR expr R_PAR
			|MINUS expr %prec UMINUS
			|NOT expr
			|INCR lvalue
			|lvalue INCR
			|DECR lvalue
			|lvalue DECR
			|primary
			;

assignexpr:	lvalue ASSIGN expr
			;

primary:	lvalue
			|call
			|objectdef
			|L_PAR funcdef R_PAR
			|const
			;

lvalue:		ID				{printf("Entered ID\n");}
			|LOCAL ID
			|DCOLON ID
			|member
			;

member:		lvalue DOT ID
			|lvalue L_BR expr R_BR
			|call DOT ID
			|call L_BR expr R_BR
			;

call:		call L_PAR elist R_PAR
			|lvalue callsuffix
			|L_PAR funcdef R_PAR L_PAR elist R_PAR
			;

callsuffix:	normcall
			|methodcall
			;

normcall:	L_PAR elist R_PAR
			;

methodcall:		DDOT ID L_PAR elist R_PAR
				;

elist:		expr
			|elist COMMA expr
			|
			;

objectdef:	L_BR R_BR
			|L_BR elist R_BR
			|L_BR indexed R_BR
			;

indexed:	indexedelem
			|indexed COMMA indexedelem
			| 
			;

indexedelem:	LCURLY_BR expr COLON RCURLY_BR
			;

block:		LCURLY_BR RCURLY_BR 			{printf("block: LCURLY_BR RCURLY_BR  at line %d --> %s\n", yylineno, yytext);}		
			|LCURLY_BR stmts  RCURLY_BR		{printf("block: LCURLY_BR stmts RCURLY_BR  at line %d --> %s\n", yylineno, yytext);}
			;

funcdef:	FUNCTION L_PAR idlist R_PAR block  		{printf("funcdef: FUNCTION L_PAR idlist R_PAR 
													block at line %d --> %s\n", yylineno, yytext);}
			|FUNCTION ID L_PAR idlist R_PAR block 	{printf("funcdef: FUNCTION ID L_PAR idlist R_PAR 
													block  at line %d --> %s\n", yylineno, yytext);}
			;

const:		REAL 		{printf("const: REAL at line %d --> %s\n", yylineno, yytext);}
			|INTEGER	{printf("const: INTEGER at line %d --> %s\n", yylineno, yytext);}
			|STRING 	{printf("const: STRING at line %d --> %s\n", yylineno, yytext);}
			|NIL		{printf("const: NIL at line %d --> %s\n", yylineno, yytext);}
			|TRUE		{printf("const: TRUE at line %d --> %s\n", yylineno, yytext);}
			|FALSE 		{printf("const: FALSE at line %d --> %s\n", yylineno, yytext);}
			;

idlist:		ID 					{printf("idlist: ID at line %d --> %s\n", yylineno, yytext);}
			|idlist COMMA ID 	{printf("idlist: COMMA ID at line %d --> %s\n", yylineno, yytext);}
			|
			;

ifstmt:		IF L_PAR expr R_PAR stmt 		{printf("ifstmt: IF L_PAR expr R_PAR stmt 
											at line %d --> %s\n", yylineno, yytext);}
			|IF L_PAR expr R_PAR stmt ELSE stmt 	{printf("ifstmt: IF L_PAR expr R_PAR stmt ELSE stmt
											at line %d --> %s\n", yylineno, yytext);}
			;


whilestmt:	WHILE L_PAR expr R_PAR stmt 	{printf("whilestmt: WHILE L_PAR expr R_PAR stmt 
											at line %d --> %s\n", yylineno, yytext);}
			;

forstmt:  	FOR L_PAR elist SEMICOLON expr SEMICOLON elist R_PAR stmt {printf("forstm: 
							FOR L_PAR elist SEMICOLON expr SEMICOLON elist R_PAR stmt at line %d --> %s\n", yylineno, yytext);}
			;

returnstmt:	RETURN SEMICOLON		{printf("returnstmt: RETURN SEMICOLON at line %d --> %s\n", yylineno, yytext);}
			|RETURN expr SEMICOLON	{printf("returnstmt: RETURN expr SEMICOLON at line %d --> %s\n", yylineno, yytext);}
			;


%%

int yyerror(char* yaccProvidedMessage){
    fprintf(stderr, "%s: at line %d, before token: %s\n", yaccProvidedMessage, yylineno, yytext);
    fprintf(stderr, "INPUT NOT VALID\n");
}

int main(int argc, char** argv){
    if(argc > 1){
        if(!(yyin = fopen(argv[1], "r"))){
            fprintf(stderr, "Cannot read file: %s\n", argv[1]);
            return 1;
        }
    }
    else{
        yyin = stdin;
    }

    yyparse();
    return 0;
}