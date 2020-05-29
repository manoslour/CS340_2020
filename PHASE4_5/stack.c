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

//-------------------------------------------------------------------

funcStack* initFuncStack(){
    funcStack *tmp = (funcStack*) malloc(sizeof(funcStack));
    tmp->next = NULL;
    return tmp;
}

int isEmptyFunc(funcStack *root){
    if(root == NULL)
        return 1;
    else
        return 0;
}

symbol* peekFunc(funcStack *root){
    if(isEmptyFunc(root)){
        printf("Stack is empty\n");
        return NULL;
    }
    else{
        return root->func;
    }
}

void pushFunc(funcStack **root, symbol* func){
    funcStack *newNode = (funcStack*) malloc(sizeof(funcStack));
    newNode->func = func;
    newNode->next = *root;
    *root = newNode;
    printf("%s pushed to stack\n", func->name);
}

symbol* popFunc(funcStack **root){
    symbol* func;
    if(isEmptyFunc(*root)){
        return NULL;
    }
    else{
        func = peekFunc(*root);
        *root = (*root)->next;
        return func;
    }
}