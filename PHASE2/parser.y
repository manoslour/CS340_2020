 
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
	enum SymbolType type;
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

program:	stmtlist	{	printf("promgram: stmtlist at line %d --> %s\n", yylineno, yytext);}
			|
			;

stmtlist:	stmt				{	printf("stmtlist: stmt at line %d --> %s\n", yylineno, yytext);}
			|stmtlist stmt		{	printf("stmtlist: stmtlist stmt at line %d --> %s\n", yylineno, yytext);}	
			;

stmt:		expr SEMICOLON			{	printf("stmt: expr SEMICOLON at line %d --> %s\n", yylineno, yytext);}
			|ifstmt					{	printf("stmt: ifstmt at line %d --> %s\n", yylineno, yytext);}
			|whilestmt				{	printf("stmt: whilestmt at line %d --> %s\n", yylineno, yytext);}
			|forstmt				{	printf("stmt: forstmt at line %d --> %s\n", yylineno, yytext);}
			|returnstmt				{	printf("stmt: returnstmt at line %d --> %s\n", yylineno, yytext);}
			|BREAK SEMICOLON		{
										printf("stmt: BREAK SEMICOLON at line %d --> %s\n", yylineno, yytext);
										if (inLoop == 0) 
											addError("Use of break while not in loop", "", yylineno);
									}
			|CONTINUE SEMICOLON		{	
										printf("stmt: CONTINUE SEMICOLON at line %d --> %s\n", yylineno, yytext);
										if (inLoop == 0) 
											addError("Use of continue while not in loop", "", yylineno);
									}
			|block					{	printf("stmt: block at line %d --> %s\n", yylineno, yytext);}
			|funcdef				{	printf("stmt: funcdef at line %d --> %s\n", yylineno, yytext);}
			|SEMICOLON				{	printf("stmt: SEMICOLON at line %d --> %s\n", yylineno, yytext);}
			;

expr:		assignexpr				{	printf("expr: assignexpr at line %d --> %s\n", yylineno, yytext);}
			|expr PLUS expr			{	printf("expr: expr PLUS expr at line %d --> %s\n", yylineno, yytext);}
			|expr MINUS expr		{	printf("expr: MINUS op expr at line %d --> %s\n", yylineno, yytext);}
			|expr MULT expr			{	printf("expr: expr MULT expr at line %d --> %s\n", yylineno, yytext);}
			|expr DIV expr			{	printf("expr: expr DIV expr at line %d --> %s\n", yylineno, yytext);}
			|expr MOD expr			{	printf("expr: expr MOD expr at line %d --> %s\n", yylineno, yytext);}
			|expr GREATER expr		{	printf("expr: expr GREATER expr at line %d --> %s\n", yylineno, yytext);}
			|expr GREATER_EQ expr	{	printf("expr: expr GREATER_EQ expr at line %d --> %s\n", yylineno, yytext);}
			|expr LESS expr			{	printf("expr: expr LESS expr at line %d --> %s\n", yylineno, yytext);}
			|expr LESS_EQ expr		{	printf("expr: expr LESS_EQ expr at line %d --> %s\n", yylineno, yytext);}
			|expr EQUAL expr		{	printf("expr: expr EQUAL expr at line %d --> %s\n", yylineno, yytext);}
			|expr NOT_EQUAL expr	{	printf("expr: expr NOT_EQUAL expr at line %d --> %s\n", yylineno, yytext);}
			|expr AND expr			{	printf("expr: expr AND expr at line %d --> %s\n", yylineno, yytext);}
			|expr OR expr			{	printf("expr: expr OR expr at line %d --> %s\n", yylineno, yytext);}
			|term					{	printf("expr: term at line %d --> %s\n", yylineno, yytext);}
			;

term:		L_PAR 						{	printf("term: L_PAR at line %d --> %s\n", yylineno, yytext);}
			expr 						{	printf("term: L_PAR expr at line %d --> %s\n", yylineno, yytext);}
			R_PAR						{	printf("term: L_PAR expr R_PAR at line %d --> %s\n", yylineno, yytext);}
			|MINUS expr %prec UMINUS	{	printf("term: MINUS expr at line %d --> %s\n", yylineno, yytext);}
			|NOT expr					{	printf("term: NOT expr at line %d --> %s\n", yylineno, yytext);}
			|INCR lvalue				{
											printf("term: INCR lvalue at line %d --> %s\n", yylineno, yytext);
											type = $2->type;
											if(type == Userfunc || type == Libfunc)
												addError("Error, using function as an lvalue", "", yylineno);
										}
			|lvalue INCR				{
											printf("term: lvalue INCR at line %d --> %s\n", yylineno, yytext);
											type = $1->type;
											if(type == Userfunc || type == Libfunc)
												addError("Error, using function as an lvalue", "", yylineno);

										}
			|DECR lvalue				{
											printf("term: DECR lvalue at line %d --> %s\n", yylineno, yytext);
											type = $2->type;
											if(type == Userfunc || type == Libfunc)
												addError("Error, using function as an lvalue", "", yylineno);
										}
			|lvalue DECR				{	
											printf("term: lvalue DECR at line %d --> %s\n", yylineno, yytext);
											type = $1->type;
											if(type == Userfunc || type == Libfunc)
												addError("Error, using function as an lvalue", "", yylineno);
										}
			|primary					{	printf("term: primary at line %d --> %s\n", yylineno, yytext);}
			;

