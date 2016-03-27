
// MT, 2016feb26

#include <assert.h>
#include "json_state_begin.h"

enum JsonState json_state_begin(struct JsonStateInput * const inObj)
{
    enum JsonState retVal = JsonState_err;

    assert(inObj!=NULL);
    assert(inObj->i==0);
    assert(inObj->i<inObj->len);

    switch(inObj->str[inObj->i])
    {
        case '[':
            retVal = JsonState_arr_begin;
            break;
        case '{':
            retVal = JsonState_obj_begin;
            break;

        default:
            break;
    }

    return retVal;
}
