#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned magicNumber;

unsigned int totalStringconsts; 
unsigned int totalGlobals; 
unsigned int totalUserFuncs;
unsigned int totalLibFuncs;
int instNo = 0;


int pos = 0;


typedef struct vmarg
{
	int val; 
	int type; 
}vmarg;

typedef struct instruction
{
	char* op;
	vmarg *result; 
	vmarg *arg1; 
	vmarg *arg2; 
	int srcLine;
}instruction;

instruction* code = (instruction*) 0 ;



void printInst(){


    for (int i=0; i<instNo; i++){
        printf( "%d ", i);
        printf( "%s", code[i].op);

        if( code[i].result == NULL || code[i].result->type == -1 || code[i].result->val == -1)
            printf("<%s >", "");
        else 
            printf("<%d %d>", code[i].result->type, code[i].result->val);
        if(code[i].arg1 == NULL || (code[i].arg1->type == -1 || code[i].arg1->val == -1))
            printf( "{%s }", "");
        else 
            printf("{%d %d}", code[i].arg1->type, code[i].arg1->val);
        
        if(code[i].arg2 == NULL || (code[i].arg2->type == -1 || code[i].arg2->val == -1))
            printf("[%s ]", "");
        else 
            printf( "[%d %d]", code[i].arg2->type, code[i].arg2->val);

       printf("\n");
    }
}



void makeInstruction(char* s, int instrNo){


	int ch;
    
	int size = strlen(s);
	char *No, *op, *line;
	vmarg *result , *arg1, *arg2;
	result = arg1 = arg2 = malloc(sizeof(vmarg)); 
	struct instruction *new_instr;
    int instructionNum;

	new_instr = malloc(sizeof(instruction));

	new_instr->result = result; 
	new_instr->arg1 = arg1; 
	new_instr->arg2 = arg2;

    No = malloc(sizeof(int));
	 
    // printf("infunc with ch: %c\n",*s);
    int i; 
    No = strtok(s, " ");
    sscanf(No, "%d", &instructionNum); 

    while ( No != NULL){
        //printf(" %s", No);
        No = strtok(NULL, "<");
        op = strdup(No);
        new_instr->op = op;
        No = strtok(NULL, ">"); 
        // printf("TWRA P DEN EXW EXW : %s\n", No);
        if (!strcmp(No, " ")) {
            new_instr->result->type = -1;
            new_instr->result->val = -1;
        }
        else {
            char *tmp = malloc(sizeof(int));
            char *tmp2 = malloc(sizeof(int));
            // /printf("TO no IENAI :%s\n", No);
            for(i = 0; i<strlen(No); i++){
                *(tmp + i) = *(No +i);
                if (*(No +i) == ' ') {

                    *(tmp + i) = '\0';
                    sscanf(tmp, "%d", &new_instr->result->type);
                    *tmp = '\0';
                    for (int j=i; j<strlen(No); j++){
                        *(tmp2 + j - i) = *(No + j);
                    }
                    sscanf(tmp2, "%d", &new_instr->result->val);
                    break;
                }
            }
        }
                    printf("res type  : %d ", new_instr->result->type);
                    printf("res val : %d\n", new_instr->result->val);

        
        No = strtok(NULL, "}");
        if (!strcmp(No, "{ ")) {
            new_instr->arg1->type = -1;
            new_instr->arg1->val = -1;
        }
        else {
            char* tmp = malloc(sizeof(int));
            char* tmp2 = malloc(sizeof(int));
            // printf("TO no IENAI :%s\n", No);
            for(i = 0; i<strlen(No); i++){
                *(tmp + i) = *(No +i+1);
                if (*(No +i) == ' ') {

                    *(tmp + i) = '\0';
                    sscanf(tmp, "%d", &new_instr->result->type);
                    *tmp = '\0';
                    for (int j=i; j<strlen(No); j++){
                        *(tmp2 + j - i) = *(No + j);
                    }
                    sscanf(tmp2, "%d", &new_instr->result->val);
                    break;
                }
            }
        }
                    printf("arg1 type : %d ", new_instr->result->type);
                    printf("arg1 val : %d\n", new_instr->result->val);

        No = strtok(NULL, "]");
        if (!strcmp(No, "[ ")){
            new_instr->arg2->type = -1;
            new_instr->arg2->val = -1;
        }
        else {
            char* tmp = malloc(sizeof(int));
            char* tmp2 = malloc(sizeof(int));
            // printf("TO no IENAI :%s\n", No);
            for(i = 0; i<strlen(No); i++){
                *(tmp + i) = *(No +i+1);
                if (*(No +i) == ' ') {

                    *(tmp + i) = '\0';
                    sscanf(tmp, "%d", &new_instr->result->type);
                    *tmp = '\0';
                    for (int j=i; j<strlen(No); j++){
                        *(tmp2 + j - i) = *(No + j);
                    }
                    sscanf(tmp2, "%d", &new_instr->result->val);
                    break;
                }
            }
        }
                    printf("arg2 type : %d", new_instr->result->type);
                    printf(" arg2 val : %d\n", new_instr->result->val);
        
        i=0;
        printf("\n");



       

       /* if (code == NULL) {
            code = (instruction*)malloc(sizeof(instruction)); 
        }
        else {
            instruction* newinstr = (instruction*)realloc(code,(instNo +1) * sizeof(instruction));
            code = newinstr;
        }

        instruction* i = (instruction*) malloc(sizeof(instruction));
        i = code + instNo; 
       
        i->op = new_instr->op;
        i->result = new_instr->result;
        i->arg1 = new_instr->arg1;
        i->arg2 = new_instr->arg2;
*/
        // DEN EXW VALEI NA PERNEI LINE .... :(
        
        
        break;

    }
     // printf("InstrNo:%d opcode : %s  result : <%d %d>  arg1 : {%d %d} arg2 : [%d %d]}  ",
     //        instNo, new_instr->op, new_instr->result->type,  new_instr->result->val, 
     //         new_instr->arg1->type, new_instr->arg1->val, 
     //         new_instr->arg2->type, new_instr->arg1->val );


};


