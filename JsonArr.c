
// MT, 2016mar16

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "JsonArr.h"

void JsonArr_push(struct JsonArr * const inArr, enum JsonType const inType, void * const inVal)
{
    struct JsonEle * const ele = JsonEle_create(inType, inVal);

    assert(inArr!=NULL);
    assert(ele!=NULL);

    if(inArr->last==NULL) // => Array is empty.
    {
        inArr->ele->val->val = ele; // Adds first element to array.
    }
    else
    {
        inArr->last->next = ele; // Appends another element to array.
    }

    inArr->last = ele; // Remembers last element of array.
}

void JsonArr_init(struct JsonArr * const inArr, bool const inFree)
{
    assert(inArr!=NULL);

    if(inFree)
    {
        JsonEle_delete(inArr->ele);
    }
    inArr->ele = JsonEle_create(JsonType_arr, NULL);
    assert(inArr->ele!=NULL);
    inArr->last = NULL;
}

void JsonArr_delete(struct JsonArr * const inArr)
{
    assert(inArr!=NULL);

    JsonEle_delete(inArr->ele);
    free(inArr);
}

struct JsonArr * JsonArr_create()
{
    struct JsonArr * const retVal = malloc(sizeof *retVal);
    struct JsonArr buf;

    JsonArr_init(&buf, false);

    assert(retVal!=NULL);

    memcpy(retVal, &buf, sizeof *retVal);

    return retVal;
}
