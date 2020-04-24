#include <stdio.h>
#include <stdlib.h>

typedef struct StackNode{
    unsigned int offset;
    struct StackNode *next;
}StackNode;

int isEmpty(StackNode *root);

unsigned int peek(StackNode *root);

void push(StackNode *root, unsigned int offset);

unsigned int pop(StackNode *root);