%{
    #include <stdio.h>
	#include "symTable.h"

    int yyerror(char* yaccProvidedMessage);
    extern int yylex(void);

    extern int yylineno;
    extern char *yytext;
    extern FILE *yyin;
	FILE *fp;
    unsigned int currscope = 0;
	unsigned int inFunc = 0;
	struct SymbolTableEntry *tmp;
	unsigned int funcPrefix = 0;
	unsigned int betweenFunc = 0;
	unsigned int inLoop = 0;
%}

%defines

%union{
    int intValue;
    double realValue;
    char* stringValue;
	struct SymbolTableEntry* exprNode;
}

%type <exprNode> lvalue
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

program:	stmtlist	{	fprintf(fp, "promgram: stmtlist at line %d --> %s\n", yylineno, yytext);}
			|
			;

stmtlist:	stmt				{	fprintf(fp, "stmtlist: stmt at line %d --> %s\n", yylineno, yytext);}
			|stmtlist stmt		{	fprintf(fp, "stmtlist: stmtlist stmt at line %d --> %s\n", yylineno, yytext);}	
			;

stmt:		expr SEMICOLON			{	fprintf(fp, "stmt: expr SEMICOLON at line %d --> %s\n", yylineno, yytext);}
			|ifstmt					{	fprintf(fp, "stmt: ifstmt at line %d --> %s\n", yylineno, yytext);}
			|whilestmt				{	fprintf(fp, "stmt: whilestmt at line %d --> %s\n", yylineno, yytext);}
			|forstmt				{	fprintf(fp, "stmt: forstmt at line %d --> %s\n", yylineno, yytext);}
			|returnstmt				{	fprintf(fp, "stmt: returnstmt at line %d --> %s\n", yylineno, yytext);}
			|BREAK SEMICOLON		{
										fprintf(fp, "stmt: BREAK SEMICOLON at line %d --> %s\n", yylineno, yytext);
										if (inLoop == 0) 
											addError("Break use while not in loop", "", yylineno);
									}
			|CONTINUE SEMICOLON		{	
										fprintf(fp, "stmt: CONTINUE SEMICOLON at line %d --> %s\n", yylineno, yytext);
										if (inLoop == 0) 
											addError("Continue use while not in loop", "", yylineno);
									}
			|block					{	fprintf(fp, "stmt: block at line %d --> %s\n", yylineno, yytext);}
			|funcdef				{	fprintf(fp, "stmt: funcdef at line %d --> %s\n", yylineno, yytext);}
			|SEMICOLON				{	fprintf(fp, "stmt: SEMICOLON at line %d --> %s\n", yylineno, yytext);}
			;

expr:		assignexpr				{	fprintf(fp, "expr: assignexpr at line %d --> %s\n", yylineno, yytext);}
			|expr PLUS expr			{	fprintf(fp, "expr: expr PLUS expr at line %d --> %s\n", yylineno, yytext);}
			|expr MINUS expr		{	fprintf(fp, "expr: MINUS op expr at line %d --> %s\n", yylineno, yytext);}
			|expr MULT expr			{	fprintf(fp, "expr: expr MULT expr at line %d --> %s\n", yylineno, yytext);}
			|expr DIV expr			{	fprintf(fp, "expr: expr DIV expr at line %d --> %s\n", yylineno, yytext);}
			|expr MOD expr			{	fprintf(fp, "expr: expr MOD expr at line %d --> %s\n", yylineno, yytext);}
			|expr GREATER expr		{	fprintf(fp, "expr: expr GREATER expr at line %d --> %s\n", yylineno, yytext);}
			|expr GREATER_EQ expr	{	fprintf(fp, "expr: expr GREATER_EQ expr at line %d --> %s\n", yylineno, yytext);}
			|expr LESS expr			{	fprintf(fp, "expr: expr LESS expr at line %d --> %s\n", yylineno, yytext);}
			|expr LESS_EQ expr		{	fprintf(fp, "expr: expr LESS_EQ expr at line %d --> %s\n", yylineno, yytext);}
			|expr EQUAL expr		{	fprintf(fp, "expr: expr EQUAL expr at line %d --> %s\n", yylineno, yytext);}
			|expr NOT_EQUAL expr	{	fprintf(fp, "expr: expr NOT_EQUAL expr at line %d --> %s\n", yylineno, yytext);}
			|expr AND expr			{	fprintf(fp, "expr: expr AND expr at line %d --> %s\n", yylineno, yytext);}
			|expr OR expr			{	fprintf(fp, "expr: expr OR expr at line %d --> %s\n", yylineno, yytext);}
			|term					{	fprintf(fp, "expr: term at line %d --> %s\n", yylineno, yytext);}
			;

