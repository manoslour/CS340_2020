#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned magicNumber;

unsigned int totalStringconsts; 
unsigned int totalGlobals; 
unsigned int totalVars;
unsigned int totalUserFuncs;
unsigned int totalLibFuncs;
int instNo = 0;
int currInst = 0;


int pos = 0;

typedef struct userfunc
{
	int address;
	int localSize;
	char* id;	
}userfunc;


typedef struct vmarg
{
	int val; 
	int type; 
}vmarg;

typedef struct instruction
{
	int op;
	vmarg *result; 
	vmarg *arg1; 
	vmarg *arg2; 
	int srcLine;
}instruction;

instruction* code = (instruction*) 0 ;


int translate_instr_op(char* name){

	if(!strcmp(name, "assign"))				return 0;
	else if(!strcmp(name, "add"))			return 1;
	else if(!strcmp(name, "sub"))			return 2;
	else if(!strcmp(name, "mul"))			return 3;
	else if(!strcmp(name, "div"))			return 4; 
	else if(!strcmp(name, "mod"))			return 5;
	else if(!strcmp(name, "uminus"))		return 6;
	else if(!strcmp(name, "and"))			return 7;
	else if(!strcmp(name, "or"))			return 8;
	else if(!strcmp(name, "not"))			return 9;
	else if(!strcmp(name, "jeq"))			return 10;
	else if(!strcmp(name, "jne"))			return 11; 
	else if(!strcmp(name, "jle"))			return 12;
	else if(!strcmp(name, "jge")) 			return 13;
	else if(!strcmp(name, "jlt"))			return 14;
	else if(!strcmp(name, "jgt"))			return 15;
	else if(!strcmp(name, "call"))			return 16;
	else if(!strcmp(name, "pusharg"))		return 17;
	else if(!strcmp(name, "funcenter"))		return 18;
	else if(!strcmp(name, "funcexit"))		return 19; 
	else if(!strcmp(name, "newtable"))		return 20;
	else if(!strcmp(name, "tablegetelem"))	return 21;
	else if(!strcmp(name, "tablesetelem"))	return 22;
	else if(!strcmp(name, "jump"))			return 23; 
	else if(!strcmp(name, "nop"))			return 24;
}


void emit_avm_instruction(char* op, int rtype, int rval, int arg1type, int arg1val, int arg2type, int arg2val, int line){
 

	if (code == NULL){
		code = (instruction*)malloc(sizeof(instruction)); 
	}
	else {
		instruction* new_instr = (instruction*)realloc(code, (instNo + 1)*sizeof(instruction));
		code = new_instr;	
	}
	instNo++;
	instruction* i = (instruction*)malloc(sizeof(instruction));

	vmarg *result , *arg1, *arg2;
	result = malloc(sizeof(struct vmarg)); 
	arg1 = malloc(sizeof(struct vmarg)); 
	arg2 = malloc(sizeof(struct vmarg)); 

	i = code + currInst;
	
	i->op = translate_instr_op(op);
	result->type = rtype;
	result->val = rval;

	arg1->type = arg1type; 
	arg1->val = arg1val;

	arg2->type = arg2type;
	arg2->val = arg2val;
	currInst++;

	i->result = result;
	i->arg1 = arg1; 
	i->arg2 = arg2; 
	i->srcLine = line ;
}

