
// MT, 2016mar17

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "JsonProp.h"
#include "JsonObj.h"

void JsonObj_add(struct JsonObj * const inObj, char * const inPropName, enum JsonType const inType, void * const inVal)
{
    struct JsonEle * const propEle = JsonEle_create(JsonType_prop, JsonProp_create(inPropName)),
        * const valEle = JsonEle_create(inType, inVal);

    assert(inObj!=NULL);
    assert(propEle!=NULL);
    assert(valEle!=NULL);

    ((struct JsonProp *)(propEle->val->val))->ele = valEle;

    if(inObj->last==NULL) // => Object has no properties, yet.
    {
        inObj->ele->val->val = propEle; // Adds first element to object.
    }
    else
    {
        inObj->last->next = propEle; // Appends another element to object.
    }

    inObj->last = propEle; // Remembers last element of object.
}

void JsonObj_init(struct JsonObj * const inObj, bool const inFree)
{
    assert(inObj!=NULL);

    if(inFree)
    {
        JsonEle_delete(inObj->ele);
    }
    inObj->ele = JsonEle_create(JsonType_obj, NULL);
    assert(inObj->ele!=NULL);
    inObj->last = NULL;
}

void JsonObj_delete(struct JsonObj * const inObj)
{
    assert(inObj!=NULL);

    JsonEle_delete(inObj->ele);
    free(inObj);
}

struct JsonObj * JsonObj_create()
{
    struct JsonObj * const retVal = malloc(sizeof *retVal);
    struct JsonObj buf;

    JsonObj_init(&buf, false);

    assert(retVal!=NULL);

    memcpy(retVal, &buf, sizeof *retVal);

    return retVal;
}
