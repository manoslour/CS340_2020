%{
    #include <stdio.h>
    int yyerror(char* yaccProvidedMessage);
    int yylex(void);

    extern int yylineno;
    extern char *yytext;
    extern FILE *yyin;
%}

%defines

%union{
    char* stringValue;
    int intValue;
    double realValue;
}

%token <stringValue> ID
%token <intValue> INTEGER
%token <realValue> REAL

%%



%%

int yyerror(char* yaccProvidedMessage){
    fprintf(stderr, "%s: at line %d, before token: %s\n", yaccProvidedMessage, yylineno, yytext);
    fprintf(stderr, "INPUT NOT VALID\n");
}

int main(int argc, char** argv){
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
    return 0;
}