term:		L_PAR 						{	fprintf(fp, "term: L_PAR at line %d --> %s\n", yylineno, yytext);}
			expr 						{	fprintf(fp, "term: L_PAR expr at line %d --> %s\n", yylineno, yytext);}
			R_PAR						{	fprintf(fp, "term: L_PAR expr R_PAR at line %d --> %s\n", yylineno, yytext);}
			|MINUS expr %prec UMINUS	{	fprintf(fp, "term: MINUS expr at line %d --> %s\n", yylineno, yytext);}
			|NOT expr					{	fprintf(fp, "term: NOT expr at line %d --> %s\n", yylineno, yytext);}
			|INCR lvalue				{
											fprintf(fp, "term: INCR lvalue at line %d --> %s\n", yylineno, yytext);
											printf("LVALUE = %s\n", yytext);
										}
			|lvalue INCR				{
											fprintf(fp, "term: lvalue INCR at line %d --> %s\n", yylineno, yytext);
											/*
											enum SymbolType type = $1->type;
											if(type != Global || type != Local || type != Formal){
												addError("Error, cant increment function", yytext, yylineno);
											}
											*/
										}
			|DECR lvalue				{fprintf(fp, "term: DECR lvalue at line %d --> %s\n", yylineno, yytext);
											/*
											enum SymbolType type = $2->type;
											if(type != Global || type != Local || type != Formal){
												addError("Error, cant decr function", yytext, yylineno);
											}
											*/
										}
			|lvalue DECR				{	fprintf(fp, "term: lvalue DECR at line %d --> %s\n", yylineno, yytext);}
			|primary					{	fprintf(fp, "term: primary at line %d --> %s\n", yylineno, yytext);}
			;

assignexpr:	lvalue ASSIGN expr		{	fprintf(fp, "assignexpr: lvalue ASSIGN expr at line %d --> %s\n", yylineno, yytext);}
			;

primary:	lvalue					{	fprintf(fp, "primary: lvalue at line %d --> %s\n", yylineno, yytext);}
			|call					{	fprintf(fp, "primary: call at line %d --> %s\n", yylineno, yytext);}
			|objectdef				{	fprintf(fp, "primary: objectdef at line %d --> %s\n", yylineno, yytext);}
			|L_PAR funcdef R_PAR	{	fprintf(fp, "primary: L_PAR funcdef R_PAR at line %d --> %s\n", yylineno, yytext);}
			|const					{	fprintf(fp, "primary: const at line %d --> %s\n", yylineno, yytext);}
			;

