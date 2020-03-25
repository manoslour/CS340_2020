#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* generateName(int nameCount){
	char *name = "$";
	char buffer[100];
	char number = itoa(nameCount, buffer, 10);
	strcat(name, number);
	return name;
}

int main(){
    printf("FUNC NAME = %s", generateName(3));
    return 0;
}