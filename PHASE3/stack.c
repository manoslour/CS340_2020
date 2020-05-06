#include "stack.h"

offsetStack* initOffsetStack(){
    offsetStack *tmp = (offsetStack*) malloc(sizeof(offsetStack));
    tmp->next = NULL;
    return tmp;
}

int isEmptyOffset(offsetStack *root){
    if(root == NULL)
        return 1;
    else
        return 0;
}

unsigned int peekOffset(offsetStack *root){
    if(isEmptyOffset(root))
        return -1;
    else
        return root->numoflocals;
}

void pushOffset(offsetStack **root, unsigned int offset){
    offsetStack *newNode = (offsetStack*) malloc(sizeof(offsetStack));
    newNode->numoflocals = offset;
    newNode->next = *root;
    *root = newNode;
}

unsigned int popOffset(offsetStack **root){
    unsigned int offset;
    if(isEmptyOffset(*root)){
        return -1;
    }
    else{
        offset = peekOffset(*root);
        *root = (*root)->next;
        return offset;
    }
}

//-------------------------------------------------------------------

counterStack* initCounterStack(){
    counterStack *tmp = (counterStack*) malloc(sizeof(counterStack));
    tmp->next = NULL;
    return tmp;
}

int isEmptyCounter(counterStack *root){
    if(root == NULL)
        return 1;
    else
        return 0;
}

unsigned int peekCounter(counterStack *root){
    if(isEmptyCounter(root)){
        printf("Stack is empty\n");
        return -1;
    }
    else{
        return root->loopcounter;
    }
}

void pushCounter(counterStack **root, unsigned int counter){
    counterStack *newNode = (counterStack*) malloc(sizeof(counterStack));
    newNode->loopcounter = counter;
    newNode->next = *root;
    *root = newNode;
    //printf("%d pushed to stack\n", counter);
}

unsigned int popCounter(counterStack **root){
    unsigned int counter;
    if(isEmptyCounter(*root)){
        return -1;
    }
    else{
        counter = peekCounter(*root);
        *root = (*root)->next;
        return counter;
    }
}