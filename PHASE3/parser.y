 %{
   #include <stdio.h>
   #include "stack.h"
   #include "symTable.h"

   int yyerror(char* yaccProvidedMessage);
   extern int yylex(void);

   FILE *fp;
   symbol *tmp;
   SymbolType type;
   extern FILE *yyin;
   extern int yylineno;
   extern char *yytext;
   unsigned int inFunc = 0;
   unsigned int inLoop = 0;
   unsigned int funcprefix = 0;
   StackNode *scopeoffsetStack = NULL;
   unsigned int betweenFunc = 0;
   unsigned int currentscope = 0;
%}

//%defines

%union{
  int intValue;
  double realValue;
	unsigned int unsignedValue;
  char* stringValue;
  struct symbol* symNode;
	struct expr* exprNode;
	struct call* callNode;
}

%type <stringValue> funcname
%type <symNode> funcprefix funcdef
%type <callNode> callsuffix normcall methodcall
%type <unsignedValue> funcbody ifprefix elseprefix
%type <exprNode> lvalue tableitem primary assignexpr call term tablemake expr elist indexed indexedelem const
%token <realValue> REAL
%token <intValue> INTEGER 
%token <stringValue> ID STRING
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

program:  stmtlist  {	fprintf(fp, "promgram: stmtlist at line %d --> %s\n", yylineno, yytext);}
			|
			;

stmtlist: stmt              { fprintf(fp, "stmtlist: stmt at line %d --> %s\n", yylineno, yytext);}
          |stmtlist stmt		{	fprintf(fp, "stmtlist: stmtlist stmt at line %d --> %s\n", yylineno, yytext);}
          ;

stmt:     expr SEMICOLON        {	fprintf(fp, "stmt: expr SEMICOLON at line %d --> %s\n", yylineno, yytext);}
          |ifstmt               { fprintf(fp, "stmt: ifstmt at line %d --> %s\n", yylineno, yytext);}
          |whilestmt            {	fprintf(fp, "stmt: whilestmt at line %d --> %s\n", yylineno, yytext);}
          |forstmt				      {	fprintf(fp, "stmt: forstmt at line %d --> %s\n", yylineno, yytext);}
          |returnstmt			      {	fprintf(fp, "stmt: returnstmt at line %d --> %s\n", yylineno, yytext);}
          |BREAK SEMICOLON		  {
										              fprintf(fp, "stmt: BREAK SEMICOLON at line %d --> %s\n", yylineno, yytext);
                                  if (inLoop == 0)
                                  addError("Use of break while not in loop", "", yylineno);
                                }
          |CONTINUE SEMICOLON		{
										              fprintf(fp, "stmt: CONTINUE SEMICOLON at line %d --> %s\n", yylineno, yytext);
                                  if (inLoop == 0)
                                    addError("Use of continue while not in loop", "", yylineno);
                                }
          |block                {	fprintf(fp, "stmt: block at line %d --> %s\n", yylineno, yytext);}
          |funcdef              {	fprintf(fp, "stmt: funcdef at line %d --> %s\n", yylineno, yytext);}
          |SEMICOLON            {	fprintf(fp, "stmt: SEMICOLON at line %d --> %s\n", yylineno, yytext);}
          ;

