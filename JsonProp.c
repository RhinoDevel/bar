
// MT, 2016mar07

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "JsonProp.h"
#include "JsonEle.h"

bool JsonProp_areEqual(struct JsonProp const * const inA, struct JsonProp const * const inB, bool const inIgnoreArrOrder)
{
    bool retVal = false;
    
    assert(inA!=NULL);
    assert(inB!=NULL);
    assert((inA->name!=NULL)&&(inA->name[0]!='\0'));
    assert((inB->name!=NULL)&&(inB->name[0]!='\0'));
    assert(inA->ele!=NULL);
    assert(inB->ele!=NULL);
    
    if(strcmp(inA->name, inB->name)==0)
    {
        retVal = JsonEle_areEqual(inA->ele, inB->ele, inIgnoreArrOrder); // *** "RECURSION" ***
    }
    
    return retVal;
}

void JsonProp_delete(struct JsonProp * const inJsonProp)
{
    assert(inJsonProp!=NULL);
    assert(inJsonProp->name!=NULL);

    free(inJsonProp->name);
    if(inJsonProp->ele!=NULL)
    {
        JsonEle_delete(inJsonProp->ele); // *** RECURSION (see JsonEle). ***
    }
    free(inJsonProp);
}

struct JsonProp * JsonProp_create(char * const inName)
{
    struct JsonProp * const retVal = malloc(sizeof *retVal);
    struct JsonProp const buf = (struct JsonProp)
    {
        .name = inName,
        .ele = NULL
    };

    assert(inName!=NULL);
    assert(retVal!=NULL);

    memcpy(retVal, &buf, sizeof *retVal);

    return retVal;
}