assignexpr:	lvalue	{
						type = $1->type;
						if(type == Userfunc || type == Libfunc){
							addError("Error, using function as an lvalue", "", yylineno);
						}
					} 
			ASSIGN expr		{	
										printf("assignexpr: lvalue ASSIGN expr at line %d --> %s\n", yylineno, yytext);
									}
			;

primary:	lvalue					{	printf("primary: lvalue at line %d --> %s\n", yylineno, yytext);}
			|call					{	printf("primary: call at line %d --> %s\n", yylineno, yytext);}
			|objectdef				{	printf("primary: objectdef at line %d --> %s\n", yylineno, yytext);}
			|L_PAR funcdef R_PAR	{	printf("primary: L_PAR funcdef R_PAR at line %d --> %s\n", yylineno, yytext);}
			|const					{	printf("primary: const at line %d --> %s\n", yylineno, yytext);}
			;

lvalue:		ID				{
								printf("lvalue: ID at line %d --> %s\n", yylineno, yylval.stringValue);

								int varInFunc;
								enum SymbolType type;
								SymbolTableEntry* result;
								
								result = generalLookUp(yylval.stringValue, currscope);
								varInFunc= findInFunc(yylval.stringValue, currscope);

								if(result != NULL){

									switch (result->type){
										case Global:
											printf("Global var found\n");
											$$->type = Global;
											break;
										case Local:
											printf("Local var found\n");
											$$->type = Local;
											if(inFunc - varInFunc >= 1)
												addError("Cannot access symbol", yytext, yylineno);
											else
												printf("Symbol %s found and it's accessible\n", yylval.stringValue);
											break;
										case Formal:
											printf("Formal var found\n");
											$$->type = Formal;
											if(inFunc - varInFunc >= 1)
												addError("Cannot access symbol", yytext, yylineno);
											else
												printf("Symbol %s found and it's accessible\n", yylval.stringValue);
											break;
										case Userfunc:
											printf("Userfunc found\n");
											$$->type = Userfunc;
											break;
										case Libfunc:
											printf("Libfunc found\n");
											$$->type = Libfunc;
											break;
									}
								}
								else{
									if(currscope == 0)
										type = Global;
									else
										type = Local;

									printf("Put %s to SymbolTable\n", yylval.stringValue);
									hashInsert(yylval.stringValue, yylineno, type, currscope, inFunc);
									$$->type = type;
								}
							}

			|LOCAL ID		{
								printf("lvalue: LOCAL ID at line %d --> %s\n", yylineno, yytext);
								//printf("LOCAL SYMBOL = %s\n", yylval.stringValue);
								//$$->type = Local;
								//printf("LOCAL LVALUE TYPE = %d\n", $$->type);

								SymbolTableEntry *found, *globalFound;
								
								found = scopeLookUp(yytext, currscope);
								globalFound = scopeLookUp(yytext, 0);

								if( (found != NULL) && (found->type == Global || found->type == Local || found->type == Formal || found->type == Userfunc) ){
									printf("Ok, symbol found locally\n");
								}
								else if( (globalFound != NULL) && (globalFound->type) == Libfunc){
									if(currscope == 0)
										printf("Ok, symbol found in scope 0\n");
									else
										addError("Error, collision with library function", yytext, yylineno);
								}
								else{
									if(currscope == 0){
										printf("New Global var in scope %d\n", currscope);
										hashInsert(yytext, yylineno, Global, currscope, inFunc);
									}
									else{
										printf("New Local var in scope %d\n", currscope);
										hashInsert(yytext, yylineno, Local, currscope, inFunc);
									}
								}
							}

			|DCOLON ID		{
								printf("lvalue: DCOLON ID at line %d --> %s\n", yylineno, yytext);

								SymbolTableEntry *found = scopeLookUp(yytext, 0);

								if( (found != NULL) && (found->type == Libfunc) )
									printf("Libfunc %s found in line %d", yytext, yylineno);
								else if( (found != NULL) && (found->type == Userfunc) )
									printf("Global userFunc %s found in line %d\n", yytext, yylineno);
								else if( (found != NULL) && (found->type == Global || found->type == Local || found->type == Formal) ) 
									printf("Global var %s found in line %d\n", yytext, yylineno); 
								else
									addError("Error, Global symbol not found", yytext, yylineno);
							}
			|member			{	printf("lvalue: member at line %d --> %s\n", yylineno, yytext);}
			;

