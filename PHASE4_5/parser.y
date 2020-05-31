 %{
  #include <stdio.h>
  #include "stack.h"
  #include "target.h"

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
  unsigned int contcount = 0;
  unsigned int funcprefix = 0;
  unsigned int breakcount = 0;
  unsigned int betweenFunc = 0;
  unsigned int loopcounter = 0;
  unsigned int currentscope = 0;
  unsigned int funcJump = 0;
  offsetStack *scopeoffsetStack = NULL;
  counterStack *loopcounterStack = NULL;
  stmt_t *breakpointer = NULL;
%}

//%defines

%union{
  int intValue;
  double realValue;
  char* stringValue;
	unsigned int unsignedValue;
  struct symbol* symNode;
	struct expr* exprNode;
	struct call* callNode;
  struct stmt_t* stmtNode;
  struct forprefix_t* forprefixNode;
}

%type <stringValue> funcname
%type <forprefixNode> forprefix
%type <symNode> funcprefix funcdef
%type <callNode> callsuffix normcall methodcall
%type <stmtNode> stmt stmtlist break continue ifstmt forstmt whilestmt loopstmt
%type <unsignedValue> funcbody ifprefix elseprefix whilestart whilecond N M
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

stmtlist: stmt              {
                              fprintf(fp, "stmtlist: stmt at line %d --> %s\n", yylineno, yytext);
                              printf("Entered stmtlist: stmt\n");
                              //fprintf(fp, "$1->breaklist = %d\n", $1->breaklist);
                              $$ = $1;
                            }
          |stmt stmtlist		{
                              fprintf(fp, "stmtlist: stmtlist stmt at line %d --> %s\n", yylineno, yytext);
                              printf("Entered stmtlist: stmt stmtlist\n");
                              printf("Breakcount = %d | Contcount = %d\n", breakcount, contcount);
                              stmt_t *tmp = (stmt_t*) malloc(sizeof(stmt_t));
                              if(breakcount > 1){
                                printf("Entered break check\n");
                                tmp->breaklist = mergelist($1->breaklist, $2->breaklist);
                                //breakcount = 0;
                              }
                              if(contcount > 1){
                                printf("Entered continue check\n");
                                tmp->contlist = mergelist($1->contlist, $2->contlist);
                                //contcount = 0;
                              }
                              $$ = tmp;
                            }
          ;

stmt:     expr SEMICOLON        {
                                  fprintf(fp, "stmt: expr SEMICOLON at line %d --> %s\n", yylineno, yytext);
                                  //fprintf(fp, "Entered stmt: expr;\n");
                                  //fprintf(fp, "$$->breaklist = %d\n", $$->breaklist);
                                }
          |ifstmt               { fprintf(fp, "stmt: ifstmt at line %d --> %s\n", yylineno, yytext);
                                  printf("Entered stmt: ifstmt\n");
                                  $$ = $1;
                                }
          |whilestmt            { fprintf(fp, "stmt: whilestmt at line %d --> %s\n", yylineno, yytext);
                                  printf("Entered stmt: whilestmt\n");
                                  $$ = $1;
                                }
          |forstmt				      {	
                                  fprintf(fp, "stmt: forstmt at line %d --> %s\n", yylineno, yytext);
                                  $$ = $1;
                                }
          |returnstmt			      {	fprintf(fp, "stmt: returnstmt at line %d --> %s\n", yylineno, yytext);}
          |break SEMICOLON		  {
										              fprintf(fp, "stmt: BREAK SEMICOLON at line %d --> %s\n", yylineno, yytext);
                                  printf("Entered stmt: break;\n");
                                  printf("breakcount = %d\n", breakcount);
                                  printf("$1->breaklist = %d\n", $1->breaklist+1);
                                  $$ = $1;
                                }
          |continue SEMICOLON		{
										              fprintf(fp, "stmt: CONTINUE SEMICOLON at line %d --> %s\n", yylineno, yytext);
                                  printf("Entered stmt: continue\n");
                                  printf("contcount = %d\n", contcount);
                                  $$ = $1;
                                }
          |block                {	fprintf(fp, "stmt: block at line %d --> %s\n", yylineno, yytext);}
          |funcdef              {	fprintf(fp, "stmt: funcdef at line %d --> %s\n", yylineno, yytext);}
          |SEMICOLON            {	fprintf(fp, "stmt: SEMICOLON at line %d --> %s\n", yylineno, yytext);}
          ;

