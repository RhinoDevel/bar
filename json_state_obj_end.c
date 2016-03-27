
// MT, 2016mar03

#include <assert.h>
#include "Obj.h"
#include "Deb.h"
#include "Stack.h"
#include "json_state_obj_end.h"

enum JsonState json_state_obj_end(struct JsonStateInput * const inObj)
{
    enum JsonState retVal = JsonState_err;
    void const * buf = NULL;

    assert(inObj!=NULL);
    assert(inObj->i<inObj->len);
    assert(inObj->str[inObj->i]=='}');

    ++inObj->i;
    //Deb_line("}");

    if(((struct JsonEle *)Stack_top(inObj->stack))->val->type==JsonType_prop)
    {
        inObj->pos = &(((struct JsonEle *)Stack_pop(inObj->stack))->next);
    }

    buf = Stack_pop(inObj->stack);
    if(buf!=NULL)
    {
        struct JsonEle * const container = (struct JsonEle *)buf;

        if(container->val->type==JsonType_obj)
        {
            if(inObj->i<inObj->len)
            {
                inObj->pos = &(container->next);
                retVal = JsonState_val_end;
            }
            else
            {
                if(Stack_isEmpty(inObj->stack))
                {
                    retVal = JsonState_done;
                }
            }
        }
    }

    return retVal;
}