member:		lvalue DOT ID 							{	printf("member: lvalue.ID at line %d --> %s\n", yylineno, yytext);}
			|lvalue L_BR expr R_BR 					{	printf("member: lvalue[expr] at line %d --> %s\n", yylineno, yytext);}
			|call DOT ID 							{	printf("member: call.ID at line %d --> %s\n", yylineno, yytext);}
			|call L_BR expr R_BR 					{	printf("member: call[expr] at line %d --> %s\n", yylineno, yytext);}
			;

call:		call L_PAR objectlist R_PAR					{	printf("call: (objectlist) at line %d --> %s\n", yylineno, yytext);}
			|lvalue callsuffix						{	printf("call: lvalue callsuffix at line %d --> %s\n", yylineno, yytext);}
			|L_PAR funcdef R_PAR L_PAR objectlist R_PAR	{	printf("call: (funcdef) (objectlist) at line %d --> %s\n", yylineno, yytext);}
			;

callsuffix:	normcall 						{	printf("callsuffix: normcall at line %d --> %s\n", yylineno, yytext);}
			|methodcall						{	printf("callsuffix: methodcall at line %d --> %s\n", yylineno, yytext);}
			;

normcall:	L_PAR objectlist R_PAR 				{	printf("normcall: (objectlist) at line %d --> %s\n", yylineno, yytext);}
			;

methodcall:		DDOT ID L_PAR objectlist R_PAR 	{	printf("methodcall: ..ID (objectlist) at line %d --> %s\n", yylineno, yytext);}
				;

objectlist:	expr 													{	printf("objectlist: expr at line %d --> %s\n", yylineno, yytext);}
			|LCURLY_BR expr COLON expr RCURLY_BR					{	printf("objectlist: {expr:expr} at line %d --> %s\n", yylineno, yytext);}
			|LCURLY_BR expr COLON expr RCURLY_BR COMMA objectlist	{	printf("objectlist: list {expr:expr}  at line %d --> %s\n", yylineno, yytext);}
			|expr COMMA objectlist									{	printf("objectlist: list expr  at line %d --> %s\n", yylineno, yytext);}
			|														{	printf("objectlist: empty  at line %d --> %s\n", yylineno, yytext);}
			;

objectdef:	L_BR objectlist R_BR 			{	printf("objectdef: [objectlist] at line %d --> %s\n", yylineno, yytext);}
			;

block:		LCURLY_BR	{
							printf("block: LCURLY_BR at line %d --> %s\n", yylineno, yytext);
							currscope++;
							} 
			RCURLY_BR 	{	
							printf("block: LCURLY_BR RCURLY_BR at line %d --> %s\n", yylineno, yytext);
							hideScope(currscope);
							currscope--;
						}		
			|LCURLY_BR	{
							printf("block: LCURLY_BR at line %d --> %s\n", yylineno, yytext); 
							currscope++;
						}
			stmtlist  	{	printf("block: LCURLY_BR  stmtlist at line %d --> %s\n", yylineno, yytext);}
			RCURLY_BR	{
							printf("block: LCURLY_BR stmtlist RCURLY_BR at line %d --> %s\n", yylineno, yytext);
							hideScope(currscope);
							currscope--;
						}
			;

funcdef:	FUNCTION
					{	
						printf("funcdef: FUNCTION at line %d --> %s\n", yylineno, yytext);
						tmp = hashInsert(generateName(funcPrefix), yylineno, Userfunc, currscope, inFunc);
						funcPrefix++;
					}
			L_PAR 	{	
						inFunc++;
						currscope++;
						printf("funcdef: FUNCTION L_PAR at line %d --> %s\n", yylineno, yytext);
					}
			idlist 	{	printf("funcdef: FUNCTION L_PAR idlist at line %d --> %s\n", yylineno, yytext);}
			R_PAR	{	
						printf("funcdef: FUNCTION L_PAR idlist R_PAR at line %d --> %s\n", yylineno, yytext);
						currscope--;
					}
			block  	{	
						printf("funcdef: FUNCTION L_PAR idlist R_PAR block at line %d --> %s\n", yylineno, yytext);
						inFunc--;
					}
			|FUNCTION ID 	{
								printf("funcdef: FUNCTION ID at line %d --> %s\n", yylineno, yytext);

								SymbolTableEntry *found, *globalFound;

								found = scopeLookUp(yytext,currscope);
								globalFound = scopeLookUp(yytext, 0);

								if( (found != NULL) && (found->type == Userfunc) ){
									addError("Error, function already exists in this scope ", yytext, yylineno);
								}
								else if ( (globalFound != NULL) && (globalFound->type == Libfunc) ){
									addError("Error, collision with library function", yytext, yylineno);
								}
								else if( (found != NULL) && (found->type == Global || found->type == Local || found->type == Formal) ){
									addError("Error, symbol already exists", yytext, yylineno);
								}
								else {
									tmp = hashInsert(yytext, yylineno, Userfunc, currscope, inFunc);
								}
							} 
			L_PAR	{
						printf("funcdef: FUNCTION ID L_PAR at line %d --> %s\n", yylineno, yytext);
						inFunc++;
						currscope++; 
						printf("funcdef: FUNCTION ID L_PAR at line %d --> %s\n", yylineno, yytext);
					}
			idlist 	{	printf("funcdef: FUNCTION ID L_PAR idlist at line %d --> %s\n", yylineno, yytext);}
			R_PAR	{
						printf("funcdef: FUNCTION ID L_PAR idlist R_PAR at line %d --> %s\n", yylineno, yytext);
						currscope--;
					}
			block 	{
						printf("funcdef: FUNCTION ID L_PAR idlist R_PAR block  at line %d --> %s\n", yylineno, yytext);
						inFunc--;
					}
			;