expr:     assignexpr            { fprintf(fp, "expr: assignexpr at line %d --> %s\n", yylineno, yytext);}
          |expr PLUS expr       {
                                  fprintf(fp, "expr: expr PLUS expr at line %d --> %s\n", yylineno, yytext);
                                  if(illegalop($1, $3))
                                    addError("Error, illegal arithmetic operation", "", yylineno);
                                  else{
                                    if($1->type == constnum_e && $3->type == constnum_e)
                                      $$ = newexpr(constnum_e);
                                    else
                                      $$ = newexpr(arithexpr_e);
                                    $$->sym = istempexpr($1) ? $1->sym : newtemp();
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
                                    $$->sym = istempexpr($1) ? $1->sym : newtemp();
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
                                    $$->sym = istempexpr($1) ? $1->sym : newtemp();
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
                                    $$->sym = istempexpr($1) ? $1->sym : newtemp();
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
                                    $$->sym = istempexpr($1) ? $1->sym : newtemp();
                                    emit(mod, $1, $3, $$, -1, yylineno);
                                  }
                                }
          |expr GREATER expr		{
                                  fprintf(fp, "expr: expr GREATER expr at line %d --> %s\n", yylineno, yytext);

                                  $$ = newexpr(boolexpr_e);
                                  $$->sym = istempexpr($1) ? $1->sym : newtemp();
                                  emit(if_greater, $1, $3, NULL, nextquad()+3, yylineno);
                                  emit(assign, newexpr_constbool(0), NULL, $$, -1, yylineno);
                                  emit(jump, NULL, NULL, NULL, nextquad()+2, yylineno);
                                  emit(assign, newexpr_constbool(1), NULL, $$, -1, yylineno);
                                }
          |expr GREATER_EQ expr	{
                                  fprintf(fp, "expr: expr GREATER_EQ expr at line %d --> %s\n", yylineno, yytext);

                                  $$ = newexpr(boolexpr_e);
                                  $$->sym = istempexpr($1) ? $1->sym : newtemp();
                                  emit(if_greatereq, $1, $3, NULL, nextquad()+3, yylineno);
                                  emit(assign, newexpr_constbool(0), NULL, $$, -1, yylineno);
                                  emit(jump, NULL, NULL, NULL, nextquad()+2, yylineno);
                                  emit(assign, newexpr_constbool(1), NULL, $$, -1, yylineno);
                                }
          |expr LESS expr       {
                                  fprintf(fp, "expr: expr LESS expr at line %d --> %s\n", yylineno, yytext);

                                  $$ = newexpr(boolexpr_e);
                                  $$->sym = istempexpr($1) ? $1->sym : newtemp();
                                  emit(if_less, $1, $3, NULL, nextquad()+3, yylineno);
                                  emit(assign, newexpr_constbool(0), NULL, $$, -1, yylineno);
                                  emit(jump, NULL, NULL, NULL, nextquad()+2, yylineno);
                                  emit(assign, newexpr_constbool(1), NULL, $$, -1, yylineno);
                                }
          |expr LESS_EQ expr		{
                                  fprintf(fp, "expr: expr LESS_EQ expr at line %d --> %s\n", yylineno, yytext);

                                  $$ = newexpr(boolexpr_e);
                                  $$->sym = istempexpr($1) ? $1->sym : newtemp();
                                  emit(if_lesseq, $1, $3, NULL, nextquad()+3, yylineno);
                                  emit(assign, newexpr_constbool(0), NULL, $$, -1, yylineno);
                                  emit(jump, NULL, NULL, NULL, nextquad()+2, yylineno);
                                  emit(assign, newexpr_constbool(1), NULL, $$, -1, yylineno);
                                }
          |expr EQUAL expr      {
                                  fprintf(fp, "expr: expr EQUAL expr at line %d --> %s\n", yylineno, yytext);

                                  $$ = newexpr(boolexpr_e);
                                  $$->sym = istempexpr($1) ? $1->sym : newtemp();
                                  emit(if_eq, $1, $3, NULL, nextquad()+3, yylineno);
                                  emit(assign, newexpr_constbool(0), NULL, $$, -1, yylineno);
                                  emit(jump, NULL, NULL, NULL, nextquad()+2, yylineno);
                                  emit(assign, newexpr_constbool(1), NULL, $$, -1, yylineno);
                                }
          |expr NOT_EQUAL expr  {
                                  fprintf(fp, "expr: expr NOT_EQUAL expr at line %d --> %s\n", yylineno, yytext);

                                  $$ = newexpr(boolexpr_e);
                                  $$->sym = istempexpr($1) ? $1->sym : newtemp();
                                  emit(if_noteq, $1, $3, NULL, nextquad()+3, yylineno);
                                  emit(assign, newexpr_constbool(0), NULL, $$, -1, yylineno);
                                  emit(jump, NULL, NULL, NULL, nextquad()+2, yylineno);
                                  emit(assign, newexpr_constbool(1), NULL, $$, -1, yylineno);
                                }
          |expr AND expr        {
                                  fprintf(fp, "expr: expr AND expr at line %d --> %s\n", yylineno, yytext);

                                    $$ = newexpr(boolexpr_e);
                                    $$->sym = istempexpr($1) ? $1->sym : newtemp();
                                    emit(and, $1, $3, $$, -1, yylineno);
                                }
          |expr OR expr			    {
                                  fprintf(fp, "expr: expr OR expr at line %d --> %s\n", yylineno, yytext);
                                    $$ = newexpr(boolexpr_e);
                                    $$->sym = istempexpr($1) ? $1->sym : newtemp();
                                    emit(or, $1, $3, $$, -1, yylineno);
                                }
          |term					        {	
                                  fprintf(fp, "expr: term at line %d --> %s\n", yylineno, yytext);
                                  $$ = $1;  
                                }
          ;

