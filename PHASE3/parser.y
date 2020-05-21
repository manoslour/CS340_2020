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

program:  stmtlist  {	printf("promgram: stmtlist at line %d --> %s\n", yylineno, yytext);}
			|
			;

stmtlist: stmt              {
                              printf("stmtlist: stmt at line %d --> %s\n", yylineno, yytext);
                              printf("Entered stmtlist: stmt\n");
                              //printf("$1->breaklist = %d\n", $1->breaklist);
                              $$ = $1;
                            }
          |stmt stmtlist		{
                              printf("stmtlist: stmtlist stmt at line %d --> %s\n", yylineno, yytext);
                              printf("Entered stmtlist: stmt stmtlist\n");
                              printf("Breakcount = %d | Contcount = %d\n", breakcount, contcount);
                              stmt_t *tmp = (stmt_t*) malloc(sizeof(stmt_t));
                              if(breakcount > 1){
                                printf("Entered break check\n");
                                tmp->breaklist = mergelist($1->breaklist, $2->breaklist);
                                breakcount = 0;
                              }
                              if(contcount > 1){
                                printf("Entered continue check\n");
                                tmp->contlist = mergelist($1->contlist, $2->contlist);
                                contcount = 0;
                              }
                              $$ = tmp;
                            }
          ;

stmt:     expr SEMICOLON        {
                                  printf("stmt: expr SEMICOLON at line %d --> %s\n", yylineno, yytext);
                                  //printf("Entered stmt: expr;\n");
                                  //printf("$$->breaklist = %d\n", $$->breaklist);
                                }
          |ifstmt               { printf("stmt: ifstmt at line %d --> %s\n", yylineno, yytext);
                                  printf("Entered stmt: ifstmt\n");
                                  //printf("$1->breaklist = %d\n", $1->breaklist);
                                  $$ = $1;
                                }
          |whilestmt            { printf("stmt: whilestmt at line %d --> %s\n", yylineno, yytext);
                                  printf("Entered stmt: whilestmt\n");
                                  $$ = $1;
                                }
          |forstmt				      {	
                                  printf("stmt: forstmt at line %d --> %s\n", yylineno, yytext);
                                  $$ = $1;
                                }
          |returnstmt			      {	printf("stmt: returnstmt at line %d --> %s\n", yylineno, yytext);}
          |break SEMICOLON		  {
										              printf("stmt: BREAK SEMICOLON at line %d --> %s\n", yylineno, yytext);
                                  printf("Entered stmt: break;\n");
                                  printf("breakcount = %d\n", breakcount);
                                  //printf("$1->breaklist = %d\n", $1->breaklist);
                                  $$ = $1;
                                }
          |continue SEMICOLON		{
										              printf("stmt: CONTINUE SEMICOLON at line %d --> %s\n", yylineno, yytext);
                                  printf("Entered stmt: continue\n");
                                  printf("contcount = %d\n", contcount);
                                  $$ = $1;
                                }
          |block                {	printf("stmt: block at line %d --> %s\n", yylineno, yytext);}
          |funcdef              {	printf("stmt: funcdef at line %d --> %s\n", yylineno, yytext);}
          |SEMICOLON            {	printf("stmt: SEMICOLON at line %d --> %s\n", yylineno, yytext);}
          ;