expr:     assignexpr            {	fprintf(fp, "expr: assignexpr at line %d --> %s\n", yylineno, yytext);}
          |expr PLUS expr       {
                                  fprintf(fp, "expr: expr PLUS expr at line %d --> %s\n", yylineno, yytext);
                                  if(illegalop($1, $3))
                                    addError("Error, illegal arithmetic operation", "", yylineno);
                                  else{
                                    if($1->type == constnum_e && $3->type == constnum_e)
                                      $$ = newexpr(constnum_e);
                                    else
                                      $$ = newexpr(arithexpr_e);
                                    $$->sym = newtemp();
                                    emit(add, $1, $3, $$, -1, yylineno);
                                  }
                                }
          |expr MINUS expr      {
                                  fprintf(fp, "expr: MINUS op expr at line %d --> %s\n", yylineno, yytext);
                                  if(illegalop($1, $3))
                                    addError("Error, illegal arithmetic operation", "", yylineno);
                                  else{
                                    if($1->type == constnum_e && $3->type == constnum_e)
                                      $$ = newexpr(constnum_e);
                                    else
                                      $$ = newexpr(arithexpr_e);
                                    $$->sym = newtemp();
                                    emit(sub, $1, $3, $$, -1, yylineno);
                                  }
                                }
          |expr MULT expr       {
                                  fprintf(fp, "expr: expr MULT expr at line %d --> %s\n", yylineno, yytext);
                                  if(illegalop($1, $3))
                                    addError("Error, illegal arithmetic operation", "", yylineno);
                                  else{
                                    if($1->type == constnum_e && $3->type == constnum_e)
                                      $$ = newexpr(constnum_e);
                                    else
                                      $$ = newexpr(arithexpr_e);
                                    $$->sym = newtemp();
                                    emit(mul, $1, $3, $$, -1, yylineno);
                                  }
                                }
          |expr DIV expr        {
                                  fprintf(fp, "expr: expr DIV expr at line %d --> %s\n", yylineno, yytext);
                                  if(illegalop($1, $3))
                                    addError("Error, illegal arithmetic operation", "", yylineno);
                                  else{
                                    if($1->type == constnum_e && $3->type == constnum_e)
                                      $$ = newexpr(constnum_e);
                                    else
                                      $$ = newexpr(arithexpr_e);
                                    $$->sym = newtemp();
                                    emit(divide, $1, $3, $$, -1, yylineno);
                                  }
                                }
          |expr MOD expr        {
                                  fprintf(fp, "expr: expr MOD expr at line %d --> %s\n", yylineno, yytext);
                                  if(illegalop($1, $3))
                                    addError("Error, illegal arithmetic operation", "", yylineno);
                                  else{
                                    if($1->type == constnum_e && $3->type == constnum_e)
                                      $$ = newexpr(constnum_e);
                                    else
                                      $$ = newexpr(arithexpr_e);
                                    $$->sym = newtemp();
                                    emit(mod, $1, $3, $$, -1, yylineno);
                                  }
                                }
          |expr GREATER expr		{
                                  fprintf(fp, "expr: expr GREATER expr at line %d --> %s\n", yylineno, yytext);
                                  if(illegalop($1, $3))
                                    addError("Error, illegal boolean operation", "", yylineno);
                                  else{
                                    printf("naiiiiiiiiiiiiiiiiiiiiiiiiiiiii");
                                    $$ = newexpr(boolexpr_e);
                                    $$->sym = newtemp();

                                    emit(if_greater, $1, $3, NULL, nextquad()+3, yylineno);
                                    emit(assign, newexpr_constbool(0), NULL, $$, -1, yylineno);
                                    emit(jump, NULL, NULL, NULL, nextquad()+2, yylineno);
                                    emit(assign, newexpr_constbool(1), NULL, $$, -1, yylineno);
                                  }
                                }
          |expr GREATER_EQ expr	{
                                  fprintf(fp, "expr: expr GREATER_EQ expr at line %d --> %s\n", yylineno, yytext);
                                  if(illegalop($1, $3))
                                    addError("Error, illegal boolean operation", "", yylineno);
                                  else{
                                    $$ = newexpr(boolexpr_e);
                                    $$->sym = newtemp();

                                    emit(if_greatereq, $1, $3, NULL, nextquad()+3, yylineno);
                                    emit(assign, newexpr_constbool(0), NULL, $$, -1, yylineno);
                                    emit(jump, NULL, NULL, NULL, nextquad()+2, yylineno);
                                    emit(assign, newexpr_constbool(1), NULL, $$, -1, yylineno);
                                  }
                                }
          |expr LESS expr       {
                                  fprintf(fp, "expr: expr LESS expr at line %d --> %s\n", yylineno, yytext);
                                  if(illegalop($1, $3))
                                    addError("Error, illegal boolean operation", "", yylineno);
                                  else{
                                    $$ = newexpr(boolexpr_e);
                                    $$->sym = newtemp();

                                    emit(if_less, $1, $3, NULL, nextquad()+3, yylineno);
                                    emit(assign, newexpr_constbool(0), NULL, $$, -1, yylineno);
                                    emit(jump, NULL, NULL, NULL, nextquad()+2, yylineno);
                                    emit(assign, newexpr_constbool(1), NULL, $$, -1, yylineno);
                                  }
                                }
          |expr LESS_EQ expr		{
                                  fprintf(fp, "expr: expr LESS_EQ expr at line %d --> %s\n", yylineno, yytext);
                                  if(illegalop($1, $3))
                                    addError("Error, illegal boolean operation", "", yylineno);
                                  else{
                                    $$ = newexpr(boolexpr_e);
                                    $$->sym = newtemp();

                                    emit(if_lesseq, $1, $3, NULL, nextquad()+3, yylineno);
                                    emit(assign, newexpr_constbool(0), NULL, $$, -1, yylineno);
                                    emit(jump, NULL, NULL, NULL, nextquad()+2, yylineno);
                                    emit(assign, newexpr_constbool(1), NULL, $$, -1, yylineno);
                                  }
                                }
          |expr EQUAL expr      {
                                  fprintf(fp, "expr: expr EQUAL expr at line %d --> %s\n", yylineno, yytext);
                                  if(illegalop($1, $3))
                                    addError("Error, illegal boolean operation", "", yylineno);
                                  else{
                                    $$ = newexpr(boolexpr_e);
                                    $$->sym = newtemp();

                                    emit(if_eq, $1, $3, NULL, nextquad()+3, yylineno);
                                    emit(assign, newexpr_constbool(0), NULL, $$, -1, yylineno);
                                    emit(jump, NULL, NULL, NULL, nextquad()+2, yylineno);
                                    emit(assign, newexpr_constbool(1), NULL, $$, -1, yylineno);
                                  }
                                }
          |expr NOT_EQUAL expr  {
                                  fprintf(fp, "expr: expr NOT_EQUAL expr at line %d --> %s\n", yylineno, yytext);
                                  if(illegalop($1, $3))
                                    addError("Error, illegal boolean operation", "", yylineno);
                                  else{
                                    $$ = newexpr(boolexpr_e);
                                    $$->sym = newtemp();

                                    emit(if_noteq, $1, $3, NULL, nextquad()+3, yylineno);
                                    emit(assign, newexpr_constbool(0), NULL, $$, -1, yylineno);
                                    emit(jump, NULL, NULL, NULL, nextquad()+2, yylineno);
                                    emit(assign, newexpr_constbool(1), NULL, $$, -1, yylineno);
                                  }
                                }
          |expr AND expr        {
                                  fprintf(fp, "expr: expr AND expr at line %d --> %s\n", yylineno, yytext);
                                  if(illegalop($1, $3))
                                    //MUST FIX FOR BOOLOP!
                                    addError("Error, illegal real operation", "", yylineno);
                                  else{
                                    $$ = newexpr(boolexpr_e);
                                    $$->sym = newtemp();
                                    emit(and, $1, $3, $$, -1, yylineno);
                                  }
                                }
          |expr OR expr			    {
                                  fprintf(fp, "expr: expr OR expr at line %d --> %s\n", yylineno, yytext);
                                  if(illegalop($1, $3))
                                    //MUST FIX FOR BOOLOP!
                                    addError("Error, illegal real operation", "", yylineno);
                                  else{
                                    $$ = newexpr(boolexpr_e);
                                    $$->sym = newtemp();
                                    emit(or, $1, $3, $$, -1, yylineno);
                                  }
                                }
          |term					        {	fprintf(fp, "expr: term at line %d --> %s\n", yylineno, yytext);}
          ;

