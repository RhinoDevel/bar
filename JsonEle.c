
// MT, 2016mar07

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "JsonEle.h"
#include "JsonVal.h"
#include "JsonProp.h"

static int JsonEle_getCount(struct JsonEle const * const inEle)
{
    int retVal = 0;
    struct JsonEle const * cur = inEle;

    while(cur!=NULL)
    {
        ++retVal;
        cur = cur->next;
    }

    return retVal;
}

static bool JsonEle_objHasAllProperties(struct JsonEle const * const inObjEle, struct JsonEle const * const inPropertiesEle, bool const inIgnoreArrOrder)
{
    bool retVal = true; // TRUE by default.

    if(inPropertiesEle!=NULL)
    {
        if(inObjEle!=NULL)
        {
            // Get count of elements in JSON object and prepare temporary array to hold pointers to elements:

            size_t const arrLen = (size_t)JsonEle_getCount(inObjEle);
            size_t i = 0;

            assert(arrLen>0);

            struct JsonEle const * * const elePtrArr = malloc(arrLen*(sizeof *elePtrArr));
            struct JsonEle const * cur = inObjEle;

            assert(elePtrArr!=NULL);

            // Copy pointers to elements to temporary array:
            //
            do
            {
                assert(i<arrLen);
                assert((cur->val->type==JsonType_prop)&&(cur->val->val!=NULL));

                elePtrArr[i] = cur;

                ++i;
                cur = cur->next;
            }while(cur!=NULL);
            assert(i==arrLen);

            // Iterate through all properties:
            //
            cur = inPropertiesEle;
            do
            {
                assert((cur->val->type==JsonType_prop)&&(cur->val->val!=NULL));

                // Find current element's type and value in temporary array:
                //
                for(i = 0;i<arrLen;++i)
                {
                    if((elePtrArr[i]!=NULL)&&JsonProp_areEqual((struct JsonProp const * const)(elePtrArr[i]->val->val), (struct JsonProp const * const)(cur->val->val), inIgnoreArrOrder)) // *** "RECURSION" ***
                    {
                        elePtrArr[i] = NULL; // "Remove" found element from temporary array.
                        break; // Found
                    }
                }
                if(i==arrLen)
                {
                    retVal = false;
                    break; // Not found.
                }

                cur = cur->next;
            }while(cur!=NULL);

            free(elePtrArr);
        }
        else // => Empty object given.
        {
            retVal = false;
        }
    }
    //
    // Otherwise: No properties given => All given exist.

    return retVal;
}

/** Each value element given must exist in array given.
 *  Equal values existing multiple times in values given must exist at least the same number of times in array given!
 */
static bool JsonEle_arrHasAllValues(struct JsonEle const * const inArrEle, struct JsonEle const * const inValuesEle, bool const inIgnoreArrOrder)
{
    bool retVal = true; // TRUE by default.

    if(inValuesEle!=NULL)
    {
        if(inArrEle!=NULL)
        {
            // Get count of elements in JSON array and prepare temporary array to hold pointers to elements:

            size_t const arrLen = (size_t)JsonEle_getCount(inArrEle);
            size_t i = 0;

            assert(arrLen>0);

            struct JsonEle const * * const elePtrArr = malloc(arrLen*(sizeof *elePtrArr));
            struct JsonEle const * cur = inArrEle;

            assert(elePtrArr!=NULL);

            // Copy pointers to elements to temporary array:
            //
            do
            {
                assert(i<arrLen);
                elePtrArr[i] = cur;

                ++i;
                cur = cur->next;
            }while(cur!=NULL);
            assert(i==arrLen);

            // Iterate through all values:
            //
            cur = inValuesEle;
            do
            {
                // Find current element's type and value in temporary array:
                //
                for(i = 0;i<arrLen;++i)
                {
                    if((elePtrArr[i]!=NULL)&&JsonEle_areEqual(elePtrArr[i], cur, inIgnoreArrOrder)) // *** "RECURSION" ***
                    {
                        elePtrArr[i] = NULL; // "Remove" found element from temporary array.
                        break; // Found
                    }
                }
                if(i==arrLen)
                {
                    retVal = false;
                    break; // Not found.
                }

                cur = cur->next;
            }while(cur!=NULL);

            free(elePtrArr);
        }
        else // => Empty array given.
        {
            retVal = false;
        }
    }
    //
    // Otherwise: No values given => All given exist.

    return retVal;
}