expr:     assignexpr            {
                                  printf("expr: assignexpr at line %d --> %s\n", yylineno, yytext);
                                  printf("Entered expr: assignexpr;\n");
                                }
          |expr PLUS expr       {
                                  printf("expr: expr PLUS expr at line %d --> %s\n", yylineno, yytext);
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
                                  printf("expr: MINUS op expr at line %d --> %s\n", yylineno, yytext);
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
                                  printf("expr: expr MULT expr at line %d --> %s\n", yylineno, yytext);
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
                                  printf("expr: expr DIV expr at line %d --> %s\n", yylineno, yytext);
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
                                  printf("expr: expr MOD expr at line %d --> %s\n", yylineno, yytext);
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
                                  printf("expr: expr GREATER expr at line %d --> %s\n", yylineno, yytext);
                                  if(illegalop($1, $3))
                                    addError("Error, illegal boolean operation", "", yylineno);
                                  else{
                                    $$ = newexpr(boolexpr_e);
                                    $$->sym = istempexpr($1) ? $1->sym : newtemp();
                                    emit(if_greater, $1, $3, NULL, nextquad()+3, yylineno);
                                    emit(assign, newexpr_constbool(0), NULL, $$, -1, yylineno);
                                    emit(jump, NULL, NULL, NULL, nextquad()+2, yylineno);
                                    emit(assign, newexpr_constbool(1), NULL, $$, -1, yylineno);

                                    //---MERIKH APOTIMHSH---
                                    /*
                                    emit(if_greater, $1, $3, NULL, 0, yylineno);
                                    $$->truelist = newlist(nextquad()-1);
                                    emit(jump, NULL, NULL, NULL, 0, yylineno);
                                    $$->falselist = newlist(nextquad()-1);

                                    printf("True list = %d\n", $$->truelist+1);
                                    printf("False list = %d\n", $$->falselist+1);
                                    */
                                  }
                                }
          |expr GREATER_EQ expr	{
                                  printf("expr: expr GREATER_EQ expr at line %d --> %s\n", yylineno, yytext);
                                  if(illegalop($1, $3))
                                    addError("Error, illegal boolean operation", "", yylineno);
                                  else{
                                    $$ = newexpr(boolexpr_e);
                                    $$->sym = istempexpr($1) ? $1->sym : newtemp();

                                    emit(if_greatereq, $1, $3, NULL, nextquad()+3, yylineno);
                                    emit(assign, newexpr_constbool(0), NULL, $$, -1, yylineno);
                                    emit(jump, NULL, NULL, NULL, nextquad()+2, yylineno);
                                    emit(assign, newexpr_constbool(1), NULL, $$, -1, yylineno);
                                  }
                                }
          |expr LESS expr       {
                                  printf("expr: expr LESS expr at line %d --> %s\n", yylineno, yytext);
                                  if(illegalop($1, $3))
                                    addError("Error, illegal boolean operation", "", yylineno);
                                  else{
                                    $$ = newexpr(boolexpr_e);
                                    $$->sym = istempexpr($1) ? $1->sym : newtemp();

                                    emit(if_less, $1, $3, NULL, nextquad()+3, yylineno);
                                    emit(assign, newexpr_constbool(0), NULL, $$, -1, yylineno);
                                    emit(jump, NULL, NULL, NULL, nextquad()+2, yylineno);
                                    emit(assign, newexpr_constbool(1), NULL, $$, -1, yylineno);
                                  }
                                }
          |expr LESS_EQ expr		{
                                  printf("expr: expr LESS_EQ expr at line %d --> %s\n", yylineno, yytext);
                                  if(illegalop($1, $3))
                                    addError("Error, illegal boolean operation", "", yylineno);
                                  else{
                                    $$ = newexpr(boolexpr_e);
                                    $$->sym = istempexpr($1) ? $1->sym : newtemp();

                                    emit(if_lesseq, $1, $3, NULL, nextquad()+3, yylineno);
                                    emit(assign, newexpr_constbool(0), NULL, $$, -1, yylineno);
                                    emit(jump, NULL, NULL, NULL, nextquad()+2, yylineno);
                                    emit(assign, newexpr_constbool(1), NULL, $$, -1, yylineno);
                                  }
                                }
          |expr EQUAL expr      {
                                  printf("expr: expr EQUAL expr at line %d --> %s\n", yylineno, yytext);
                                  if(illegalop($1, $3))
                                    addError("Error, illegal boolean operation", "", yylineno);
                                  else{
                                    $$ = newexpr(boolexpr_e);
                                    $$->sym = istempexpr($1) ? $1->sym : newtemp();

                                    emit(if_eq, $1, $3, NULL, nextquad()+3, yylineno);
                                    emit(assign, newexpr_constbool(0), NULL, $$, -1, yylineno);
                                    emit(jump, NULL, NULL, NULL, nextquad()+2, yylineno);
                                    emit(assign, newexpr_constbool(1), NULL, $$, -1, yylineno);
                                  }
                                }
          |expr NOT_EQUAL expr  {
                                  printf("expr: expr NOT_EQUAL expr at line %d --> %s\n", yylineno, yytext);
                                  if(illegalop($1, $3))
                                    addError("Error, illegal boolean operation", "", yylineno);
                                  else{
                                    $$ = newexpr(boolexpr_e);
                                    $$->sym = istempexpr($1) ? $1->sym : newtemp();

                                    emit(if_noteq, $1, $3, NULL, nextquad()+3, yylineno);
                                    emit(assign, newexpr_constbool(0), NULL, $$, -1, yylineno);
                                    emit(jump, NULL, NULL, NULL, nextquad()+2, yylineno);
                                    emit(assign, newexpr_constbool(1), NULL, $$, -1, yylineno);
                                  }
                                }
          |expr AND expr        {
                                  printf("expr: expr AND expr at line %d --> %s\n", yylineno, yytext);

                                    $$ = newexpr(boolexpr_e);
                                    $$->sym = istempexpr($1) ? $1->sym : newtemp();
                                    emit(and, $1, $3, $$, -1, yylineno);

                                    //---MERIKH APOTIMHSH---
                                    /*
                                    if($1->type != boolexpr_e){
                                      emit(if_eq, newexpr_constbool(1), $1, NULL, 0, yylineno);
                                      //$$->truelist = newlist(nextquad()-1);
                                      emit(jump, NULL, NULL, NULL, 0, yylineno);
                                      //$$->falselist = newlist(nextquad()-1);
                                    }
                                    if($4->type != boolexpr_e){
                                      emit(if_eq, newexpr_constbool(1), $4, NULL, 0, yylineno);
                                      emit(jump, NULL, NULL, NULL, 0, yylineno);
                                    }

                                    //---MERIKH APOTIMHSH---
                                    printf("M = %d\n", $3+1);
                                    patchlabel($1->truelist, $3);
                                    $$->truelist = $4->truelist;
                                    printf("True list = %d\n", $$->truelist+1);
                                    printf("Merging falselist: $1->falselist = %d | $4->falselist = %d\n", $1->falselist+1, $4->falselist+1);
                                    $$->falselist = mergelist($1->falselist, $4->falselist);
                                    printf("False list = %d\n", $$->falselist+1);
                                    */
                                }
          |expr OR expr			    {
                                  printf("expr: expr OR expr at line %d --> %s\n", yylineno, yytext);
                                    $$ = newexpr(boolexpr_e);
                                    $$->sym = istempexpr($1) ? $1->sym : newtemp();
                                    emit(or, $1, $3, $$, -1, yylineno);

                                    //---MERIKH APOTIMHSH---
                                    /*
                                    printf("M = %d\n", $3+1);
                                    patchlabel($1->falselist, $3);
                                    printf("Merging truelist: $1->truelist = %d | $4->truelist = %d\n", $1->truelist+1, $4->truelist+1);
                                    $$->truelist = mergelist($1->truelist, $4->truelist);
                                    printf("Truelist = %d\n", $$->truelist+1);
                                    $$->falselist = $4->falselist;
                                    printf("Falselist = %d\n", $$->falselist+1);
                                    */
                                }
          |term					        {	printf("expr: term at line %d --> %s\n", yylineno, yytext);}
          ;