term:     L_PAR expr R_PAR			    {
                                      fprintf(fp, "term: L_PAR expr R_PAR at line %d --> %s\n", yylineno, yytext);
                                      $$ = $2;
                                    }
          |MINUS expr %prec UMINUS	{
                                      fprintf(fp, "term: MINUS expr at line %d --> %s\n", yylineno, yytext);
                                      check_arith($2, "-expr");
                                      $$ = newexpr(arithexpr_e);
                                      $2->numConst = -1 * $2->numConst; 
                                      $$->sym = istempexpr($2) ? $2->sym : newtemp();
                                      emit(mul, $2, NULL, $$, -1, yylineno);
                                    }
          |NOT expr	                {
                                      fprintf(fp, "term: NOT expr at line %d --> %s\n", yylineno, yytext);
                                      $$ = newexpr(boolexpr_e);
                                      $$->sym = istempexpr($2) ? $2->sym : newtemp();
                                      emit(not, $2, NULL, $$, -1, yylineno);
                                    }
          |INCR lvalue              {
                                      fprintf(fp, "term: INCR lvalue at line %d --> %s\n", yylineno, yytext);
                                      check_arith($2, "++lvalue");
                                      if($2->type == tableitem_e){
                                        $$ = emit_iftableitem($2, yylineno);
                                        emit(add, $$, newexpr_constnum(1), $$, -1, yylineno);
                                        emit(tablesetelem, $2->index, $$, $2, -1, yylineno);

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
                                        emit(tablesetelem, $1->index, val, $1, -1, yylineno);
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
                                        emit(tablesetelem, $2->index, $$, $2, -1, yylineno);
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
                                        emit(tablesetelem, $1->index, val, $1, -1, yylineno);
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
                                    emit(tablesetelem, $1->index, $3, $1, -1, yylineno);
                                    $$ = emit_iftableitem($1, yylineno);
                                    $$->type = assignexpr_e;
                                  }
                                  else{
                                    emit(assign, $3, NULL, $1, -1, yylineno);
                                    $$ = newexpr(assignexpr_e);
                                    $$->sym = istempexpr($3) ? $3->sym : newtemp();
                                    emit(assign, $1, NULL, $$, -1, yylineno);
                                  }
                                }
            ;

