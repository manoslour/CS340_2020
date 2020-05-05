#include <stdio.h>
#include <stdlib.h>

typedef struct StackNode{
    unsigned int numoflocals;
    struct StackNode *next;
}StackNode;

StackNode* initStack();

int isEmpty(StackNode *root);

unsigned int peek(StackNode *root);

void push(StackNode *root, unsigned int offset);

unsigned int pop(StackNode *root);