void makeInstruction(char* s, int instrNo, int line ){

	int ch;
    int rval , rtype , val1 , type1 , val2 , type2 = 0 ;	
	int size = strlen(s);
	char *No, *op ;
	int instructionNum; 

	
    No = malloc(sizeof(int));
	 
    int i; 
    No = strtok(s, " ");
    sscanf(No, "%d", &instructionNum); 

    while ( No != NULL){
        No = strtok(NULL, "<");
        op = strdup(No);
        
        No = strtok(NULL, ">"); 
        if (!strcmp(No, " ")) {
           rtype = -1;
            rval = -1;
        }
        else {
            char *tmp = malloc(sizeof(int));
            char *tmp2 = malloc(sizeof(int));
            for(i = 0; i<strlen(No); i++){
                *(tmp + i) = *(No +i);
                if (*(No +i) == ' ') {

                    *(tmp + i) = '\0';
                    sscanf(tmp, "%d", &rtype);
                    *tmp = '\0';
                    int j;
                    for (j=i; j<strlen(No); j++){
                        *(tmp2 + j - i) = *(No + j);
                    }
                    *(tmp2 + j - i) = '\0';
                    sscanf(tmp2, "%d", &rval);
                    free(tmp); 
                    free(tmp2);
                    break;
                }
            }
        }
        
        No = strtok(NULL, "}");
        if (!strcmp(No, "{ ")) {
            val1= -1;
            type1 = -1;
        }
        else {
            char* tmp3 = malloc(sizeof(int));
            char* tmp4 = malloc(sizeof(int));
            for(i = 0; i<strlen(No); i++){
                *(tmp3 + i) = *(No +i+1);
                if (*(No +i) == ' ') {

                    *(tmp3 + i) = '\0';
                    sscanf(tmp3, "%d", &type1);
                    *tmp3 = '\0';
                    int j;
                    for (j=i; j<strlen(No); j++){
                        *(tmp4 + j - i) = *(No + j);
                    }
                    *(tmp4 + j - i) = '\0';
                    sscanf(tmp4, "%d", &val1);
                    free(tmp3); 
                    free(tmp4);
                    break;
                }
            }
        }

        No = strtok(NULL, "]");
        if (!strcmp(No, "[ ")){
            type2 = -1;
            val2 = -1;
        }
        else {
            char* tmp5 = malloc(sizeof(int));
            char* tmp6 = malloc(sizeof(int));
            for(i = 0; i<strlen(No); i++){
                *(tmp5 + i) = *(No +i+1);
                if (*(No +i) == ' ') {
                    *(tmp5 + i) = '\0';
                    sscanf(tmp5, "%d", &type2);
                    *tmp5 = '\0';
                    int j;
                    for (j=i; j<strlen(No); j++){
                        *(tmp6 + j - i) = *(No + j);
                    }
                    *(tmp6 + j - i) = '\0';
                    sscanf(tmp6, "%d", &val2);
                    free(tmp5); 
                    free(tmp6);
                    break;
                }
            }
        }
        emit_avm_instruction(op, rtype, rval, type1, val1, type2, val2, line);
        break;
    }
};

int get_instr_line(char* s){

 	
 	char *line = malloc(sizeof(int)), *tmp = strdup(s);
 	int j = 0;
 	int i = strlen(tmp); 
 	int k;
 	int line_no;
 	
 	for(i = 0; i<=strlen(tmp); i++){
 		if (*(tmp + i) == ']') {
 			i++;
 			for( k = i; k<=strlen(tmp); k++){
 				*(line + j++) = *(tmp + k); 
 			}
 			*(line + j++) = '\0';	
 			sscanf(line , "%d", &line_no);

 			return line_no;
 		}
 	}



 }


double* numConsts = (double*)0; 
unsigned currNumConst = 0;

char** stringConsts = (char**)0;
unsigned curStringConsts = 0;

char** namedLibFuncs = (char**)0; 
unsigned currLibFuncs = 0;

userfunc* userFuncs = (userfunc*)0;
unsigned currUserFunc = 0;  