term:     L_PAR expr R_PAR			    {
                                      fprintf(fp, "term: L_PAR expr R_PAR at line %d --> %s\n", yylineno, yytext);
                                      $$ = $2;
                                    }
          |MINUS expr %prec UMINUS	{
                                      fprintf(fp, "term: MINUS expr at line %d --> %s\n", yylineno, yytext);
                                      check_arith($2, "-expr");
                                      $$ = newexpr(arithexpr_e);
                                      $$->sym = newtemp();
                                      emit(uminus, $2, NULL, $$, -1, yylineno);
                                    }
          |NOT expr	                {
                                      fprintf(fp, "term: NOT expr at line %d --> %s\n", yylineno, yytext);
                                      $$ = newexpr(boolexpr_e);
                                      $$->sym = newtemp();
                                      emit(not, $2, NULL, $$, -1, yylineno);
                                    }
          |INCR lvalue              {
                                      fprintf(fp, "term: INCR lvalue at line %d --> %s\n", yylineno, yytext);
                                      check_arith($2, "++lvalue");
                                      if($2->type == tableitem_e){
                                        $$ = emit_iftableitem($2, yylineno);
                                        emit(add, $$, newexpr_constnum(1), $$, -1, yylineno);
                                        emit(tablesetelem, $2, $2->index, $$, -1, yylineno);

                                      }
                                      else{
                                        emit(add, $2, newexpr_constnum(1), $2, -1, yylineno);
                                        $$ = newexpr(arithexpr_e);
                                        $$->sym = newtemp();
                                        emit(assign, $2, NULL, $$, -1, yylineno);
                                      }
                                    }
          |lvalue INCR              {
                                      fprintf(fp, "term: lvalue INCR at line %d --> %s\n", yylineno, yytext);
                                      check_arith($1, "lvalue++");
                                      $$ = newexpr(var_e);
                                      $$->sym = newtemp();
                                      if($1->type == tableitem_e){
                                        expr* val = emit_iftableitem($1, yylineno);
                                        emit(assign, val, NULL, $$, -1, yylineno);
                                        emit(add, val, newexpr_constnum(1), val, -1, yylineno);
                                        emit(tablesetelem, $1, $1->index, val, -1, yylineno);
                                      }
                                      else{
                                        emit(assign, $1, NULL, $$, -1, yylineno);
                                        emit(add, $1, newexpr_constnum(1), $1, -1, yylineno);
                                      }
                                    }
          |DECR lvalue              {
                                      fprintf(fp, "term: DECR lvalue at line %d --> %s\n", yylineno, yytext);
                                      check_arith($2, "--lvalue");
                                      if($2->type == tableitem_e){
                                        $$ = emit_iftableitem($2, yylineno);
                                        emit(sub, $$, newexpr_constnum(1), $$, -1, yylineno);
                                        emit(tablesetelem, $2, $2->index, $$, -1, yylineno);
                                      }
                                      else{
                                        emit(sub, $2, newexpr_constnum(1), $2, -1, yylineno);
                                        $$ = newexpr(arithexpr_e);
                                        $$->sym = newtemp();
                                        emit(assign, $2, NULL, $$, -1, yylineno);
                                      }
                                    }
          |lvalue DECR              {
                                      fprintf(fp, "term: lvalue DECR at line %d --> %s\n", yylineno, yytext);
                                      check_arith($1, "lvalue--");
                                      $$ = newexpr(var_e);
                                      $$->sym = newtemp();
                                      if($1->type == tableitem_e){
                                        expr* val = emit_iftableitem($1, yylineno);
                                        emit(assign, val, NULL, $$, -1, yylineno);
                                        emit(sub, val, newexpr_constnum(1), val, -1, yylineno);
                                        emit(tablesetelem, $1, $1->index, val, -1, yylineno);
                                      }
                                      else{
                                        emit(assign, $1, NULL, $$, -1, yylineno);
                                        emit(sub, $1, newexpr_constnum(1), $1, -1, yylineno);
                                      }
                                    }
			    |primary                  {
                                      fprintf(fp, "term: primary at line %d --> %s\n", yylineno, yytext);
                                      $$ = $1;
                                    }
			    ;

