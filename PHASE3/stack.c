#include "stack.h"

StackNode* initStack(){
    StackNode *tmp = (StackNode*) malloc(sizeof(StackNode));
    tmp->next = NULL;
    return tmp;
}

int isEmpty(StackNode *root){
    if(root == NULL)
        return 1;
    else
        return 0;
}

unsigned int peek(StackNode *root){
    if(isEmpty(root))
        return -1;
    else
        return root->numoflocals;
}

void push(StackNode *root, unsigned int offset){
    StackNode *newNode = (StackNode*) malloc(sizeof(StackNode));
    newNode->numoflocals = offset;
    newNode->next = root;
    root = newNode;
    printf("Node Offset = %d\n", offset);
    printf("Pushed node to stack\n");
}

unsigned int pop(StackNode *root){
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

    StackNode *root = NULL;

    push(&root, 10);
    push(&root, 20);
    push(&root, 30);
    printf("%d popped from stack\n", pop(&root));
    printf("Top element is %d\n", peek(root));
    return 0;
}
*/