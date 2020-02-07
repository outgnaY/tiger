#include "stack.h"
#include "util.h"
#include <stdlib.h>

Stack Stack_init(void)
{
    Stack s = checked_malloc(sizeof(*s));
    s->size = 0;
    s->top = NULL;
    return s;
} 

int Stack_push(Stack s, void* data)
{
    stackElem e = checked_malloc(sizeof(*e));
    e->data = data;
    e->next = s->top;
    s->top = e;
    s->size++;
    return TRUE;
}

void* Stack_pop(Stack s)
{
    /* empty stack */
    if(s->size == 0) {
        return NULL;
    }
    void* data = s->top->data;
    s->top = s->top->next;
    s->size--;
    return data;
}

void* Stack_peek(Stack s)
{
    /* empty stack */
    if(s->size == 0) {
        return NULL;
    }
    return s->top->data;
}

int Stack_empty(Stack s)
{
    return s->size == 0;
}

int Stack_size(Stack s)
{
    return s->size;
}
/*
int main(int argc, char **argv) {
    Stack s = Stack_init();
    int d1 = 1;
    int d2 = 2;
    int d3 = 3;
    int** data;
    Stack_push(s, &d1);
    Stack_push(s, &d2);
    Stack_peek(s, data);
    printf("data = %d\n", **data);
    Stack_pop(s, data);
    printf("data = %d\n", **data);
}
*/