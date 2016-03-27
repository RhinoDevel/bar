
// MT, 2016feb29

#include <assert.h>
#include <stdlib.h>
#include "Obj.h"
#include "Deb.h"
#include "Str.h"
#include "JsonProp.h"
#include "json_state_prop_begin.h"

enum JsonState json_state_prop_begin(struct JsonStateInput * const inObj)
{
    enum JsonState retVal = JsonState_err;

    assert(inObj!=NULL);
    assert(inObj->i<inObj->len);
    assert(!Stack_isEmpty(inObj->stack));
    assert(((struct JsonEle *)Stack_top(inObj->stack))->val->type==JsonType_obj);
    assert(inObj->str[inObj->i]=='"');

    char * const prop = Str_string_create(inObj->str, inObj->len, '"', &(inObj->i));

    if(prop!=NULL)
    {
        //Deb_line("\"%s\"", prop);

        if(inObj->str[inObj->i]==':')
        {
            ++inObj->i;
            //Deb_line(":");

            *(inObj->pos) = JsonEle_create(JsonType_prop, JsonProp_create(prop));
            Stack_push(inObj->stack, *(inObj->pos));
            inObj->pos = &(((struct JsonProp *)((*(inObj->pos))->val->val))->ele);

            retVal = JsonState_val_begin;
        }
    }

    return retVal;
}