void printL(){
	FILE *file;
	file = fopen("constArrays.txt", "w+");
	fprintf(file, "Total global vars : %d\n", totalGlobals );
	fprintf(file, "Const String Array :\n");
	for(int i=0; i<curStringConsts; i++){
		fprintf(file, "%d: %s \n",i, stringConsts[i] );			
	}

	fprintf(file, "Const Nums Array :\n");

	for(int i=0; i<currNumConst; i++){
		fprintf(file, "%d: %lf \n",i, numConsts[i] );			
	}
	
	fprintf(file, "User Functions Array :\n");

	for(int i=0; i<currUserFunc; i++){
		fprintf(file, "%d: %d, %d, %s\n",i,
			userFuncs[i].address,
			userFuncs[i].localSize,
			userFuncs[i].id );			
	}

	fprintf(file, "Named Libfuncs Array :\n");

	for(int i=0; i<currLibFuncs; i++){
		fprintf(file, "%d: %s \n",i, namedLibFuncs[i] );			
	}

	fprintf(file, "\n\nInstructions :\n" );

	for(int i=0; i<currInst; i++){
		fprintf(file, "%d ",i );
		fprintf(file, " %d ", code[i].op);

		if(code[i].result->type == -1 || code[i].result->val == -1)
			fprintf(file, "%s", " ");
		else 
			fprintf(file, " %d,%d ",code[i].result->type, code[i].result->val );

		if(code[i].arg1->type == -1 || code[i].arg1->val == -1)
			fprintf(file, "%s", " ");
		else 
			fprintf(file, " %d,%d ",code[i].arg1->type, code[i].arg1->val );

		if(code[i].arg2->type == -1 || code[i].arg2->val == -1)
			fprintf(file, "%s", " ");
		else 
			fprintf(file, " %d,%d ",code[i].arg2->type, code[i].arg2->val );
		
		fprintf(file, "%d\n",code[i].srcLine);
	}
}


//function to add libFuncs to namedLibfuncs global array
void add_lib_func(char* name){

	if (namedLibFuncs == NULL){
		namedLibFuncs = (char**)malloc((sizeof(char) + 8) * 12);
	}
	namedLibFuncs[currLibFuncs++] = strdup(name);
}


//function to add userfuncs to userfuncs global array
void add_user_func(int address, int localsize, char* name){


	if(userFuncs == NULL){
		userFuncs = malloc(sizeof(userfunc) * totalUserFuncs + 1000 );
	}
	userFuncs[currUserFunc].address = address;
	userFuncs[currUserFunc].localSize = localsize; 
	userFuncs[currUserFunc].id = strdup(name);
	currUserFunc++;
}


// tokenize the string to take the info about userfunc
void make_user_func(char* s){
	int address, localsize;

	if (*(s) == '\0')
		return;

	int i=0;
	int pos =0;
	char* token = strtok(s, " ");
	while (token != NULL){
		
		if(pos == 0 ) pos++; 
		else if (pos == 1) {pos++;sscanf(token, "%d", &address);}
		else if (pos == 2) {pos++;sscanf(token, "%d", &localsize);}
		else if (pos == 3) {break;}

		token = strtok(NULL, " ");
	}
	add_user_func(address, localsize, token);
}

//add string to stringConsts global array 
void add_string_const(char* s){

	if (stringConsts == NULL){
		stringConsts = malloc(sizeof(char*) * totalStringconsts);
	}
	stringConsts[curStringConsts++] = strdup(s); 
}

//add num to numConsts global array 
void add_num_consts(char* s){

	if (numConsts == NULL){
		numConsts = malloc(sizeof(double) * totalVars);
	}

	double num ; 
	sscanf(s, "%lf", &num); 

	numConsts[currNumConst++] = num;

}

