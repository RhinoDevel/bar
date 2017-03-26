
// MT, 2016feb28

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "Stack.h"

void Stack_push(struct Stack * const inStack, void * const inVal)
{
    assert(inStack!=NULL);

    inStack->top = StackEle_create(inVal, inStack->top);
}

void* Stack_pop(struct Stack * const inStack)
{
    void* retVal = Stack_top(inStack);

    if(retVal!=NULL)
    {
        struct StackEle * const below = inStack->top->below;

        StackEle_delete(inStack->top, false);
        inStack->top = below;
    }

    return retVal;
}

void* Stack_top(struct Stack const * const inStack)
{
    void* retVal = NULL;

    assert(inStack!=NULL);

    if(!Stack_isEmpty(inStack))
    {
        retVal = inStack->top->val;
    }

    return retVal;
}

void Stack_clear(struct Stack * const inStack)
{
    assert(inStack!=NULL);

    while(!Stack_isEmpty(inStack))
    {
        void * const val = Stack_pop(inStack);

        if(inStack->takesOwnership)
        {
            free(val);
        }
    }
}

void Stack_print_str(struct Stack * const inStack)
{
    struct Stack * buf = Stack_create(false);

    while(inStack->top!=NULL)
    {
        char * const str = (char*)Stack_pop(inStack);

        printf("\"%s\"\n", str);

        Stack_push(buf, str);
    }

    inStack->top = buf->top;    // Kind
    buf->top = NULL;            // of
    Stack_delete(buf);          // hard-coded.

    Stack_flip(inStack);
}

void Stack_delete(struct Stack * const inStack)
{
    Stack_clear(inStack);
    free(inStack);
}

struct Stack * Stack_create(bool const inTakesOwnership)
{
    struct Stack * const retVal = malloc(sizeof *retVal),
        buf = (struct Stack){ .takesOwnership = inTakesOwnership, .top = NULL };

    assert(retVal!=NULL);

    memcpy(retVal, &buf, sizeof *retVal);

    return retVal;
}

void Stack_flip(struct Stack * const inStack)
{
    assert(inStack!=NULL);

    struct Stack * const buf = Stack_create(false);

    while(inStack->top!=NULL)
    {
        Stack_push(buf, Stack_pop(inStack));
    }

    inStack->top = buf->top; // Kind of
    buf->top = NULL;         // hard-coded.
    Stack_delete(buf);
}

bool Stack_isEmpty(struct Stack const * const inStack)
{
    assert(inStack!=NULL);

    return inStack->top==NULL;
}

int Stack_count(struct Stack const * const inStack)
{
    assert(inStack!=NULL);

    int c = 0;
    struct StackEle const * e = inStack->top;

    while(e!=NULL)
    {
        ++c;
        e = e->below;
    }
    return c;
}
