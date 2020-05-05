#include <stdio.h>
#include <stdlib.h>

typedef struct offsetStack{
    unsigned int numoflocals;
    struct offsetStack* next;
}offsetStack;

typedef struct loopStack{
    unsigned int counter;
    struct loopStack* next;
}loopStack;

offsetStack* initStack();

int isEmpty(offsetStack *root);

unsigned int peek(offsetStack *root);

void push(offsetStack *root, unsigned int offset);

unsigned int pop(offsetStack *root);