assignexpr: lvalue ASSIGN expr  {
                                  fprintf(fp, "assignexpr: lvalue ASSIGN expr at line %d --> %s\n", yylineno, yytext);
                                  if($1->type == tableitem_e){
                                    emit(tablesetelem, $1, $1->index, $3, -1, yylineno);
                                    $$ = emit_iftableitem($1, yylineno);
                                    $$->type = assignexpr_e;
                                  }
                                  else{
                                    emit(assign, $3, NULL, $1, -1, yylineno);
                                    $$ = newexpr(assignexpr_e);
                                    $$->sym = newtemp();
                                    emit(assign, $1, NULL, $$, -1, yylineno);
                                  }
                                  //printf("Exiting assignexpr\n");
                                }
            ;

primary:    lvalue                { fprintf(fp, "primary: lvalue at line %d --> %s\n", yylineno, yytext);
										                $$ = emit_iftableitem($1, yylineno);
                                  }
            |call                 { fprintf(fp, "primary: call at line %d --> %s\n", yylineno, yytext);}
			      |tablemake            { fprintf(fp, "primary: tablemake at line %d --> %s\n", yylineno, yytext);}
			      |L_PAR funcdef R_PAR	{
                                    fprintf(fp, "primary: L_PAR funcdef R_PAR at line %d --> %s\n", yylineno, yytext);
                                    $$ = newexpr(programfunc_e);
                                    $$->sym = $2;
                                  }
			      |const					      {
                                    fprintf(fp, "primary: const at line %d --> %s\n", yylineno, yytext);
                                    printf("Enter primary: const\n");
                                    //printf("cosnt($1) %f\n", $1->numConst);
                                    $$ = $1;
                                  }
			      ;