lvalue:		ID				{
								fprintf(fp, "lvalue: ID at line %d --> %s\n", yylineno, yylval.stringValue);

								int result, varInFunc;
								enum SymbolType type;
								
								result = generalLookUp(yylval.stringValue, currscope);
								varInFunc= findInFunc(yylval.stringValue, currscope);

								switch (result){
									case 1:
										fprintf(fp, "Libfunc found\n");
										break;
									case 2:
										fprintf(fp, "Userfunc found\n");
										break;
									case 3:
										fprintf(fp, "Global var found\n");
										break;
									case 4:
										fprintf(fp, "Local var found\n");
										if(inFunc - varInFunc >= 1)
											addError("Cannot access symbol", yytext, yylineno);
										else
											fprintf(fp, "Symbol %s found and it's accessible\n", yylval.stringValue);
										break;
									case 5:
										fprintf(fp, "Formal var found\n");
										if(inFunc - varInFunc >= 1)
											addError("Cannot access symbol", yytext, yylineno);
										else
											fprintf(fp, "Symbol %s found and it's accessible\n", yylval.stringValue);
										break;
									default:
										if(currscope == 0)
											type = Global;
										else
											type = Local;
										fprintf(fp, "Put %s to SymbolTable\n", yylval.stringValue);
										$$ = hashInsert(yylval.stringValue, yylineno, type, currscope, inFunc);
								}
							}

			|LOCAL ID		{
								fprintf(fp, "lvalue: LOCAL ID at line %d --> %s\n", yylineno, yytext);

								int found = scopeLookUp(yytext, currscope);

								if( found == 2 || found == 3 || found == 4 || found == 5){
									fprintf(fp, "Ok, symbol found locally\n");
								}
								else if(scopeLookUp(yytext, 0) == 1){
									if(currscope == 0)
										fprintf(fp, "Ok, symbol found in scope 0\n");
									else
										addError("Error, collision with library function", yytext, yylineno);
								}
								else{
									if(currscope == 0){
										fprintf(fp, "New Global var in scope %d\n", currscope);
										hashInsert(yytext, yylineno, Global, currscope, inFunc);
									}
									else{
										fprintf(fp, "New Local var in scope %d\n", currscope);
										hashInsert(yytext, yylineno, Local, currscope, inFunc);
									}
								}		
							}

			|DCOLON ID		{
								fprintf(fp, "lvalue: DCOLON ID at line %d --> %s\n", yylineno, yytext);

								int found = scopeLookUp(yytext, 0);

								if(found == 1)
									fprintf(fp, "Libfunc %s found in line %d", yytext, yylineno);
								else if(found == 2)
									fprintf(fp, "Global userFunc %s found in line %d\n", yytext, yylineno);
								else if(found == 3 || found == 4 || found == 5) 
									fprintf(fp, "Global var %s found in line %d\n", yytext, yylineno); 
								else
									addError("Error, Global symbol not found", yytext, yylineno);
							}
			|member			{	fprintf(fp, "lvalue: member at line %d --> %s\n", yylineno, yytext);}
			;

member:		lvalue DOT ID 							{	fprintf(fp, "member: lvalue.ID at line %d --> %s\n", yylineno, yytext);}
			|lvalue L_BR expr R_BR 					{	fprintf(fp, "member: lvalue[expr] at line %d --> %s\n", yylineno, yytext);}
			|call DOT ID 							{	fprintf(fp, "member: call.ID at line %d --> %s\n", yylineno, yytext);}
			|call L_BR expr R_BR 					{	fprintf(fp, "member: lvalue[expr] at line %d --> %s\n", yylineno, yytext);}
			;

call:		call L_PAR objectlist R_PAR					{	fprintf(fp, "call: (objectlist) at line %d --> %s\n", yylineno, yytext);}
			|lvalue callsuffix						{	fprintf(fp, "call: lvalue callsuffix at line %d --> %s\n", yylineno, yytext);}
			|L_PAR funcdef R_PAR L_PAR objectlist R_PAR	{	fprintf(fp, "call: (funcdef) (objectlist) at line %d --> %s\n", yylineno, yytext);}
			;

callsuffix:	normcall 						{	fprintf(fp, "callsuffix: normcall at line %d --> %s\n", yylineno, yytext);}
			|methodcall						{	fprintf(fp, "callsuffix: methodcall at line %d --> %s\n", yylineno, yytext);}
			;

normcall:	L_PAR objectlist R_PAR 				{	fprintf(fp, "normcall: (objectlist) at line %d --> %s\n", yylineno, yytext);}
			;

methodcall:		DDOT ID L_PAR objectlist R_PAR 	{	fprintf(fp, "methodcall: ..ID (objectlist) at line %d --> %s\n", yylineno, yytext);}
				;


