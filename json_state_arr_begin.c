
// MT, 2016feb26

#include <assert.h>
#include "Obj.h"
#include "Deb.h"
#include "Stack.h"
#include "json_state_arr_begin.h"

enum JsonState json_state_arr_begin(struct JsonStateInput * const inObj)
{
    enum JsonState retVal = JsonState_err;

    assert(inObj!=NULL);
    assert(inObj->i<inObj->len);
    assert(inObj->str[inObj->i]=='[');

    ++inObj->i;
    //Deb_line("[");

    if(inObj->i<inObj->len)
    {
        if(inObj->root==NULL)
        {
            inObj->root = JsonEle_create(JsonType_arr, NULL);
            inObj->pos = (struct JsonEle * *)(&(inObj->root->val->val));

            Stack_push(inObj->stack, inObj->root);
        }
        else
        {
            *(inObj->pos) = JsonEle_create(JsonType_arr, NULL);
            Stack_push(inObj->stack, *(inObj->pos));
            inObj->pos = (struct JsonEle * *)(&((*(inObj->pos))->val->val));
        }

        if(inObj->str[inObj->i]==']')
        {
            retVal = JsonState_arr_end;
        }
        else
        {
            retVal = JsonState_val_begin;
        }
    }

    return retVal;
}