lvalue:     ID          {
                          fprintf(fp, "lvalue: ID at line %d --> %s\n", yylineno, yylval.stringValue);
                          symbol *sym = lookup(yylval.stringValue, currentscope);
                          if(sym == NULL){
                            sym = hashInsert(yylval.stringValue, currentscope, yylineno, var_s, currscopespace(), currscopeoffset());
                            inccurrscopeoffset();
                            //printf("Inserted symbol %s\n", yylval.stringValue);
                          }
                          else {
                            printf("Symbol %s already defined\n", sym->name);
                            //MUST CHECK ACCESSIBILITY
                          }
                          $$ = lvalue_expr(sym);
                        }

            |LOCAL ID   {
                          symbol *sym, *tmp;
                          sym = scopelookup(yylval.stringValue,currentscope);
                          if(sym == NULL){
                            tmp = scopelookup(yylval.stringValue, 0);
                            if(tmp != NULL && tmp->type == libraryfunc_s)
                              printf("Error, collision with libfunc at line %d\n", yylineno);
                            else{
                              sym = hashInsert(yytext, currentscope, yylineno, var_s, currscopespace(), currscopeoffset());
                              inccurrscopeoffset();
                              //printf("Inserted local symbol %s\n", yylval.stringValue);
                            }
                          }
                          else{
                            if(sym->type == programfunc_s){
                              printf("Warning sym is a function\n");
                              $$ = lvalue_expr(sym);
                            }
                          }
                        }

			|DCOLON ID		    {
								          symbol *sym = scopelookup(yytext,0);
                          if(sym == NULL)
                            printf("Symbol %s does not exist\n", yylval.stringValue);
                          else
                            $$ = lvalue_expr(sym);\
                        }
			|tableitem			  {
                          fprintf(fp, "lvalue: tableitem at line %d --> %s\n", yylineno, yytext);
                          $$ = $1;
                        }
			;

tableitem:  lvalue DOT ID 				    {
                                        fprintf(fp, "tableitem: lvalue.ID at line %d --> %s\n", yylineno, yytext);
                                        $$ = member_item($1, $3, yylineno);
                                      }
			      |lvalue L_BR expr R_BR 		{
                                        fprintf(fp, "tableitem: lvalue[expr] at line %d --> %s\n", yylineno, yytext);
                                        $1 = emit_iftableitem($1, yylineno);
                                        $$ = newexpr(tableitem_e);
                                        $$->sym = $1->sym;
                                        $$->index = $3;
                                      }
			      |call DOT ID 						  { fprintf(fp, "tableitem: call.ID at line %d --> %s\n", yylineno, yytext);}
			      |call L_BR expr R_BR 			{	fprintf(fp, "tableitem: call[expr] at line %d --> %s\n", yylineno, yytext);}
			      ;

call:	      call L_PAR elist R_PAR	  {
                                        fprintf(fp, "call: (elist) at line %d --> %s\n", yylineno, yytext);
                                        printf("call (elist)\n");
                                        $$ = make_call($1, $3, yylineno);
                                      }
            |lvalue callsuffix				{
                                        fprintf(fp, "call: lvalue callsuffix at line %d --> %s\n", yylineno, yytext);
                                        $1 = emit_iftableitem($1, yylineno); //In case it was a table item too
                                        printf("Enterd call: lvalue callsufix\n");
                                        if($2->method){
                                          //printf("Entered if check\n");
                                          expr* t = $1;
                                          $1 = emit_iftableitem(member_item(t, $2->name, yylineno), yylineno);
                                          expr* tmp = $2->elist;
                                          while( (tmp->next) != NULL){
                                            //printf("tmp = %s\n", tmp->sym->name);
                                            tmp = tmp->next;
                                          }
                                          tmp->next = t; //Insert as first argument(reserved, so last)
                                        }
                                        $$ = make_call($1, $2->elist, yylineno);
                                      }
            |L_PAR funcdef R_PAR L_PAR elist R_PAR    {
                                                        fprintf(fp, "call: (funcdef) (elist) at line %d --> %s\n", yylineno, yytext);
                                                        printf("(funcdef)(elist)\n");
                                                        expr* func = newexpr(programfunc_e);
                                                        func->sym = $2;
                                                        $$ = make_call(func, $5, yylineno);
                                                      }
            ;