term:     L_PAR expr R_PAR			    {
                                      printf("term: L_PAR expr R_PAR at line %d --> %s\n", yylineno, yytext);
                                      $$ = $2;
                                    }
          |MINUS expr %prec UMINUS	{
                                      printf("term: MINUS expr at line %d --> %s\n", yylineno, yytext);
                                      check_arith($2, "-expr");
                                      $$ = newexpr(arithexpr_e);
                                      $$->sym = istempexpr($2) ? $2->sym : newtemp();
                                      emit(uminus, $2, NULL, $$, -1, yylineno);
                                    }
          |NOT expr	                {
                                      printf("term: NOT expr at line %d --> %s\n", yylineno, yytext);
                                      $$ = newexpr(boolexpr_e);
                                      $$->sym = istempexpr($2) ? $2->sym : newtemp();
                                      emit(not, $2, NULL, $$, -1, yylineno);

                                      //---MERIKH APOTIMHSH---
                                      /*  
                                      $$->truelist = $2->falselist;
                                      $$->falselist = $2->truelist;
                                      */
                                    }
          |INCR lvalue              {
                                      printf("term: INCR lvalue at line %d --> %s\n", yylineno, yytext);
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
                                      printf("term: lvalue INCR at line %d --> %s\n", yylineno, yytext);
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
                                      printf("term: DECR lvalue at line %d --> %s\n", yylineno, yytext);
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
                                      printf("term: lvalue DECR at line %d --> %s\n", yylineno, yytext);
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
                                      printf("term: primary at line %d --> %s\n", yylineno, yytext);
                                      $$ = $1;
                                    }
			    ;