struct JsonEle * JsonEle_objGetPropVal(struct JsonEle const * const inFirstEle, char const * const inPropName)
{
    struct JsonEle * retVal = NULL;
    struct JsonEle const * cur = inFirstEle;

    assert((inPropName!=NULL)&&(inPropName[0]!='\0'));

    while(cur!=NULL)
    {
        assert(cur->val!=NULL);
        assert(cur->val->type==JsonType_prop);
        assert(cur->val->val!=NULL);

        struct JsonProp const * const prop = (struct JsonProp const *)(cur->val->val);

        assert(prop->ele!=NULL);
        assert((prop->name!=NULL)&&(prop->name[0]!='\0'));

        if(strcmp(inPropName, prop->name)==0)
        {
            retVal = prop->ele;
            break; // Found
        }

        cur = cur->next;
    }

    return retVal;
}

bool JsonEle_arrAreEqual(struct JsonEle const * const inA, struct JsonEle const * const inB, bool const inIgnoreArrOrder)
{
    bool retVal = false;

    if(JsonEle_getCount(inA)==JsonEle_getCount(inB))
    {
        if(inIgnoreArrOrder)
        {
            retVal = JsonEle_arrHasAllValues(inA, inB, true); // *** "RECURSION" ***
        }
        else
        {
            struct JsonEle const * curA = inA,
                * curB = inB;

            while(curA!=NULL)
            {
                assert(curB!=NULL);

                if(!JsonEle_areEqual(curA, curB, false)) // *** "RECURSION" ***
                {
                    break; // Elements with different types and/or values found at current index.
                }

                curA = curA->next;
                curB = curB->next;
            }
            if(curA==NULL)
            {
                retVal = true;
            }
        }
    }

    return retVal;
}

bool JsonEle_objAreEqual(struct JsonEle const * const inA, struct JsonEle const * const inB, bool const inIgnoreArrOrder)
{
    bool retVal = false;

    if(JsonEle_getCount(inA)==JsonEle_getCount(inB))
    {
        retVal = JsonEle_objHasAllProperties(inA, inB, inIgnoreArrOrder); // *** "RECURSION" ***
    }

    return retVal;
}

bool JsonEle_areEqual(struct JsonEle const * const inA, struct JsonEle const * const inB, bool const inIgnoreArrOrder)
{
    assert(inA!=NULL);
    assert(inB!=NULL);

    return JsonVal_areEqual(inA->val, inB->val, inIgnoreArrOrder); // *** "RECURSION" ***
}

void JsonEle_delete(struct JsonEle * const inJsonEle)
{
    assert(inJsonEle!=NULL);

    struct JsonEle * cur = inJsonEle,
        * next = NULL;

    do
    {
        next = cur->next;
        JsonVal_delete(cur->val); // *** RECURSION (see JsonVal). ***
        free(cur);
        cur = next;
    }while(cur!=NULL);
}

struct JsonEle * JsonEle_create(enum JsonType const inType, void * const inVal)
{
    struct JsonEle * const retVal = malloc(sizeof *retVal);
    struct JsonEle const buf = (struct JsonEle)
        {
            .val = JsonVal_create(inType, inVal),
            .next = NULL
        };

    assert(retVal!=NULL);
    assert(buf.val!=NULL);

    memcpy(retVal, &buf, sizeof *retVal);

    return retVal;
}