const:		REAL 		{	printf("const: REAL at line %d --> %s\n", yylineno, yytext);}
			|INTEGER	{	printf("const: INTEGER at line %d --> %s\n", yylineno, yytext);}
			|STRING 	{	printf("const: FLEX_STRING at line %d --> %s\n", yylineno, yytext);}
			|NIL		{	printf("const: NIL at line %d --> %s\n", yylineno, yytext);}
			|TRUE		{	printf("const: TRUE at line %d --> %s\n", yylineno, yytext);}
			|FALSE 		{	printf("const: FALSE at line %d --> %s\n", yylineno, yytext);}
			;

idlist:		ID	{
					printf("idlist: ID at line %d --> %s\n", yylineno, yytext);	

					SymbolTableEntry *formal;
					SymbolTableEntry* found, *globalFound;
					found = scopeLookUp(yytext, currscope);
					globalFound = scopeLookUp(yytext, 0);

					if ( (globalFound != NULL) && (globalFound->type) == Libfunc ){
						printf("Error, collision with library function\n");
						addError("Error, collision with library function", yytext, yylineno);
					}
					else if ( (found != NULL) && (found->type == Formal || found->type == Local || found->type == Userfunc ) ){
						printf("Error, symbol already exists\n");
						addError("Error, symbol already exists", yytext, yylineno);
					}
					else {
						printf("Put formal arg %s to SymbolTable\n", yytext);
						tmp = hashInsert(yytext,yylineno,Formal,currscope, inFunc);
						insertFormal(tmp, formal);
					}
				}
			|idlist COMMA ID 	{	
									printf("idlist: idlist COMMA ID at line %d --> %s\n", yylineno, yytext);													
									SymbolTableEntry *formal;
									
									SymbolTableEntry *found, *globalFound;
									
									found = scopeLookUp(yytext, currscope);
									globalFound = scopeLookUp(yytext, 0);

									if ( (globalFound != NULL) && (globalFound->type) == Libfunc ) {
										addError("Error, collision with library function", yytext, yylineno);
									}
									else if (found != NULL){
										addError("Error, symbol already exists", yytext, yylineno);
									}
									else {
										formal = hashInsert(yytext,yylineno,Formal,currscope, inFunc);
										insertFormal(tmp, formal);
									}
								}
			|
			;

ifstmt:		IF L_PAR expr R_PAR stmt 		{	printf("ifstmt: IF L_PAR expr R_PAR stmt at line %d --> %s\n", yylineno, yytext);}
			|IF L_PAR expr R_PAR stmt ELSE stmt 	{	printf("ifstmt: IF L_PAR expr R_PAR stmt ELSE stmt line %d --> %s\n", yylineno, yytext);}
			;


whilestmt:	WHILE L_PAR expr R_PAR	{	inLoop = 1;}
			stmt 	{	printf("whilestmt: WHILE L_PAR expr R_PAR stmt at line %d --> %s\n", yylineno, yytext);}
			;

forstmt:  	FOR L_PAR objectlist SEMICOLON expr SEMICOLON objectlist R_PAR 	{	inLoop = 1;}
			stmt {	printf("forstm: FOR L_PAR objectlist SEMICOLON expr SEMICOLON objectlist R_PAR stmt at line %d --> %s\n", yylineno, yytext);}
			;

returnstmt:	RETURN  SEMICOLON
					{
						printf("returnstmt: RETURN SEMICOLON at line %d --> %s\n", yylineno, yytext);
						if (inFunc == 0) 
							addError("Use of return while not in function", "", yylineno);
					} 
			|RETURN expr SEMICOLON	{
										printf("returnstmt: RETURN expr SEMICOLON at line %d --> %s\n", yylineno, yytext);
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