assignexpr: lvalue ASSIGN expr  {
                                  printf("assignexpr: lvalue ASSIGN expr at line %d --> %s\n", yylineno, yytext);
                                  printf("Entered assignexpr: lvalue = expr\n");
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

                                  //---MERIKH APOTIMHSH
                                  /*
                                   if($3->type == boolexpr_e){
                                      printf("%s is boolexpr\n", $3->sym->name);
                                    //EDW PREPEI NA GINEI TO TELIKO BACKPATCHING THS MERIKHS APOTIMHSHS
                                    //KAI NA PROSTETHOUN TA 3 EXTRA QUADS.
                                      patchlist($3->truelist, nextquad());
                                      patchlist($3->falselist, nextquad()+2);
                                    
                                      emit(assign, newexpr_constbool(1), NULL, $$, -1, yylineno);
                                      emit(jump, NULL, NULL, NULL, nextquad()+2, yylineno);
                                      emit(assign, newexpr_constbool(0), NULL, $$, -1, yylineno);
                                      emit(assign, $3, NULL, $1, -1, yylineno);
                                      emit(assign, $1, NULL, $$, -1, yylineno);
                                    }
                                    else{
                                    */
                                }
            ;

primary:    lvalue                { printf("primary: lvalue at line %d --> %s\n", yylineno, yytext);
										                $$ = emit_iftableitem($1, yylineno);
                                  }
            |call                 { printf("primary: call at line %d --> %s\n", yylineno, yytext);}
			      |tablemake            { printf("primary: tablemake at line %d --> %s\n", yylineno, yytext);}
			      |L_PAR funcdef R_PAR	{
                                    printf("primary: L_PAR funcdef R_PAR at line %d --> %s\n", yylineno, yytext);
                                    $$ = newexpr(programfunc_e);
                                    $$->sym = $2;
                                  }
			      |const					      {
                                    printf("primary: const at line %d --> %s\n", yylineno, yytext);
                                    $$ = $1;
                                  }
			      ;

lvalue:     ID          {
                          printf("lvalue: ID at line %d --> %s\n", yylineno, yylval.stringValue);
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
                            printf("Symbol %s does not exist\n", yylval.stringValue);
                          else
                            $$ = lvalue_expr(sym);\
                        }
			|tableitem			  {
                          printf("lvalue: tableitem at line %d --> %s\n", yylineno, yytext);
                          $$ = $1;
                        }
			;

