#include "symTable.h"














ScopeListEntry *scope_head = NULL; //Global pointer to the scope list's head
symbol *HashTable[Buckets];
struct errorToken *ERROR_HEAD = NULL; // GLobal pointer to the start of error_tokkens list

quad* quads = (quad*) 0;
unsigned total = 0;
unsigned int currQuad = 0;

unsigned int tempcounter = 0;
extern unsigned int funcprefix;
extern unsigned int currentscope;

unsigned int programVarOffset = 0;
unsigned int functionLocalOffset = 0;
unsigned int formalArgOffset = 0;
unsigned int scopeSpaceCounter = 1;

void emit(iopcode op, expr* arg1, expr* arg2, expr* result, unsigned int label, unsigned int line){

	if(currQuad == total)
		expand();

	quad* p = quads + currQuad++;
	p->arg1 = arg1;
	p->arg2 = arg2;
	p->result = result;
	p->label = label;
	p->line = line;
}

void expand(){

	assert(total == currQuad);
	quad* p = (quad*) malloc(NEW_SIZE);
	if(quads){
		memcpy(p, quads, CURR_SIZE);
		free(quads);
	}
	quads = p;
	total += EXPAND_SIZE;
}


void initialize(){
  expr *arg1, *arg2, *result ;
  symbol *new_sym;

  new_sym = malloc(sizeof(symbol));
  arg1 = malloc(sizeof(expr));
  arg2 = malloc(sizeof(expr));
  result = malloc(sizeof(expr));

  new_sym->name = "KALISPERA";
  new_sym->isActive = 0;
  new_sym->scope =9;
  new_sym->line = 1;
  new_sym->iaddress =20;
  new_sym->type = 2;
  new_sym->space = 1;
  new_sym->offset = 23;
  new_sym->totalLocals = 25;

  arg1->sym = arg2->sym = result->sym = new_sym;
  arg1->index = arg2->index = result->index = NULL;
  arg1->numConst = arg2->numConst = result->numConst = 0;
  arg1->strConst = arg2->strConst = result->strConst = 0;
  arg1->boolConst = arg2->boolConst = result->boolConst = 0;


  emit(assign, arg1, arg2, result, 23, 3 );

}

void printQuads(){

	int i;
	printf("\nQuad# \t opcode \t result \t\t arg1 \t\t\t arg2 \t\t label");
	printf("\n--------------------------------------------------------------" );
	for (i=0; i<currQuad; i++){
		printf("\n %d: \t %d \t\t %s \t\t %s \t\t %s \t %d", i, (quads+i)->op
		, (quads+i)->result->sym->name, (quads+i)->arg1->sym->name, (quads+i)->arg2->sym->name, (quads+i)->label);
	}
  printf("\n");
}






int main(){

	char str[20];
	   scanf("%[^\n]%*c", str);
	   printf("%s", str);
	return 0;
}
