
// MT, 2016mar07

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "JsonVal.h"
#include "JsonEle.h"
#include "JsonProp.h"

bool JsonVal_areEqual(struct JsonVal const * const inA, struct JsonVal const * const inB, bool const inIgnoreArrOrder)
{
    bool retVal = false;
    
    assert(inA!=NULL);
    assert(inB!=NULL);
    assert(inA->type!=JsonType_prop);
    assert(inB->type!=JsonType_prop);
    
    if(inA->type==inB->type)
    {
        switch(inA->type)
        {
            case JsonType_null:
                assert(inA->val==NULL);
                assert(inB->val==NULL);
                retVal = true;
                break;
                
            case JsonType_boolean:
                assert(inA->val!=NULL);
                assert(inB->val!=NULL);
                retVal = *((bool const *)(inA->val))==*((bool const *)(inB->val));
                break;
                
            case JsonType_number:
                assert(inA->val!=NULL);
                assert(inB->val!=NULL);
                retVal = *((double const *)(inA->val))==*((double const *)(inB->val));
                break;
                
            case JsonType_string:
                assert(inA->val!=NULL);
                assert(inB->val!=NULL);
                retVal = strcmp((char const *)(inA->val), (char const *)(inB->val))==0;
                break;
                
            case JsonType_arr:
                retVal = JsonEle_arrAreEqual((struct JsonEle const *)(inA->val), (struct JsonEle const *)(inB->val), inIgnoreArrOrder); // *** "RECURSION" ***
                break;
                
            case JsonType_obj:
                retVal = JsonEle_objAreEqual((struct JsonEle const *)(inA->val), (struct JsonEle const *)(inB->val), inIgnoreArrOrder); // *** "RECURSION" ***
                break;
                
            case JsonType_prop: // (falls through)
            default:
                assert(false);
                break;
        }
    }
    
    return retVal;
}

void JsonVal_delete(struct JsonVal * const inJsonVal)
{
    assert(inJsonVal!=NULL);

    switch(inJsonVal->type)
    {
        case JsonType_null:
            assert(inJsonVal->val==NULL);
            break;

        case JsonType_boolean:
        case JsonType_number:
        case JsonType_string:
            assert(inJsonVal->val!=NULL);
            free(inJsonVal->val);
            break;

        case JsonType_arr:
        case JsonType_obj:
            if(inJsonVal->val!=NULL)
            {
                JsonEle_delete((struct JsonEle *)(inJsonVal->val)); // *** RECURSION (see JsonEle). ***
            }
            //
            // Otherwise: Empty array/object.

            break;

        case JsonType_prop:
            assert(inJsonVal->val!=NULL);
            JsonProp_delete((struct JsonProp *)(inJsonVal->val)); // *** RECURSION (see JsonProp). ***
            break;

        default:
            assert(false);
            break;
    }

    free(inJsonVal);
}

struct JsonVal * JsonVal_create(enum JsonType const inType, void * const inVal)
{
    struct JsonVal * const retVal = malloc(sizeof *retVal);
    struct JsonVal const buf = (struct JsonVal)
        {
            .type = inType,
            .val = inVal
        };

    assert(retVal!=NULL);

    memcpy(retVal, &buf, sizeof *retVal);

    return retVal;
}