tableitem:  lvalue DOT ID 				    {
                                        printf("tableitem: lvalue.ID at line %d --> %s\n", yylineno, yytext);
                                        printf("Entered tableitem: lvalue.ID\n");
                                        $$ = member_item($1, $3, yylineno);
                                      }
			      |lvalue L_BR expr R_BR 		{
                                        printf("tableitem: lvalue[expr] at line %d --> %s\n", yylineno, yytext);
                                        printf("Entered tableitem: lvalue[expr]\n");
                                        $1 = emit_iftableitem($1, yylineno);
                                        $$ = newexpr(tableitem_e);
                                        $$->sym = $1->sym;
                                        $$->index = $3;
                                      }
			      |call DOT ID 						  { printf("tableitem: call.ID at line %d --> %s\n", yylineno, yytext);}
			      |call L_BR expr R_BR 			{	printf("tableitem: call[expr] at line %d --> %s\n", yylineno, yytext);}
			      ;

call:	      call L_PAR elist R_PAR	  {
                                        printf("call: (elist) at line %d --> %s\n", yylineno, yytext);
                                        printf("Entered call (elist)\n");
                                        $$ = make_call($1, $3, yylineno);
                                      }
            |lvalue callsuffix				{
                                        printf("call: lvalue callsuffix at line %d --> %s\n", yylineno, yytext);
                                        printf("Enterd call: lvalue callsufix\n");
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
                                                        printf("call: (funcdef) (elist) at line %d --> %s\n", yylineno, yytext);
                                                        printf("(funcdef)(elist)\n");
                                                        expr* func = newexpr(programfunc_e);
                                                        func->sym = $2;
                                                        $$ = make_call(func, $5, yylineno);
                                                      }
            ;

callsuffix:	normcall          {
                                printf("callsuffix: normcall at line %d --> %s\n", yylineno, yytext);
                                $$ = $1;
                                printf("Entered normcall\n");
                              }
			      |methodcall       {
                                printf("callsuffix: methodcall at line %d --> %s\n", yylineno, yytext);
                                printf("Entered callsufix: methodcall\n");
                                $$ = $1;
                              }
			      ;

normcall:	L_PAR elist R_PAR   {
                                printf("normcall: (elist) at line %d --> %s\n", yylineno, yytext);
                                $$->elist = $2;
                                $$->method = 0;
                                $$->name = NULL;
                              }
			    ;

methodcall: DDOT ID L_PAR elist R_PAR {
                                        printf("methodcall: ..ID (elist) at line %d --> %s\n", yylineno, yytext);
                                        $$->elist = $4;
                                        $$->method = 1;
                                        $$->name = $2;
                                      }
				    ;

elist:      expr                { 
                                  printf("elist: expr at line %d --> %s\n", yylineno, yytext);
                                  printf("expr = %s\n", $1->sym->name);
                                  $$ = $1;
                                }
		        |expr COMMA elist 	{
								                  printf("elist: expr, elist at line %d --> %s\n", yylineno, yytext);
                                  printf("expr = %s | elist = %s\n", $1->sym->name, $3->sym->name);
                                  expr* tmp = $3;
                                  while(tmp->next != NULL){
                                    printf("tmp = %s | next = %s\n", tmp->sym->name, tmp->next->sym->name);
                                    tmp = tmp->next;
                                  }
                                  tmp->next = $1;
                                  $$ = $3;
                                  
                                }
 		        |					          {
								                  printf("elist: empty at line %d --> %s\n", yylineno, yytext);
                                  $$ = NULL;
                                }
 		        ;

