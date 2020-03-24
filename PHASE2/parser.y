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

program:	stmts	{printf("promgram: stmts at line %d --> %s\n", yylineno, yytext);}
			|
			;

stmts:		stmt	{printf("stmts: stmt at line %d --> %s\n", yylineno, yytext);}
			|stmts stmt		{printf("stmts: stmts stmt at line %d --> %s\n", yylineno, yytext);}	
			;

stmt:		expr SEMICOLON		{printf("stmt: expr SEMICOLON at line %d --> %s\n", yylineno, yytext);}
			|ifstmt		{printf("stmt: ifstmt at line %d --> %s\n", yylineno, yytext);}
			|whilestmt		{printf("stmt: whilestmt at line %d --> %s\n", yylineno, yytext);}
			|forstmt		{printf("stmt: forstmt at line %d --> %s\n", yylineno, yytext);}
			|BREAK SEMICOLON		{printf("stmt: BREAK SEMICOLON at line %d --> %s\n", yylineno, yytext);}
			|CONTINUE SEMICOLON		{printf("stmt: CONTINUE SEMICOLON at line %d --> %s\n", yylineno, yytext);}
			|block		{printf("stmt: block at line %d --> %s\n", yylineno, yytext);}
			|funcdef		{printf("stmt: funcdef at line %d --> %s\n", yylineno, yytext);}
			|SEMICOLON		{printf("stmt: SEMICOLON at line %d --> %s\n", yylineno, yytext);}
			;

expr:		assignexpr	{printf("expr: assignexpr at line %d --> %s\n", yylineno, yytext);}
			|expr op expr		{printf("expr: expr op expr at line %d --> %s\n", yylineno, yytext);}
			|term		{printf("expr: term at line %d --> %s\n", yylineno, yytext);}
			;

op:			PLUS		{printf("op: PLUS at line %d --> %s\n", yylineno, yytext);}
			|MINUS		{printf("op: MINUS at line %d --> %s\n", yylineno, yytext);}
			|MULT		{printf("op: MULT at line %d --> %s\n", yylineno, yytext);}
			|DIV		{printf("op: DIV at line %d --> %s\n", yylineno, yytext);}
			|MOD		{printf("op: MOD at line %d --> %s\n", yylineno, yytext);}
			|GREATER		{printf("op: GREATER at line %d --> %s\n", yylineno, yytext);}
			|GREATER_EQ		{printf("op: GREATER_EQ at line %d --> %s\n", yylineno, yytext);}
			|LESS		{printf("op: LESS at line %d --> %s\n", yylineno, yytext);}
			|LESS_EQ		{printf("op: LESS_EQ at line %d --> %s\n", yylineno, yytext);}
			|EQUAL		{printf("op: EQUAL at line %d --> %s\n", yylineno, yytext);}
			|NOT_EQUAL		{printf("op: NOT_EQUAL at line %d --> %s\n", yylineno, yytext);}
			|AND		{printf("op: AND at line %d --> %s\n", yylineno, yytext);}
			|OR		{printf("op: OR at line %d --> %s\n", yylineno, yytext);}
			;

term:		L_PAR expr R_PAR		{printf("term: L_PAR expr R_PAR at line %d --> %s\n", yylineno, yytext);}
			|MINUS expr %prec UMINUS		{printf("term: MINUS expr at line %d --> %s\n", yylineno, yytext);}
			|NOT expr		{printf("term: NOT expr at line %d --> %s\n", yylineno, yytext);}
			|INCR lvalue		{printf("term: INCR lvalue at line %d --> %s\n", yylineno, yytext);}
			|lvalue INCR		{printf("term: lvalue INCR at line %d --> %s\n", yylineno, yytext);}
			|DECR lvalue		{printf("term: DECR lvalue at line %d --> %s\n", yylineno, yytext);}
			|lvalue DECR		{printf("term: lvalue DECR at line %d --> %s\n", yylineno, yytext);}
			|primary		{printf("term: primary at line %d --> %s\n", yylineno, yytext);}
			;

assignexpr:	lvalue ASSIGN expr		{printf("assignexpr: lvalue ASSIGN expr at line %d --> %s\n", yylineno, yytext);}
			;

primary:	lvalue		{printf("primary: lvalue at line %d --> %s\n", yylineno, yytext);}
			|call		{printf("primary: call at line %d --> %s\n", yylineno, yytext);}
			|objectdef		{printf("primary: objectdef at line %d --> %s\n", yylineno, yytext);}
			|L_PAR funcdef R_PAR		{printf("primary: L_PAR funcdef R_PAR at line %d --> %s\n", yylineno, yytext);}
			|const		{printf("primary: const at line %d --> %s\n", yylineno, yytext);}
			;

lvalue:		ID				{printf("lvalue: ID at line %d --> %s\n", yylineno, yytext);}
			|LOCAL ID		{printf("lvalue: LOCAL ID at line %d --> %s\n", yylineno, yytext);}
			|DCOLON ID		{printf("lvalue: DCOLON ID at line %d --> %s\n", yylineno, yytext);}
			|member		{printf("lvalue: member at line %d --> %s\n", yylineno, yytext);}
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

block:		LCURLY_BR RCURLY_BR
			|LCURLY_BR stmts  RCURLY_BR
			;

funcdef:	FUNCTION L_PAR idlist R_PAR block
			|FUNCTION ID L_PAR idlist R_PAR block
			;

const:		REAL
			|INTEGER
			|STRING
			|NIL
			|TRUE
			|FALSE
			;

idlist:		ID
			|idlist COMMA ID
			|
			;

ifstmt:	IF L_PAR expr R_PAR stmt
			|IF L_PAR expr R_PAR stmt ELSE stmt
			;


whilestmt:	WHILE L_PAR expr R_PAR stmt
			;

forstmt:  	FOR L_PAR elist SEMICOLON expr SEMICOLON elist R_PAR stmt
			;

returnstmt:	RETURN SEMICOLON
			|RETURN expr SEMICOLON
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