callsuffix:	normcall          {
                                fprintf(fp, "callsuffix: normcall at line %d --> %s\n", yylineno, yytext);
                                $$ = $1;
                                printf("Entered normcall\n");
                              }
			      |methodcall       {
                                fprintf(fp, "callsuffix: methodcall at line %d --> %s\n", yylineno, yytext);
                                printf("Entered callsufix: methodcall\n");
                                $$ = $1;
                              }
			      ;

normcall:	L_PAR elist R_PAR   {
                                fprintf(fp, "normcall: (elist) at line %d --> %s\n", yylineno, yytext);
                                $$->elist = $2;
                                $$->method = 0;
                                $$->name = NULL;
                              }
			    ;

methodcall: DDOT ID L_PAR elist R_PAR {
                                        fprintf(fp, "methodcall: ..ID (elist) at line %d --> %s\n", yylineno, yytext);
                                        printf("methodcall: ..id(elist)\n");
                                        $$->elist = $4;
                                        $$->method = 1;
                                        $$->name = $2;
                                      }
				    ;

elist:      expr                { fprintf(fp, "elist: expr at line %d --> %s\n", yylineno, yytext);}
		        |expr COMMA elist 	{
								                  fprintf(fp, "elist: expr, elist at line %d --> %s\n", yylineno, yytext);
                                  $1->next = $3;
                                  /*
                                  expr* tmp = $3;
                                  while(tmp->next != NULL){
                                    tmp = tmp->next;
                                  }
                                  tmp->next = $1;
                                  $$ = $3;
                                  */
                                }
 		        |					          {
								                  fprintf(fp, "elist: empty at line %d --> %s\n", yylineno, yytext);
                                  $$ = NULL;
                                }
 		        ;

tablemake:	L_BR elist R_BR     {
                                  fprintf(fp, "tablemake: [elist] at line %d --> %s\n", yylineno, yytext);
                                  printf("Entered tablemake: [elist]\n");
                                  int i = 0;
                                  expr* tmp = $2;
                                  expr* t = newexpr(newtable_e);
                                  t->sym = newtemp();
                                  emit(tablecreate, t, NULL, NULL, -1, yylineno);
                                  while(tmp != NULL){
                                    emit(tablesetelem, t, newexpr_constnum(i++), tmp, -1, yylineno);
                                    tmp = tmp->next;
                                  }
                                  $$ = t;
                                }
 				    |L_BR indexed R_BR {
					 							          fprintf(fp, "tablemake: [indexed] at line %d --> %s\n", yylineno, yytext);
                                  printf("Entered tablemake[indexed]\n");
                                  //printf("{%s:%d}\n", $2->strConst, (int)$2->index->numConst);
                                  expr* t = newexpr(newtable_e);
                                  t->sym = newtemp();
                                  expr* tmp = $2;
                                  emit(tablecreate, t, NULL, NULL, -1, yylineno);
                                  while(tmp != NULL){
                                    emit(tablesetelem, t, tmp, tmp->index, -1, yylineno);
                                    tmp = tmp->next;
                                  }
                                  $$ = t;
                                }
				      ;

indexed:	    indexedelem                 {
                                            fprintf(fp, "indexed: indexedelem at line %d --> %s\n", yylineno, yytext);
                                            printf("Entered indexed: indexelem\n");
                                            $$ = $1;
                                            //printf("indexedelem($1) = %s\n", $1->strConst);
                                            //printf("index = %d\n",(int)$1->index->numConst);
                                          }
			        |indexedelem COMMA indexed  {
                                            fprintf(fp, "indexed: indexedelem, indexed at line %d --> %s\n", yylineno, yytext);
                                            printf("Entered indexed: indexelem, indexed\n");
                                            //printf("indexedelem($1) = %s\n", $1->strConst);
                                            $1->next = $3;
                                          }
			        ;

