%{
    #include <stdio.h>
	#include "symTable.h"

    int yyerror(char* yaccProvidedMessage);
    extern int yylex(void);

    extern int yylineno;
    extern char *yytext;
    extern FILE *yyin;
    unsigned int currscope = 0;
	unsigned int inFunc = 0;
	struct SymbolTableEntry *tmp;
	unsigned int funcPrefix = 0;
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

program:	stmtlist	{printf("promgram: stmtlist at line %d --> %s\n", yylineno, yytext);}
			|
			;

stmtlist:		stmt	{printf("stmtlist: stmt at line %d --> %s\n", yylineno, yytext);}
			|stmtlist stmt		{printf("stmtlist: stmtlist stmt at line %d --> %s\n", yylineno, yytext);}	
			;

stmt:		expr SEMICOLON		{printf("stmt: expr SEMICOLON at line %d --> %s\n", yylineno, yytext);}
			|ifstmt		 {printf("stmt: ifstmt at line %d --> %s\n", yylineno, yytext);}
			|whilestmt		{printf("stmt: whilestmt at line %d --> %s\n", yylineno, yytext);}
			|forstmt		{printf("stmt: forstmt at line %d --> %s\n", yylineno, yytext);}
			|returnstmt		{printf("stmt: returnstmt at line %d --> %s\n", yylineno, yytext);}
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

lvalue:		ID				{
								printf("lvalue: ID at line %d --> %s\n", yylineno, yytext);

								char *tmp;
								int result;
								enum SymbolType type;
								
								result = generalLookUp(yylval.stringValue, currscope);

								if(result == 1)
									printf("Variable found\n");
								else if(result == 2)
									printf("Userfunc found\n");
								else if(result == 3)
									printf("Libfunc found\n");
								else{
									if(currscope == 0)
										type = Global;
									else
										type = Local;
									printf("Put %s to SymbolTable\n", yylval.stringValue);
									hashInsert(yylval.stringValue, yylineno, type, currscope);
								}
							}

			|LOCAL ID		{
								printf("lvalue: LOCAL ID at line %d --> %s\n", yylineno, yytext);

								if( scopeLookUp(yytext, currscope) == 1){
									printf("Ok, found locally\n");
								}
								else if(scopeLookUp(yytext, 0) == 2 || scopeLookUp(yytext, 0) == 3){
									if(currscope == 0)
										printf("Ok, found in scope 0\n");
									else
										addError("Error, collision with library function", yytext, yylineno);
								}
								else{
									if(currscope == 0){
										printf("New Global var in scope %d\n", currscope);
										hashInsert(yytext, yylineno, Global, currscope);
									}
									else{
										printf("New Local var in scope %d\n", currscope);
										hashInsert(yytext, yylineno, Local, currscope);
									}
								}		
							}

			|DCOLON ID		{
								if(scopeLookUp(yytext, 0) == 1) 
									printf("Global var %s found in line %d\n", yytext, yylineno); 
								else if(scopeLookUp(yytext, 0) == 3)
									printf("Libfunc %s found in line %d", yytext, yylineno);
								else
									addError("Error, Global variable not found", yytext, yylineno);
							}
			|member			{printf("lvalue: member at line %d --> %s\n", yylineno, yytext);}
			;

member:		lvalue DOT ID 							{printf("member: lvalue.ID at line %d --> %s\n", yylineno, yytext);}
			|lvalue L_BR expr R_BR 					{printf("member: lvalue[expr] at line %d --> %s\n", yylineno, yytext);}
			|call DOT ID 							{printf("member: call.ID at line %d --> %s\n", yylineno, yytext);}
			|call L_BR expr R_BR 					{printf("member: lvalue[expr] at line %d --> %s\n", yylineno, yytext);}
			;

call:		call L_PAR elist R_PAR					{printf("call: (elist) at line %d --> %s\n", yylineno, yytext);}
			|lvalue callsuffix						{printf("call: lvalue callsuffix at line %d --> %s\n", yylineno, yytext);}
			|L_PAR funcdef R_PAR L_PAR elist R_PAR	{printf("call: (funcdef) (elist) at line %d --> %s\n", yylineno, yytext);}
			;

callsuffix:	normcall 						{printf("callsuffix: normcall at line %d --> %s\n", yylineno, yytext);}
			|methodcall						{printf("callsuffix: methodcall at line %d --> %s\n", yylineno, yytext);}
			;

normcall:	L_PAR elist R_PAR 				{printf("normcall: (elist) at line %d --> %s\n", yylineno, yytext);}
			;

methodcall:		DDOT ID L_PAR elist R_PAR 	{printf("methodcall: ..ID (elist) at line %d --> %s\n", yylineno, yytext);}
				;

elist:		expr						{printf("elist: expr at line %d --> %s\n", yylineno, yytext);}
			|expr COMMA elist			{printf("elist: elist comma expr at line %d --> %s\n", yylineno, yytext);}
			|
			;

objectdef:	L_BR R_BR 					{printf("objectdef: [] at line %d --> %s\n", yylineno, yytext);}
			|L_BR elist R_BR 			{printf("objectdef: [elist] at line %d --> %s\n", yylineno, yytext);}
			|L_BR indexed R_BR 			{printf("objectdef: [indexed] at line %d --> %s\n", yylineno, yytext);}
			;

indexed:	indexedelem					{printf("indexed: indexelem at line %d --> %s\n", yylineno, yytext);}
			|indexedelem COMMA indexed 	{printf("indexed: indexed comma indexelem at line %d --> %s\n", yylineno, yytext);}
			|
			;

indexedelem:	LCURLY_BR expr COLON expr RCURLY_BR	{printf("indexelem: {expr:expr} at line %d --> %s\n", yylineno, yytext);}
			;

block:		LCURLY_BR	{if (inFunc == 0) currscope++;} 
			RCURLY_BR 	{	
							hideScope(currscope);
							if(inFunc == 1) inFunc = 0;
							currscope--;
						}		
			|LCURLY_BR	{if (inFunc == 0) currscope++;} 
			stmtlist  
			RCURLY_BR	{
							hideScope(currscope);
							if(inFunc == 1) inFunc = 0;
							currscope--;
						}
			;

funcdef:	FUNCTION
					{	
						printf("funcdef: FUNCTION at line %d --> %s\n", yylineno, yytext);
						tmp = hashInsert(generateName(funcPrefix),yylineno,Userfunc,currscope);
						funcPrefix++;
					}
			L_PAR 	{
						printf("funcdef: FUNCTION L_PAR at line %d --> %s\n", yylineno, yytext);
						currscope++;
						inFunc = 1;
					}
			idlist 	{printf("funcdef: FUNCTION L_PAR idlist at line %d --> %s\n", yylineno, yytext);}
			R_PAR block  	{printf("funcdef: FUNCTION L_PAR idlist R_PAR block at line %d --> %s\n", yylineno, yytext);}
			|FUNCTION ID 	{
								printf("funcdef: FUNCTION ID at line %d --> %s\n", yylineno, yytext);
								int found = scopeLookUp(yytext,currscope);

								if(found == 1){
									addError("Error, variable already exists",yytext,yylineno);
								}
								else if(found == 2){
									addError("Error, function already exists in this scope ",yytext,yylineno);
								}
								else if (scopeLookUp(yytext,0) == 3){
									addError("Error, collision with library function",yytext,yylineno);
								}
								else {
									tmp = hashInsert(yytext, yylineno, Userfunc, currscope);
								}
							} 
			L_PAR	{
						printf("funcdef: FUNCTION ID L_PAR at line %d --> %s\n", yylineno, yytext);
						currscope++; inFunc = 1;
					}
			idlist 	{printf("funcdef: FUNCTION ID L_PAR idlist at line %d --> %s\n", yylineno, yytext);}
			R_PAR	{printf("funcdef: FUNCTION ID L_PAR idlist R_PAR at line %d --> %s\n", yylineno, yytext);}
			block 	{printf("funcdef: FUNCTION ID L_PAR idlist R_PAR block  at line %d --> %s\n", yylineno, yytext);}
			;

const:		REAL 		{printf("const: REAL at line %d --> %s\n", yylineno, yytext);}
			|INTEGER	{printf("const: INTEGER at line %d --> %s\n", yylineno, yytext);}
			|STRING 	{printf("const: FLEX_STRING at line %d --> %s\n", yylineno, yytext);}
			|NIL		{printf("const: NIL at line %d --> %s\n", yylineno, yytext);}
			|TRUE		{printf("const: TRUE at line %d --> %s\n", yylineno, yytext);}
			|FALSE 		{printf("const: FALSE at line %d --> %s\n", yylineno, yytext);}
			;

idlist:		ID			
			{	
				SymbolTableEntry *formal;
				
				int found = scopeLookUp(yytext, currscope);

				if (found == 3 ) {
					addError("Error, collision with library function", yytext, yylineno);
				}
				else if (found != 0){
					addError("Error, symbol already exists", yytext, yylineno);
				}
				else {
					formal = hashInsert(yytext,yylineno,Formal,currscope);
					insertFormal(tmp, formal);
				}
			}
			|idlist COMMA ID 	{														
									SymbolTableEntry *formal;
									
									int found = scopeLookUp(yytext, currscope);

									if (found == 3 ) {
										addError("Error, collision with library function", yytext, yylineno);
									}
									else if (found != 0){
										addError("Error, symbol already exists", yytext, yylineno);
									}
									else {
										formal = hashInsert(yytext,yylineno,Formal,currscope);
										insertFormal(tmp, formal);
									}
								}
			|
			;

ifstmt:		IF L_PAR expr R_PAR stmt 		{printf("ifstmt: IF L_PAR expr R_PAR stmt at line %d --> %s\n", yylineno, yytext);}
			|IF L_PAR expr R_PAR stmt ELSE stmt 	{printf("ifstmt: IF L_PAR expr R_PAR stmt ELSE stmt line %d --> %s\n", yylineno, yytext);}
			;


whilestmt:	WHILE L_PAR expr R_PAR stmt 	{printf("whilestmt: WHILE L_PAR expr R_PAR stmt at line %d --> %s\n", yylineno, yytext);}
			;

forstmt:  	FOR L_PAR elist SEMICOLON expr SEMICOLON elist R_PAR stmt {printf("forstm: FOR L_PAR elist SEMICOLON expr SEMICOLON elist R_PAR stmt at line %d --> %s\n", yylineno, yytext);}
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
    
    initialize();

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
    printScopeList();
	printErrorList();

    return 0;
}