#include <stdio.h>
#include <stdlib.h>

typedef struct offsetStack{
    unsigned int numoflocals;
    struct offsetStack* next;
}offsetStack;

typedef struct counterStack{
    unsigned int loopcounter;
    struct counterStack* next;
}counterStack;

offsetStack* initOffsetStack();

int isEmptyOffset(offsetStack *root);

unsigned int peekOffset(offsetStack *root);

void pushOffset(offsetStack **root, unsigned int offset);

unsigned int popOffset(offsetStack **root);

counterStack* initCounterStack();

int isEmptyCounter(counterStack *root);

unsigned int peekCounter(counterStack *root);

void pushCounter(counterStack **root, unsigned int counter);

unsigned int popCounter(counterStack **root);