tablemake:	L_BR elist R_BR     {
                                  printf("tablemake: [elist] at line %d --> %s\n", yylineno, yytext);
                                  printf("Entered tablemake: [elist]\n");
                                  int i = 0;
                                  expr* tmp = $2;
                                  expr* t = newexpr(newtable_e);
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
					 							          printf("tablemake: [indexed] at line %d --> %s\n", yylineno, yytext);
                                  printf("Entered tablemake[indexed]\n");
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
                                            printf("indexed: indexedelem at line %d --> %s\n", yylineno, yytext);
                                            printf("Entered indexed: indexelem\n");
                                            $$ = $1;
                                            //printf("indexedelem($1) = %s\n", $1->strConst);
                                            //printf("index = %d\n",(int)$1->index->numConst);
                                          }
			        |indexedelem COMMA indexed  {
                                            printf("indexed: indexedelem, indexed at line %d --> %s\n", yylineno, yytext);
                                            printf("Entered indexed: indexelem, indexed\n");
                                            //printf("indexedelem($1) = %s\n", $1->strConst);
                                            $1->next = $3;
                                          }
			        ;

indexedelem:	LCURLY_BR expr COLON expr RCURLY_BR {
                                                    printf("indexedelem: {expr:expr} at line %d --> %s\n", yylineno, yytext);
                                                    printf("Entered indexedelem: {expr:expr}\n");
                                                    //printf("{%s:%d}\n", $2->strConst, (int)$4->numConst);
                                                    $2->index = $4;
                                                    $$ = $2;
                                                  }
				      ;

block:  blockstart blockend
        |blockstart	stmtlist blockend	{	
                                        printf("block: LCURLY_BR  stmtlist at line %d --> %s\n", yylineno, yytext);
                                        if(loopcounter != 0){
                                          breakpointer = $2;
                                          printf("breakpointer->breaklist = %d | breakpointer->contlist = %d\n", breakpointer->breaklist, breakpointer->contlist);
                                        }
                                      }
			  ;

blockstart: LCURLY_BR {
                        printf("block: LCURLY_BR at line %d --> %s\n", yylineno, yytext);
                        printf("Entered block\n");
                        currentscope++;
                      }

blockend: RCURLY_BR   {
                        printf("block: LCURLY_BR RCURLY_BR at line %d --> %s\n", yylineno, yytext);
                        hideScope(currentscope);
                        currentscope--;
                      }

funcdef:  funcprefix funcargs funcblockstart funcbody	funcblockend {
                                              printf("funcdef: funcprefix funcargs funcbody at line %d --> %s\n", yylineno, yytext);
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
                                              printf("funcprefix: FUNCTION funcname at line %d --> %s\n", yylineno, yytext);
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
                                              printf("funcname: ID at line %d --> %s\n", yylineno, yytext);
                                              $$ = $1;
                                            }
				      |	                            {
                                              printf("funcname: empty at line %d --> %s\n", yylineno, yytext);
                                              $$ = newtempfuncname();
                                            }
				      ;

funcargs:		  L_PAR	                        {	currentscope++; inFunc++;}
				      idlist
				      R_PAR	                        {
                                              printf("funcargs: (idlist) at line %d --> %s\n", yylineno, yytext);
                                              currentscope--;
                                              enterscopespace();
                                              resetfunctionlocalsoffset();
                                            }
				      ;

funcbody: block {
                  printf("funcbody: block at line %d --> %s\n", yylineno, yytext);
                  inFunc--;
                  $$ = currscopeoffset();
                  exitscopespace();
                }

funcblockstart: {
                  printf("Entered funcblockstart\n");
                  printf("loopcounter before push = %d\n", loopcounter);
                  pushCounter(&loopcounterStack, loopcounter); 
                  loopcounter = 0;
                  printf("loopcounter after push = %d\n", loopcounter);
                }

funcblockend:   { 
                  printf("Entered funcblockend\n");
                  printf("loopcounter before pop = %d\n", loopcounter);
                  loopcounter = popCounter(&loopcounterStack);
                  printf("loopounter after pop = %d\n", loopcounter);
                }