int main (){



	FILE *fp ; 
	fp = fopen("test", "r"); 
	char *s = malloc(sizeof(int)); 
	int  i = 0;
	int ch;
	int n=0;
    char *line_buf;
    int line_buf_size = 0;
	while ((ch = fgetc(fp)) != EOF){   
        *(s+i++) = (char) ch;
        if (ch == '\n' && pos == 0) {		//read the magic number and check if its the same
        	pos ++;
        	sscanf(s, "%d", &magicNumber);
	        if(magicNumber != 42069){
	        	printf("Wrong magicNumber execution failed \n"); 
				return 0;
	        }
	        ch = fgetc(fp);
	        char* globalChar = malloc(sizeof(int));
	        i=0;
	        while (ch != '\n'){
	        	*(globalChar + i++) = (char) ch;
	        	ch = fgetc(fp);
 	        }  
 	        *(globalChar + i) = '\0';
 	        sscanf(globalChar, "%d", &totalGlobals);
        }

        if( ch == '#') {
        	// ENTER THE STRING CONST ARRAY and obtein info about the const strings
        	if (pos == 1){ 
        		pos++;
        		
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

        		while (1){
        			int instring = 0;
        			if (ch == '#') break; 
        			ch = fgetc(fp);
        			char* string = malloc(sizeof(char*));
        			if(ch == '\"') {
        				int i = 0;
        				if (instring == 0){
        					instring ++;
        					ch = fgetc(fp); 
        					while (1){
        						*(string + i++) = (char)ch;
        						ch = fgetc(fp); 
        						if (ch == '\"') break;
        					}
        					add_string_const(string);
        				}
        			}	
        		}
        	}
        	if (pos == 2   ){
        		// ENTER THE Num CONST ARRAY and obtein info about the const nums
        		
        		pos++;
        		while(1){
        			ch = fgetc(fp); 
        			if (ch == '\n') break;
        		}
        		i = 0;
        		char *totalVarChar = malloc(sizeof(int)); 
        		while (1){
        			ch = fgetc(fp); 
        			*(totalVarChar + i++) = (char) ch;
        			if (ch == '\n') break;
        		}  
        		sscanf(totalVarChar, "%d", &totalVars);
        		
        		while(1){
        			if (ch == '#') break;
        			ch = fgetc(fp); 
        			double num;
        			char* tmp = malloc(sizeof(double)); 
        			if (ch == ' ') {
        				int i = 0;
        				while (ch != '\n'){
        					ch = fgetc(fp);
        					if (ch == '\n') break;
        					*(tmp + i++) = (char)ch;
        				}
        				add_num_consts(tmp);
        			}
        		}

        	}
        	if (pos == 3){
        		pos++;
        		//ENTER THE userfunc ARRAY and obtein info about the usefuncs
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
        	
        		char* name = malloc(sizeof(char*));
        		int i;
        		while(1){
        			ch = fgetc(fp);

        			if (ch == '#') break;
        			i=0;
        			while (ch != '\n'  ) {
        				*(name +i++) = (char)ch;
        				ch = fgetc(fp);
        			}
        			*(name +i) = '\0';
        			make_user_func(name);
        			if (ch == '#') break;
        		}
        	}
        	if (pos == 4){
        		//add the lib funcs to the namedLibFunc array
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
	        	
	        	while(1){
	    			ch = fgetc(fp); 
	    			if (ch == 'I') break;
	        	}
	        	add_lib_func("print");
    			add_lib_func("input");
    			add_lib_func("objectmemberkeys");
    			add_lib_func("objecttotalmembers");
    			add_lib_func("objectcopy");
    			add_lib_func("tootalarguments");
    			add_lib_func("argument");
    			add_lib_func("typeof");
    			add_lib_func("strtonum");
    			add_lib_func("sqrt");
    			add_lib_func("cos");
    			add_lib_func("sin");
        	}
        	if (pos == 5){
        		//Finaly tokenize the instrucrions and add them to the global array code
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
                        int line = get_instr_line(instruction_n);
                        makeInstruction(instruction_n, instNo, line); 
                        i = 0;
                        *(instruction_n + i) = '\0' ;
                        instNo++;
                    }     
                }
                *(instruction_n + i) = '\0' ;
                int line = get_instr_line(instruction_n);
                makeInstruction(instruction_n, instNo, line); 
        	}
        }
    }
	fclose(fp);
  	printL();
	return 0;


}