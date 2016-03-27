
// MT, 2016feb29

#include <assert.h>
#include "JsonEle.h"
#include "json_state_val_begin.h"

enum JsonState json_state_val_begin(struct JsonStateInput * const inObj)
{
    enum JsonState retVal = JsonState_err;

    assert(inObj!=NULL);
    assert(inObj->i<inObj->len);
    assert(!Stack_isEmpty(inObj->stack));

    struct JsonEle * const top = (struct JsonEle *)Stack_top(inObj->stack);

    switch(top->val->type)
    {
        case JsonType_obj:
            if(inObj->str[inObj->i]=='"')
            {
                retVal = JsonState_prop_begin;
            }
            break;
        case JsonType_prop:
        case JsonType_arr:
            switch(inObj->str[inObj->i])
            {
                case '[':
                    retVal = JsonState_arr_begin;
                    break;
                case '{':
                    retVal = JsonState_obj_begin;
                    break;
                case 't':
                case 'f':
                    retVal = JsonState_boolean;
                    break;
                case '-':
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    retVal = JsonState_number;
                    break;
                case '"':
                    retVal = JsonState_string;
                    break;
                case 'n':
                    retVal = JsonState_null;
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }

    return retVal;
}
