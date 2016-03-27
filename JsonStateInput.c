
// MT, 2016feb26

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "JsonStateInput.h"

void JsonStateInput_addEle(struct JsonStateInput * const inJsonStateInput, enum JsonType const inType, void * const inVal)
{
    assert(inJsonStateInput!=NULL);
    assert(inJsonStateInput->pos!=NULL);

    *(inJsonStateInput->pos) = JsonEle_create(inType, inVal);
    assert(*(inJsonStateInput->pos)!=NULL);

    inJsonStateInput->pos = &((*(inJsonStateInput->pos))->next);
}

struct JsonStateInput * JsonStateInput_create(char * const inStr, bool const inTakesOwnership)
{
    struct JsonStateInput * const retVal = malloc(sizeof *retVal);
    struct JsonStateInput const buf = (struct JsonStateInput)
        {
            .str = inStr,
            .len = strlen(inStr),
            .takesOwnership = inTakesOwnership,

            .i = 0,
            .stack = Stack_create(false),
            .root = NULL,
            .pos = NULL
        };

    assert(inStr!=NULL);
    assert(retVal!=NULL);

    memcpy(retVal, &buf, sizeof *retVal);

    return retVal;
}

void JsonStateInput_delete(struct JsonStateInput * const inJsonStateInput)
{
    assert(inJsonStateInput!=NULL);

    if(inJsonStateInput->takesOwnership)
    {
        free(inJsonStateInput->str);
    }
    if(inJsonStateInput->root!=NULL)
    {
        JsonEle_delete(inJsonStateInput->root);
    }
    Stack_delete(inJsonStateInput->stack);
    free(inJsonStateInput);
}