int main (){



	FILE *fp ; 
	fp = fopen("test", "r"); 
	char *s = malloc(1000000); 
	int  i = 0;
	int ch;
	int n=0;
    char *line_buf;
    int line_buf_size = 0;
	while ((ch = fgetc(fp)) != EOF)
    {
        printf("+%c", ch);
        
        *(s+i++) = (char) ch;
        if (ch == '\n' && pos == 0) {
        	pos ++;
        	sscanf(s, "%d", &magicNumber);
	        if(magicNumber != 42069){
	        	printf("Wrong magicNumber execution failed "); 
				return 0;
	        }
        }
        if( ch == '#') {
        	if (pos == 1){ // ENTER THE STRING CONST ARRAY 
        		pos++;
        		printf("Mpika pos = 1\n\n");
        		while (1){
        			ch = fgetc(fp); 
        			if (ch == '\n') break;
        		}
        		i=0;
        		char *totalStringconstsChar = malloc(sizeof(int));
				while (1){
        			ch = fgetc(fp); 
        			*(totalStringconstsChar + i++) = (char) ch;
        			if (ch == '\n') break;

        		}   
        		sscanf(totalStringconstsChar, "%d", &totalStringconsts);     		
        		printf("\nTotal string consts%d\n", totalStringconsts);

        		while (1){
        			ch = fgetc(fp);
        			if (ch = '#') break; 
        			//theloyme ta conststring array  ? 
        		}
        		
        	}
        	if (pos == 2   ){
        		//we have num consts
        		printf("mpika pos 2\n");
        		pos++;
        		while(1){
        			ch = fgetc(fp); 
        			if (ch == '\n') break;
        		}
        		i = 0;
        		char *totalGlobalsChar = malloc(sizeof(int)); 
        		while (1){
        			ch = fgetc(fp); 
        			*(totalGlobalsChar + i++) = (char) ch;
        			if (ch == '\n') break;
        		}  
        		sscanf(totalGlobalsChar, "%d", &totalGlobals);
        		printf("total globals einai %d\n\n", totalGlobals);
        		
        		//prepeui na apothikeysoume tis times  ????
        		while(1){
        			ch = fgetc(fp); 
        			if (ch == '#') break;
        		}

        	}
        	if (pos == 3){
        		pos++;
        		printf("MPIKA POS 3 me char : %c\n", ch);
        		while (1){
        			ch = fgetc(fp); 
        			if (ch == '\n') break;
        		}
        		i = 0;
        		char *totalUserFuncsChar = malloc(sizeof(int)); 
        		while(1){
        			ch = fgetc(fp);
        			*(totalUserFuncsChar + i++) = (char) ch;
        			if (ch == '\n') break;
        		}
        		sscanf(totalUserFuncsChar, "%d", &totalUserFuncs); 
        		printf("Total User funcs : %d\n",totalUserFuncs );

        		while(1){
        			ch = fgetc(fp); 
        			if (ch == '#') break;
        		}

        	}
        	if (pos == 4){
        		printf("mpika pos 4\n");
        		pos++;
	        	while (1){
	    			ch = fgetc(fp); 
	    			if (ch == '\n') break;
	    		}
	    		i = 0;
	    		char* totalLibFuncsChar = malloc(sizeof(int));
	    		while(1){
	    			ch = fgetc(fp); 
	    			*(totalLibFuncsChar + i++) = (char) ch;
	    			if (ch == '\n') break;
	    		}
	    		sscanf(totalLibFuncsChar, "%d", &totalLibFuncs);
	    		printf("Total Lib funcs : %d", totalLibFuncs);
	        	
	        	while(1){
	    			ch = fgetc(fp); 
	    			if (ch == '#') break;
	        	}
        	}
        	if (pos == 5){
        		printf("\nMpika pos 5\n");
        		pos++;

        		for (i=0; i<2; i++){
	        		while (1){
	        			ch = fgetc(fp);
	        			if (ch == '\n') break;
	        		}
        		}
        		i = 0;
        		char* instruction_n = malloc (500) ; 
        		while ((ch = fgetc(fp)) != EOF){
        			*(instruction_n + i++) = (char) ch; 
                   
            		if (ch == '\n') {
                        *(instruction_n + i) = '\0' ;
                        makeInstruction(instruction_n, instNo); 
                        i = 0;
                        *(instruction_n + i) = '\0' ;
                        instNo++;
                    }     

                }
                *(instruction_n + i) = '\0' ;
                
                makeInstruction(instruction_n, instNo); 


        	}
        }






    }
  	printf("\n");
	fclose(fp);
    //printInst();
	return 0;


}