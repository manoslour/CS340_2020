#include <stdio.h>
#include <stdlib.h>


int globmem; 



int main (){



	char *i= malloc(10); 

	int j = 0; 

	for (j=0 ; j<10; j++) {
		printf("Desmeusa apo %p  \n", i );
		i++;
	}

	free(i);

	return 0;
}