const: REAL 		{
                  printf("const: REAL at line %d --> %s\n", yylineno, yytext);
                  printf("const: REAL\n");
                  printf("REAL($1) = %f\n", $1);
                  $$ = newexpr_constnum($1);
                }
			 |INTEGER	{
                  printf("const: INTEGER at line %d --> %s\n", yylineno, yytext);
                  $$ = newexpr_constnum($1);
                }
       |STRING 	{
                  printf("const: FLEX_STRING at line %d --> %s\n", yylineno, yytext);
                  $$ = newexpr_conststring($1);
                }
       |NIL		  {	
                  printf("const: NIL at line %d --> %s\n", yylineno, yytext);
                  $$ = newexpr_conststring("nil");
                }
       |TRUE		{	printf("const: TRUE at line %d --> %s\n", yylineno, yytext);
                  $$ = newexpr_conststring("true");
                }
       |FALSE 	{	printf("const: FALSE at line %d --> %s\n", yylineno, yytext);
                  $$ = newexpr_conststring("false");
                }
       ;

idlist:		ID	{
                printf("idlist: ID at line %d --> %s\n", yylineno, yytext);
                symbol *sym = lookup(yylval.stringValue, currentscope);
                if(sym == NULL){
                  sym = hashInsert(yylval.stringValue, currentscope, yylineno, var_s, currscopespace(), currscopeoffset());
                  inccurrscopeoffset();
                }
              }
          |idlist COMMA ID 	{
                              printf("idlist: idlist COMMA ID at line %d --> %s\n", yylineno, yytext);
                              symbol *sym = lookup(yylval.stringValue, currentscope);
                              if(sym == NULL){
                                sym = hashInsert(yylval.stringValue, currentscope, yylineno, var_s, currscopespace(), currscopeoffset());
                                inccurrscopeoffset();
                              }
                            }
			    |
          ;

ifprefix: IF L_PAR expr R_PAR           {
                                          printf("ifprefix: if(expr) at line %d --> %s\n", yylineno, yytext);
                                          emit(if_eq, $3, newexpr_constbool(1), NULL, nextquad()+2, yylineno);
                                          $$ = nextquad();
                                          emit(jump, NULL, NULL, NULL, 0, yylineno);
                                        }

elseprefix: ELSE                        {
                                          printf("elseprefix: else at line %d --> %s\n", yylineno, yytext);
                                          $$ = nextquad();
                                          emit(jump, NULL, NULL, NULL, 0, yylineno);
                                        }

ifstmt: ifprefix stmt                   {
                                          printf("ifstmt: ifprefix stmt at line %d --> %s\n", yylineno, yytext);
                                          printf("Entered ifstmt: ifprefix stmt\n");
                                          patchlabel($1, nextquad());
                                          if(breakcount != 0 || contcount != 0){
                                            //printf("$2->breaklist = %d\n", $2->breaklist);
                                            $$ = $2;
                                          }
                                        }
        |ifprefix stmt elseprefix stmt  {
                                          printf("ifstmt: ifprefix stmt elseprefix stmt at line %d --> %s\n", yylineno, yytext);
                                          printf("Entered ifstmt: ifprefix stmt elseprefix stmt\n");
                                          patchlabel($1, $3+1);
                                          patchlabel($3, nextquad());

                                          stmt_t* tmp = (stmt_t*) malloc(sizeof(stmt_t));
                                          if(breakcount > 1){
                                            printf("$2->breaklist = %d | $4->breaklist = %d\n", $2->breaklist, $4->breaklist);
                                            tmp->breaklist = mergelist($2->breaklist, $4->breaklist);
                                            breakcount = 0;
                                          }
                                          if(contcount > 1){
                                            printf("$2->contlist = %d | $4->contlist = %d\n", $2->contlist, $4->contlist);
                                            tmp->contlist = mergelist($2->contlist, $4->contlist);
                                            contcount = 0;
                                          }
                                          $$ = tmp;
                                        }

whilestart: WHILE                       {
                                          printf("whilestart: WHILE at line %d --> %s\n", yylineno, yytext);
                                          printf("Entered whilestart\n");
                                          $$ = nextquad();
                                        }