primary:    lvalue                { fprintf(fp, "primary: lvalue at line %d --> %s\n", yylineno, yytext);
										                $$ = emit_iftableitem($1, yylineno);
                                  }
            |call                 { 
                                    fprintf(fp, "primary: call at line %d --> %s\n", yylineno, yytext);
                                    $$ = $1;
                                  }
			      |tablemake            { fprintf(fp, "primary: tablemake at line %d --> %s\n", yylineno, yytext);}
			      |L_PAR funcdef R_PAR	{
                                    fprintf(fp, "primary: L_PAR funcdef R_PAR at line %d --> %s\n", yylineno, yytext);
                                    $$ = newexpr(programfunc_e);
                                    $$->sym = $2;
                                  }
			      |const					      {
                                    fprintf(fp, "primary: const at line %d --> %s\n", yylineno, yytext);
                                    $$ = $1;
                                  }
			      ;

lvalue:     ID          {
                          fprintf(fp, "lvalue: ID at line %d --> %s\n", yylineno, yylval.stringValue);
                          symbol *sym = lookup(yylval.stringValue, currentscope);
                          if(sym == NULL){
                            sym = hashInsert(yylval.stringValue, currentscope, yylineno, var_s, currscopespace(), currscopeoffset());
                            inccurrscopeoffset();
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
                            }
                          }
                          else
                            if(sym->type == programfunc_s)
                              printf("Warning sym is a function\n");
                          $$ = lvalue_expr(sym);
                        }

			|DCOLON ID		    {
								          symbol *sym = scopelookup(yytext,0);
                          if(sym == NULL)
                            fprintf(fp, "Symbol %s does not exist\n", yylval.stringValue);
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
                                        $$ = make_call($1, $3, yylineno);
                                      }
            |lvalue callsuffix				{
                                        fprintf(fp, "call: lvalue callsuffix at line %d --> %s\n", yylineno, yytext);
                                        $1 = emit_iftableitem($1, yylineno);
                                        if($2->method){
                                          expr* t = $1;
                                          $1 = emit_iftableitem(member_item(t, $2->name, yylineno), yylineno);
                                          expr* tmp = $2->elist;
                                          while( (tmp->next) != NULL){
                                            tmp = tmp->next;
                                          }
                                          tmp->next = t;
                                        }
                                        $$ = make_call($1, $2->elist, yylineno);
                                      }
            |L_PAR funcdef R_PAR L_PAR elist R_PAR    {
                                                        fprintf(fp, "call: (funcdef) (elist) at line %d --> %s\n", yylineno, yytext);
                                                        expr* func = newexpr(programfunc_e);
                                                        func->sym = $2;
                                                        $$ = make_call(func, $5, yylineno);
                                                      }
            ;

callsuffix:	normcall          {
                                fprintf(fp, "callsuffix: normcall at line %d --> %s\n", yylineno, yytext);
                                $$ = $1;
                              }
			      |methodcall       {
                                fprintf(fp, "callsuffix: methodcall at line %d --> %s\n", yylineno, yytext);
                                $$ = $1;
                              }
			      ;

