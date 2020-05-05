#include "stack.h"

offsetStack* initStack(){
    offsetStack *tmp = (offsetStack*) malloc(sizeof(offsetStack));
    tmp->next = NULL;
    return tmp;
}

int isEmpty(offsetStack *root){
    if(root == NULL)
        return 1;
    else
        return 0;
}

unsigned int peek(offsetStack *root){
    if(isEmpty(root))
        return -1;
    else
        return root->numoflocals;
}

void push(offsetStack *root, unsigned int offset){
    offsetStack *newNode = (offsetStack*) malloc(sizeof(offsetStack));
    newNode->numoflocals = offset;
    newNode->next = root;
    root = newNode;
    printf("Node Offset = %d\n", offset);
    printf("Pushed node to stack\n");
}

unsigned int pop(offsetStack *root){
    unsigned int offset;
    if(isEmpty(root)){
        return -1;
    }
    else{
        offset = peek(root);
        root = root->next;
        return offset;
    }
}

/*
int main(){

    offsetStack *root = NULL;

    push(&root, 10);
    push(&root, 20);
    push(&root, 30);
    printf("%d popped from stack\n", pop(&root));
    printf("Top element is %d\n", peek(root));
    return 0;
}
*/