whilecond: L_PAR expr R_PAR           {
                                          printf("whilesecond: (expr) at line %d --> %s\n", yylineno, yytext);
                                          printf("Entered whilecond\n");
                                          emit(if_eq, $2, newexpr_constbool(1), NULL, nextquad()+2, yylineno);
                                          $$ = nextquad();
                                          emit(jump, NULL, NULL, NULL, 0, yylineno);
                                        }

whilestmt: whilestart whilecond loopstmt {
                                          printf("whilestmt: while(expr) stmt at line %d --> %s\n", yylineno, yytext);
                                          printf("Entered whilestmt, line = %d\n", yylineno);
                                          emit(jump, NULL, NULL, NULL, $1, yylineno);
                                          patchlabel($2, nextquad());

                                          if(breakcount != 0){
                                            printf("$3->breaklist = %d\n", $3->breaklist);
                                            patchlist($3->breaklist, nextquad());
                                            breakcount = 0;
                                          }
                                          if(contcount != 0){
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
                                                          printf("forprefix: L_PAR elist SEMICOLON M expr SEMICOLON at line %d --> %s\n", yylineno, yytext);
                                                          printf("Enterd forprefix\n");
                                                          forprefix_t* tmp = (forprefix_t*) malloc(sizeof(forprefix_t));
                                                          tmp->test = $5;
                                                          tmp->enter = nextquad();
                                                          emit(if_eq, $6, newexpr_constbool(1), NULL, 0, yylineno);
                                                          $$ = tmp;
                                                        }

forstmt:  forprefix N elist R_PAR N loopstmt N          {
                                                          printf("forstm: for(elist; expr; elist) stmt at line %d --> %s\n", yylineno, yytext);
                                                          printf("Enterd forstm\n");
                                                          patchlabel($1->enter, $5+1); //true jump
                                                          patchlabel($2, nextquad()); //false jump
                                                          patchlabel($5, $1->test); //loop jump
                                                          patchlabel($7, $2+1); //closure jump

                                                          if(breakcount != 0){
                                                            printf("$6->breaklist = %d\n", $6->breaklist);  
                                                            patchlist($6->breaklist, nextquad());
                                                          }
                                                          if(contcount != 0){
                                                            printf("$6->contlist = %d\n", $6->contlist);
                                                            patchlist($6->contlist, $2+1);
                                                          }
                                                          $$ = $6;
                                                        }

returnstmt:	RETURN SEMICOLON    {
                                  printf("returnstmt: RETURN SEMICOLON at line %d --> %s\n", yylineno, yytext);
                                  if (inFunc == 0)
                                    addError("Use of return while not in function", "", yylineno);
                                  else
                                    emit(ret, NULL, NULL, NULL, -1, yylineno);
                                }
			      |RETURN expr SEMICOLON   {
                                        printf("returnstmt: RETURN expr SEMICOLON at line %d --> %s\n", yylineno, yytext);
                                        if (inFunc == 0)
                                          addError("Use of return while not in function", "", yylineno);
                                        else
                                          emit(ret, NULL, NULL, $2, -1, yylineno);
                                      }
			      ;

loopstart: { printf("loopstart: at line %d --> %s\n", yylineno, yytext); ++loopcounter;}

loopend:   { printf("loopend: at line %d --> %s\n", yylineno, yytext); --loopcounter;}

loopstmt: loopstart stmt loopend  { 
                                    printf("loopstmt: loopstart stmt loopend at line %d --> %s\n", yylineno, yytext);
                                    printf("Entered loopstmt: loopstart stmt loopend\n"); 
                                    
                                    if(breakcount != 0 || contcount != 0){
                                      if(breakpointer != NULL){
                                        printf("Some magic happens\n");
                                        $$ = breakpointer;
                                      }
                                      else{
                                        printf("Normal case\n");
                                        $$ = $2;
                                      }
                                    }
                                  }

break: BREAK        {
                      printf("break: BREAK at line %d --> %s\n", yylineno, yytext);
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
                      printf("continue: CONTINUE at line %d --> %s\n", yylineno, yytext);
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

    fclose(fp);
    return 0;
}