normcall:	L_PAR elist R_PAR   {
                                fprintf(fp, "normcall: (elist) at line %d --> %s\n", yylineno, yytext);
                                callStr* tmp = (callStr*) malloc(sizeof(expr));
                                tmp->elist = $2;
                                tmp->method = 0;
                                tmp->name = NULL;
                                $$ = tmp;
                              }
			    ;

methodcall: DDOT ID L_PAR elist R_PAR {
                                        fprintf(fp, "methodcall: ..ID (elist) at line %d --> %s\n", yylineno, yytext);
                                        callStr* tmp = (callStr*) malloc(sizeof(expr));
                                        tmp->elist = $4;
                                        tmp->method = 1;
                                        tmp->name = $2;
                                        $$ = tmp;
                                      }
				    ;

elist:      expr                { 
                                  fprintf(fp, "elist: expr at line %d --> %s\n", yylineno, yytext);
                                  $$ = $1;
                                }
		        |expr COMMA elist 	{
								                  fprintf(fp, "elist: expr, elist at line %d --> %s\n", yylineno, yytext);
                                  expr* tmp = $3;
                                  while(tmp->next != NULL){
                                    tmp = tmp->next;
                                  }
                                  tmp->next = $1;
                                  $$ = $3;
                                }
 		        |					          {
								                  fprintf(fp, "elist: empty at line %d --> %s\n", yylineno, yytext);
                                  $$ = NULL;
                                }
 		        ;

tablemake:	L_BR elist R_BR     {
                                  fprintf(fp, "tablemake: [elist] at line %d --> %s\n", yylineno, yytext);
                                  int i = 0;
                                  expr* tmp;
                                  expr* prev = NULL;
                                  expr* curr = $2;
                                  expr* next = NULL;
                                  expr* t = newexpr(newtable_e);

                                  //Reverse elist
                                  while(curr != NULL){
                                    next = curr->next;
                                    curr->next = prev;
                                    prev = curr;
                                    curr = next;
                                  }
                                  tmp = prev;

                                  if($2 == NULL)
                                    t->sym = newtemp();
                                  else
                                    t->sym = istempexpr($2) ? $2->sym : newtemp();
                                  emit(tablecreate, t, NULL, NULL, -1, yylineno);
                                  while(tmp != NULL){
                                    emit(tablesetelem, newexpr_constnum(i++), tmp, t, -1, yylineno);
                                    tmp = tmp->next;
                                  }
                                  $$ = t;
                                }
 				    |L_BR indexed R_BR {
					 							          fprintf(fp, "tablemake: [indexed] at line %d --> %s\n", yylineno, yytext);
                                  expr* t = newexpr(newtable_e);
                                  t->sym = istempexpr($2) ? $2->sym : newtemp();
                                  expr* tmp = $2;
                                  emit(tablecreate, t, NULL, NULL, -1, yylineno);
                                  while(tmp != NULL){
                                    emit(tablesetelem, tmp, tmp->index, t, -1, yylineno);
                                    tmp = tmp->next;
                                  }
                                  $$ = t;
                                }
				      ;

indexed:	    indexedelem                 {
                                            fprintf(fp, "indexed: indexedelem at line %d --> %s\n", yylineno, yytext);
                                            $$ = $1;
                                          }
			        |indexedelem COMMA indexed  {
                                            fprintf(fp, "indexed: indexedelem, indexed at line %d --> %s\n", yylineno, yytext);
                                            $1->next = $3;
                                          }
			        ;

indexedelem:	LCURLY_BR expr COLON expr RCURLY_BR {
                                                    fprintf(fp, "indexedelem: {expr:expr} at line %d --> %s\n", yylineno, yytext);
                                                    $2->index = $4;
                                                    $$ = $2;
                                                  }
				      ;

block:  blockstart blockend
        |blockstart	stmtlist blockend	{	
                                        fprintf(fp, "block: LCURLY_BR  stmtlist at line %d --> %s\n", yylineno, yytext);
                                        if(loopcounter != 0){
                                          breakpointer = $2;
                                          printf("breakpointer->breaklist = %d | breakpointer->contlist = %d\n", breakpointer->breaklist, breakpointer->contlist);
                                        }
                                      }
			  ;