indexedelem:	LCURLY_BR expr COLON expr RCURLY_BR {
                                                    fprintf(fp, "indexedelem: {expr:expr} at line %d --> %s\n", yylineno, yytext);
                                                    printf("Entered indexedelem: {expr:expr}\n");
                                                    //printf("{%s:%d}\n", $2->strConst, (int)$4->numConst);
                                                    $2->index = $4;
                                                    $$ = $2;
                                                  }
				      ;

block:        LCURLY_BR	  {
                            fprintf(fp, "block: LCURLY_BR at line %d --> %s\n", yylineno, yytext);
                            currentscope++;
                          }
              RCURLY_BR   {
                            fprintf(fp, "block: LCURLY_BR RCURLY_BR at line %d --> %s\n", yylineno, yytext);
                            hideScope(currentscope);
                            currentscope--;
                          }
              |LCURLY_BR	{
                            fprintf(fp, "block: LCURLY_BR at line %d --> %s\n", yylineno, yytext);
                            currentscope++;
                          }
              stmtlist  	{	fprintf(fp, "block: LCURLY_BR  stmtlist at line %d --> %s\n", yylineno, yytext);}
			        RCURLY_BR	{
                          fprintf(fp, "block: LCURLY_BR stmtlist RCURLY_BR at line %d --> %s\n", yylineno, yytext);
                          hideScope(currentscope);
                          currentscope--;
                        }
			        ;

funcdef:      funcprefix funcargs funcbody	{
                                              fprintf(fp, "funcdef: funcprefix funcargs funcbody at line %d --> %s\n", yylineno, yytext);
                                              exitscopespace();
                                              $1->totalLocals = $3;
                                              int offset = pop(scopeoffsetStack); // pop and get pre scope offset
                                              restorecurrscopeoffset(offset);
                                              $$ = $1;
                                              emit(funcend, NULL, NULL, lvalue_expr($1), -1, yylineno);
                                            }
				      ;

funcprefix:		FUNCTION funcname	            {
                                              fprintf(fp, "funcprefix: FUNCTION funcname at line %d --> %s\n", yylineno, yytext);
                                              $$ = hashInsert($2, currentscope, yylineno, programfunc_s, currscopespace(), currscopeoffset());
                                              $$->iaddress = nextquadlabel();
                                              emit(funcstart, NULL, NULL, lvalue_expr($$), -1, yylineno);
                                              push(scopeoffsetStack, currscopeoffset()); // Save current offset
                                              enterscopespace();
                                              resetformalargsoffset();
                                            }
              ;

funcname:     ID	                          {
                                              fprintf(fp, "funcname: ID at line %d --> %s\n", yylineno, yytext);
                                              $$ = $1; // Works
                                              printf("Funcname = %s\n", $$);
                                            }
				      |	                            {
                                              fprintf(fp, "funcname: empty at line %d --> %s\n", yylineno, yytext);
                                              $$ = newtempfuncname(); // Works
                                              printf("Funcname = %s\n", $$);
                                            }
				      ;

funcargs:		   L_PAR	                      {	currentscope++;}
				       idlist
				       R_PAR	                      {
                                              fprintf(fp, "funcargs: (idlist) at line %d --> %s\n", yylineno, yytext);
                                              currentscope--;
                                              enterscopespace();
                                              resetfunctionlocalsoffset();
                                            }
				;

funcbody:     block	{
                      fprintf(fp, "funcbody: block at line %d --> %s\n", yylineno, yytext);
                      $$ = currscopeoffset();
                      exitscopespace();
                    }
				      ;

const: REAL 		{
                  fprintf(fp, "const: REAL at line %d --> %s\n", yylineno, yytext);
                  printf("const: REAL\n");
                  printf("REAL($1) = %f\n", $1);
                  $$ = newexpr_constnum($1);
                }
			 |INTEGER	{
                  fprintf(fp, "const: INTEGER at line %d --> %s\n", yylineno, yytext);
                  printf("const: INTEGER\n");
                  printf("REAL($1) = %d\n", $1);
                  $$ = newexpr_constnum($1);
                }
       |STRING 	{
                  fprintf(fp, "const: FLEX_STRING at line %d --> %s\n", yylineno, yytext);
                  $$ = newexpr_conststring($1);
                }
       |NIL		  {	fprintf(fp, "const: NIL at line %d --> %s\n", yylineno, yytext);}
       |TRUE		{	fprintf(fp, "const: TRUE at line %d --> %s\n", yylineno, yytext);}
       |FALSE 	{	fprintf(fp, "const: FALSE at line %d --> %s\n", yylineno, yytext);}
       ;

