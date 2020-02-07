#ifndef STACK_H
#define STACK_H

/* a simple implement of linked stack */
#include "util.h"

typedef struct stackElem_* stackElem;

struct stackElem_ {
    void *data;
    stackElem next;
};

struct Stack_ {
    int size;
    stackElem top;
};
typedef struct Stack_* Stack;

Stack Stack_init(void);

int Stack_push(Stack s, void* data);

void* Stack_pop(Stack s);

void* Stack_peek(Stack s);

int Stack_empty(Stack s);

#endif  /* STACK_H */