blockstart: LCURLY_BR {
                        fprintf(fp, "block: LCURLY_BR at line %d --> %s\n", yylineno, yytext);
                        fprintf(fp, "Entered block\n");
                        currentscope++;
                      }

blockend: RCURLY_BR   {
                        fprintf(fp, "block: LCURLY_BR RCURLY_BR at line %d --> %s\n", yylineno, yytext);
                        hideScope(currentscope);
                        currentscope--;
                      }

funcdef:  funcprefix funcargs funcblockstart funcbody	funcblockend {
                                              fprintf(fp, "funcdef: funcprefix funcargs funcbody at line %d --> %s\n", yylineno, yytext);
                                              exitscopespace();
                                              $1->totalLocals = $4;
                                              int offset = popOffset(&scopeoffsetStack); // pop and get pre scope offset
                                              restorecurrscopeoffset(offset);
                                              $$ = $1;
                                              emit(funcend, NULL, NULL, lvalue_expr($1), -1, yylineno);
                                              patchlabel(funcJump, nextquad());
                                            }
				  ;

funcprefix:		FUNCTION funcname	            {
                                              fprintf(fp, "funcprefix: FUNCTION funcname at line %d --> %s\n", yylineno, yytext);
                                              $$ = hashInsert($2, currentscope, yylineno, programfunc_s, currscopespace(), currscopeoffset());
                                              $$->iaddress = nextquadlabel();
                                              funcJump = nextquad();
                                              emit(jump, NULL, NULL, NULL, 0, yylineno);
                                              emit(funcstart, NULL, NULL, lvalue_expr($$), -1, yylineno);
                                              pushOffset(&scopeoffsetStack, currscopeoffset()); // Save current offset
                                              enterscopespace();
                                              resetformalargsoffset();
                                            }
              ;

funcname:     ID	                          {
                                              fprintf(fp, "funcname: ID at line %d --> %s\n", yylineno, yytext);
                                              $$ = $1;
                                            }
				      |	                            {
                                              fprintf(fp, "funcname: empty at line %d --> %s\n", yylineno, yytext);
                                              $$ = newtempfuncname();
                                            }
				      ;

funcargs:		  L_PAR	                        {	currentscope++; inFunc++;}
				      idlist
				      R_PAR	                        {
                                              fprintf(fp, "funcargs: (idlist) at line %d --> %s\n", yylineno, yytext);
                                              currentscope--;
                                              enterscopespace();
                                              resetfunctionlocalsoffset();
                                            }
				      ;

funcbody: block {
                  fprintf(fp, "funcbody: block at line %d --> %s\n", yylineno, yytext);
                  inFunc--;
                  $$ = currscopeoffset();
                  exitscopespace();
                }

funcblockstart: {
                  fprintf(fp, "Entered funcblockstart\n");
                  printf("loopcounter before push = %d\n", loopcounter);
                  pushCounter(&loopcounterStack, loopcounter); 
                  loopcounter = 0;
                  printf("loopcounter after push = %d\n", loopcounter);
                }

funcblockend:   { 
                  fprintf(fp, "Entered funcblockend\n");
                  printf("loopcounter before pop = %d\n", loopcounter);
                  loopcounter = popCounter(&loopcounterStack);
                  printf("loopounter after pop = %d\n", loopcounter);
                }