objectlist:	expr 													{	fprintf(fp, "objectlist: expr at line %d --> %s\n", yylineno, yytext);}
			|LCURLY_BR expr COLON expr RCURLY_BR					{	fprintf(fp, "objectlist: {expr:expr} at line %d --> %s\n", yylineno, yytext);}
			|LCURLY_BR expr COLON expr RCURLY_BR COMMA objectlist	{	fprintf(fp, "objectlist: list {expr:expr}  at line %d --> %s\n", yylineno, yytext);}
			|expr COMMA objectlist									{	fprintf(fp, "objectlist: list expr  at line %d --> %s\n", yylineno, yytext);}
			|														{	fprintf(fp, "objectlist: empty  at line %d --> %s\n", yylineno, yytext);}
			;

objectdef:	L_BR objectlist R_BR 			{	fprintf(fp, "objectdef: [objectlist] at line %d --> %s\n", yylineno, yytext);}
			;

block:		LCURLY_BR	{
							fprintf(fp, "block: LCURLY_BR at line %d --> %s\n", yylineno, yytext);
							currscope++;
							} 
			RCURLY_BR 	{	
							fprintf(fp, "block: LCURLY_BR RCURLY_BR at line %d --> %s\n", yylineno, yytext);
							hideScope(currscope);
							currscope--;
						}		
			|LCURLY_BR	{
							fprintf(fp, "block: LCURLY_BR at line %d --> %s\n", yylineno, yytext); 
							currscope++;
						}
			stmtlist  	{	fprintf(fp, "block: LCURLY_BR  stmtlist at line %d --> %s\n", yylineno, yytext);}
			RCURLY_BR	{
							fprintf(fp, "block: LCURLY_BR stmtlist RCURLY_BR at line %d --> %s\n", yylineno, yytext);
							hideScope(currscope);
							currscope--;
						}
			;

funcdef:	FUNCTION
					{	
						fprintf(fp, "funcdef: FUNCTION at line %d --> %s\n", yylineno, yytext);
						tmp = hashInsert(generateName(funcPrefix), yylineno, Userfunc, currscope, inFunc);
						funcPrefix++;
					}
			L_PAR 	{	
						inFunc++;
						currscope++;
						fprintf(fp, "funcdef: FUNCTION L_PAR at line %d --> %s\n", yylineno, yytext);
					}
			idlist 	{	fprintf(fp, "funcdef: FUNCTION L_PAR idlist at line %d --> %s\n", yylineno, yytext);}
			R_PAR	{	
						fprintf(fp, "funcdef: FUNCTION L_PAR idlist R_PAR at line %d --> %s\n", yylineno, yytext);
						currscope--;
					}
			block  	{	
						fprintf(fp, "funcdef: FUNCTION L_PAR idlist R_PAR block at line %d --> %s\n", yylineno, yytext);
						inFunc--;
					}
			|FUNCTION ID 	{
								fprintf(fp, "funcdef: FUNCTION ID at line %d --> %s\n", yylineno, yytext);
								int found = scopeLookUp(yytext,currscope);

								if(found == 2){
									addError("Error, function already exists in this scope ",yytext,yylineno);
								}
								else if (scopeLookUp(yytext, 0) == 1){
									addError("Error, collision with library function",yytext,yylineno);
								}
								else if(found == 3 || found == 4 || found == 5){
									addError("Error, symbol already exists",yytext,yylineno);
								}
								else {
									tmp = hashInsert(yytext, yylineno, Userfunc, currscope, inFunc);
								}
							} 
			L_PAR	{
						fprintf(fp, "funcdef: FUNCTION ID L_PAR at line %d --> %s\n", yylineno, yytext);
						inFunc++;
						currscope++; 
						fprintf(fp, "funcdef: FUNCTION ID L_PAR at line %d --> %s\n", yylineno, yytext);
					}
			idlist 	{	fprintf(fp, "funcdef: FUNCTION ID L_PAR idlist at line %d --> %s\n", yylineno, yytext);}
			R_PAR	{
						fprintf(fp, "funcdef: FUNCTION ID L_PAR idlist R_PAR at line %d --> %s\n", yylineno, yytext);
						currscope--;
					}
			block 	{
						fprintf(fp, "funcdef: FUNCTION ID L_PAR idlist R_PAR block  at line %d --> %s\n", yylineno, yytext);
						inFunc--;
					}
			;

