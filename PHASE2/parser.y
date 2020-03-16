%{

#include <stdio.h>
#include "parser.h"

%}

%union{}
%token

%start program


%right ASSIGN
%left OR
%left AND
%left EQUAL,NOT_EQUAL
%nonassoc GREATER,GREATER_EQ,LESS,LESS_EQ
%left PLUS,MINUS
%left MULT,DIV,MOD
%right NOT,INCR,DECR,UMINUS
%left DOT,DDOT
%left L_BR,R_BR
%left L_PAR,R_PAR

%%

program:	stmt
			|
			;
stmt:		expr SEMICOLON
			|ifstmt
			|whilestmt
			|forstmt
			|break SEMICOLON
			|continue SEMICOLON
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

term:		L_PAR expr R_PER
			|MINUS expr
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

lvalue:		ID
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

methodcall:	DDOT ID L_PAR elist R_PAR 

elist:		

objectdef:	

indexed:

indexelem:	LCURLY_BR expr COLON RCURLY_BR
			;

block:

funcdef:








%%

int main(int argc, char **argv) {
	yyparse();
	return 0;
}