const: REAL 		{
                  fprintf(fp, "const: REAL at line %d --> %s\n", yylineno, yytext);
                  $$ = newexpr_constnum($1);
                }
			 |INTEGER	{
                  fprintf(fp, "const: INTEGER at line %d --> %s\n", yylineno, yytext);
                  $$ = newexpr_constnum($1);
                }
       |STRING 	{
                  fprintf(fp, "const: FLEX_STRING at line %d --> %s\n", yylineno, yytext);
                  $$ = newexpr_conststring($1);
                }
       |NIL		  {	
                  fprintf(fp, "const: NIL at line %d --> %s\n", yylineno, yytext);
                  $$ = newexpr_conststring("nil");
                }
       |TRUE		{	fprintf(fp, "const: TRUE at line %d --> %s\n", yylineno, yytext);
                  $$ = newexpr_conststring("true");
                }
       |FALSE 	{	fprintf(fp, "const: FALSE at line %d --> %s\n", yylineno, yytext);
                  $$ = newexpr_conststring("false");
                }
       ;

idlist:		ID	{
                fprintf(fp, "idlist: ID at line %d --> %s\n", yylineno, yytext);
                symbol *sym = lookup(yylval.stringValue, currentscope);
                if(sym == NULL){
                  sym = hashInsert(yylval.stringValue, currentscope, yylineno, var_s, currscopespace(), currscopeoffset());
                  inccurrscopeoffset();
                }
              }
          |idlist COMMA ID 	{
                              fprintf(fp, "idlist: idlist COMMA ID at line %d --> %s\n", yylineno, yytext);
                              symbol *sym = lookup(yylval.stringValue, currentscope);
                              if(sym == NULL){
                                sym = hashInsert(yylval.stringValue, currentscope, yylineno, var_s, currscopespace(), currscopeoffset());
                                inccurrscopeoffset();
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
                                          printf("Entered ifstmt: ifprefix stmt elseprefix stmt\n");
                                          patchlabel($1, $3+1);
                                          patchlabel($3, nextquad());
                                        }

whilestart: WHILE                       {
                                          fprintf(fp, "whilestart: WHILE at line %d --> %s\n", yylineno, yytext);
                                          printf("Entered whilestart\n");
                                          $$ = nextquad();
                                        }

whilecond: L_PAR expr R_PAR           {
                                          fprintf(fp, "whilesecond: (expr) at line %d --> %s\n", yylineno, yytext);
                                          printf("Entered whilecond\n");
                                          emit(if_eq, $2, newexpr_constbool(1), NULL, nextquad()+2, yylineno);
                                          $$ = nextquad();
                                          emit(jump, NULL, NULL, NULL, 0, yylineno);
                                        }

whilestmt: whilestart whilecond loopstmt {
                                          fprintf(fp, "whilestmt: while(expr) stmt at line %d --> %s\n", yylineno, yytext);
                                          printf("Entered whilestmt, line = %d\n", yylineno);
                                          emit(jump, NULL, NULL, NULL, $1, yylineno);
                                          patchlabel($2, nextquad());
                                          
                                          if(breakcount > 1){
                                            printf("$3->breaklist = %d\n", $3->breaklist+1);
                                            patchlist($3->breaklist, nextquad());
                                            breakcount = 0;
                                          }
                                          if(contcount > 1){
                                            printf("$3->contlist = %d\n", $3->contlist);
                                            patchlist($3->contlist, $1);
                                            contcount = 0;
                                          }
                                          $$ = $3;
                                        }

N:          {
              $$ = nextquad();
              emit(jump,NULL, NULL, NULL, 0, yylineno);
            }

M:          {
              $$ = nextquad();
            }

forprefix:  FOR L_PAR elist SEMICOLON M expr SEMICOLON  {
                                                          fprintf(fp, "forprefix: L_PAR elist SEMICOLON M expr SEMICOLON at line %d --> %s\n", yylineno, yytext);
                                                          printf("Enterd forprefix\n");
                                                          forprefix_t* tmp = (forprefix_t*) malloc(sizeof(forprefix_t));
                                                          tmp->test = $5;
                                                          tmp->enter = nextquad();
                                                          emit(if_eq, $6, newexpr_constbool(1), NULL, 0, yylineno);
                                                          $$ = tmp;
                                                        }

forstmt:  forprefix N elist R_PAR N loopstmt N          {
                                                          fprintf(fp, "forstm: for(elist; expr; elist) stmt at line %d --> %s\n", yylineno, yytext);
                                                          printf("Enterd forstm\n");
                                                          patchlabel($1->enter, $5+1); //true jump
                                                          patchlabel($2, nextquad()); //false jump
                                                          patchlabel($5, $1->test); //loop jump
                                                          patchlabel($7, $2+1); //closure jump

                                                          if(breakcount > 1){
                                                            printf("$6->breaklist = %d\n", $6->breaklist);  
                                                            patchlist($6->breaklist, nextquad());
                                                          }
                                                          if(contcount > 1){
                                                            printf("$6->contlist = %d\n", $6->contlist);
                                                            patchlist($6->contlist, $2+1);
                                                          }
                                                          $$ = $6;
                                                        }

returnstmt:	RETURN SEMICOLON    {
                                  fprintf(fp, "returnstmt: RETURN SEMICOLON at line %d --> %s\n", yylineno, yytext);
                                  if (inFunc == 0)
                                    addError("Use of return while not in function", "", yylineno);
                                  else
                                    emit(ret, NULL, NULL, NULL, -1, yylineno);
                                }
			      |RETURN expr SEMICOLON   {
                                        fprintf(fp, "returnstmt: RETURN expr SEMICOLON at line %d --> %s\n", yylineno, yytext);
                                        if (inFunc == 0)
                                          addError("Use of return while not in function", "", yylineno);
                                        else
                                          emit(ret, NULL, NULL, $2, -1, yylineno);
                                      }
			      ;

loopstart: { fprintf(fp, "loopstart: at line %d --> %s\n", yylineno, yytext); ++loopcounter;}

loopend:   { fprintf(fp, "loopend: at line %d --> %s\n", yylineno, yytext); --loopcounter;}

loopstmt: loopstart stmt loopend  { 
                                    fprintf(fp, "loopstmt: loopstart stmt loopend at line %d --> %s\n", yylineno, yytext);
                                    printf("Entered loopstmt: loopstart stmt loopend\n"); 
                                    
                                    if(breakcount != 0 || contcount != 0)
                                      if(breakpointer != NULL){
                                        printf("Some magic happens\n");
                                        $2 = breakpointer;
                                      }

                                    $$ = $2;
                                  }

break: BREAK        {
                      fprintf(fp, "break: BREAK at line %d --> %s\n", yylineno, yytext);
                      printf("Entered break\n");
                      printf("loopcounter = %d\n", loopcounter);
                      if(loopcounter == 0)
                        addError("Use of break while not in loop", "", yylineno);
                      else{
                        printf("Adding break jump\n");
                        breakcount++;
                        stmt_t* tmp = (stmt_t*) malloc(sizeof(stmt_t));
                        make_stmt(tmp);
                        printf("breaklist = %d\n", tmp->breaklist);
                        tmp->breaklist = newlist(nextquad());
                        emit(jump, NULL, NULL, NULL, 0, yylineno);
                        $$ = tmp;
                      }
                    }

continue: CONTINUE  {
                      fprintf(fp, "continue: CONTINUE at line %d --> %s\n", yylineno, yytext);
                      printf("Entered continue\n");
                      if(loopcounter == 0)
                        addError("Use of continue while not in loop", "", yylineno);
                      else{
                        printf("Adding continue jump\n");
                        contcount++;
                        stmt_t* tmp = (stmt_t*) malloc(sizeof(stmt_t));
                        make_stmt(tmp);
                        printf("contlist = %d\n", tmp->contlist);
                        tmp->contlist = newlist(nextquad());
                        emit(jump, NULL, NULL, NULL, 0, yylineno);
                        $$ = tmp;
                      }
                    }
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

    scopeoffsetStack = initOffsetStack();
    loopcounterStack = initCounterStack();

    initialize();
    yyparse();
    printQuads();
    printScopeList();
    printErrorList();

    exec_generate();
    patch_incomplete_jumps();
    printInstrucrtions();
    fclose(fp);
    return 0;
}