idlist:		ID	{
                fprintf(fp, "idlist: ID at line %d --> %s\n", yylineno, yytext);
                symbol *sym = lookup(yylval.stringValue, currentscope);
                if(sym == NULL){
                  sym = hashInsert(yylval.stringValue, currentscope, yylineno, var_s, currscopespace(), currscopeoffset());
                  inccurrscopeoffset();
                  //printf("Inserted symbol %s\n", yylval.stringValue);
                }
                else{
                  printf("Symbol %s already defined\n", sym->name);
                  //MUST CHECK ACCESSIBILITY
                }
              }
          |idlist COMMA ID 	{
                              fprintf(fp, "idlist: idlist COMMA ID at line %d --> %s\n", yylineno, yytext);
                              symbol *sym = lookup(yylval.stringValue, currentscope);
                              if(sym == NULL){
                                sym = hashInsert(yylval.stringValue, currentscope, yylineno, var_s, currscopespace(), currscopeoffset());
                                inccurrscopeoffset();
                                //printf("Inserted symbol %s\n", yylval.stringValue);
                              }
                              else {
                                printf("Symbol %s already defined\n", sym->name);
                                //MUST CHECK ACCESSIBILITY
                              }
                            }
			    |
          ;

ifprefix: IF L_PAR expr R_PAR           {
                                          fprintf(fp, "ifprefix: if(expr) at line %d --> %s\n", yylineno, yytext);
                                          emit(if_eq, $3, newexpr_constbool(1), NULL, nextquad()+2, yylineno);
                                          $$ = nextquad();
                                          emit(jump, NULL, NULL, NULL, 0, yylineno);
                                        }

elseprefix: ELSE                        {
                                          fprintf(fp, "elseprefix: else at line %d --> %s\n", yylineno, yytext);
                                          $$ = nextquad();
                                          emit(jump, NULL, NULL, NULL, 0, yylineno);
                                        }

ifstmt: ifprefix stmt                   {
                                          fprintf(fp, "ifstmt: ifprefix stmt at line %d --> %s\n", yylineno, yytext);
                                          patchlabel($1, nextquad());
                                        }
        |ifprefix stmt elseprefix stmt  {
                                          fprintf(fp, "ifstmt: ifprefix stmt elseprefix stmt at line %d --> %s\n", yylineno, yytext);
                                          patchlabel($1, $3+1);
                                          patchlabel($3, nextquad());
                                        }

        ;




whilestmt:  WHILE L_PAR expr R_PAR  {	inLoop = 1;}
			      stmt 	                  {	fprintf(fp, "whilestmt: WHILE L_PAR expr R_PAR stmt at line %d --> %s\n", yylineno, yytext);}
			      ;

forstmt:  	FOR L_PAR elist SEMICOLON expr SEMICOLON elist R_PAR 	{	inLoop = 1;}
			      stmt { fprintf(fp, "forstm: for(elist; expr; elist) stmt at line %d --> %s\n", yylineno, yytext);}
			      ;

returnstmt:	RETURN SEMICOLON    {
                                  fprintf(fp, "returnstmt: RETURN SEMICOLON at line %d --> %s\n", yylineno, yytext);
                                  if (inFunc == 0)
                                    addError("Use of return while not in function", "", yylineno);
                                }
			      |RETURN expr SEMICOLON   {
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
    fp = fopen("syntaxAnalysis", "w+");

    if(argc > 1){
        if(!(yyin = fopen(argv[1], "r"))){
            fprintf(stderr, "Cannot read file: %s\n", argv[1]);
            return 1;
        }
    }
    else{
        yyin = stdin;
    }
    scopeoffsetStack = initStack();
    initialize();
    yyparse();
    printQuads();
    printScopeList();
    printErrorList();

    fclose(fp);
    return 0;
}
