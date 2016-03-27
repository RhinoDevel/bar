
// MT, 2016feb29

#include <assert.h>

#ifndef NDEBUG
    #include "Stack.h"
#endif //NDEBUG

#include "json_state_val_end.h"

enum JsonState json_state_val_end(struct JsonStateInput * const inObj)
{
    enum JsonState retVal = JsonState_err;

    assert(inObj!=NULL);
    assert(!Stack_isEmpty(inObj->stack));

    if(inObj->i<inObj->len)
    {
        if(((struct JsonEle *)Stack_top(inObj->stack))->val->type==JsonType_prop)
        {
            inObj->pos = &(((struct JsonEle *)Stack_pop(inObj->stack))->next);
        }

        switch(inObj->str[inObj->i])
        {
            case ',':
                retVal = JsonState_val_next;
                break;
            case ']':
                retVal = JsonState_arr_end;
                break;
            case '}':
                retVal = JsonState_obj_end;
                break;

            default:
                break;
        }
    }

    return retVal;
}