const:		REAL 		{	fprintf(fp, "const: REAL at line %d --> %s\n", yylineno, yytext);}
			|INTEGER	{	fprintf(fp, "const: INTEGER at line %d --> %s\n", yylineno, yytext);}
			|STRING 	{	fprintf(fp, "const: FLEX_STRING at line %d --> %s\n", yylineno, yytext);}
			|NIL		{	fprintf(fp, "const: NIL at line %d --> %s\n", yylineno, yytext);}
			|TRUE		{	fprintf(fp, "const: TRUE at line %d --> %s\n", yylineno, yytext);}
			|FALSE 		{	fprintf(fp, "const: FALSE at line %d --> %s\n", yylineno, yytext);}
			;

idlist:		ID	{
					fprintf(fp, "idlist: ID at line %d --> %s\n", yylineno, yytext);	

					SymbolTableEntry *formal;
					int found = scopeLookUp(yytext, currscope);

					if (scopeLookUp(yytext, 0) == 1 ){
						addError("Error, collision with library function", yytext, yylineno);
					}
					else if (found != 0){
						addError("Error, symbol already exists", yytext, yylineno);
					}
					else {
						tmp = hashInsert(yytext,yylineno,Formal,currscope, inFunc);
						insertFormal(tmp, formal);
					}
				}
			|idlist COMMA ID 	{	
									fprintf(fp, "idlist: idlist COMMA ID at line %d --> %s\n", yylineno, yytext);													
									SymbolTableEntry *formal;
									
									int found = scopeLookUp(yytext, currscope);

									if (scopeLookUp(yytext, 0) == 1 ) {
										addError("Error, collision with library function", yytext, yylineno);
									}
									else if (found != 0){
										addError("Error, symbol already exists", yytext, yylineno);
									}
									else {
										formal = hashInsert(yytext,yylineno,Formal,currscope, inFunc);
										insertFormal(tmp, formal);
									}
								}
			|
			;

ifstmt:		IF L_PAR expr R_PAR stmt 		{	fprintf(fp, "ifstmt: IF L_PAR expr R_PAR stmt at line %d --> %s\n", yylineno, yytext);}
			|IF L_PAR expr R_PAR stmt ELSE stmt 	{	fprintf(fp, "ifstmt: IF L_PAR expr R_PAR stmt ELSE stmt line %d --> %s\n", yylineno, yytext);}
			;


whilestmt:	WHILE L_PAR expr R_PAR	{	inLoop = 1;}
			stmt 	{	fprintf(fp, "whilestmt: WHILE L_PAR expr R_PAR stmt at line %d --> %s\n", yylineno, yytext);}
			;

forstmt:  	FOR L_PAR objectlist SEMICOLON expr SEMICOLON objectlist R_PAR 	{	inLoop = 1;}
			stmt {	fprintf(fp, "forstm: FOR L_PAR objectlist SEMICOLON expr SEMICOLON objectlist R_PAR stmt at line %d --> %s\n", yylineno, yytext);}
			;

returnstmt:	RETURN  SEMICOLON
					{
						fprintf(fp, "returnstmt: RETURN SEMICOLON at line %d --> %s\n", yylineno, yytext);
						if (inFunc == 0) 
							addError("Use of return while not in function", "", yylineno);
					} 
			|RETURN expr SEMICOLON	{
										fprintf(fp, "returnstmt: RETURN expr SEMICOLON at line %d --> %s\n", yylineno, yytext);
										if (inFunc == 0) 
											addError("Use of return while not in function", "", yylineno);
									}
			;

%%

int yyerror(char* yaccProvidedMessage){
    fprintf(stderr, "%s: at line %d, before token: %s\n", yaccProvidedMessage, yylineno, yytext);
    fprintf(stderr, "INPUT NOT VALID\n");
}

int main(int argc, char** argv){
    fp = fopen("grammarOutput", "w+");
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

	fclose(fp);

    return 0;
}