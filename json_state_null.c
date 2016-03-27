
// MT, 2016feb29

#include <assert.h>
#include <string.h>
#include "Deb.h"
#include "Stack.h"
#include "json_state_null.h"

enum JsonState json_state_null(struct JsonStateInput * const inObj)
{
    enum JsonState retVal = JsonState_err;

    assert(inObj!=NULL);
    assert(inObj->i<inObj->len);
    assert(inObj->str[inObj->i]=='n');
    assert(!Stack_isEmpty(inObj->stack));

    if((inObj->i+3)<inObj->len)
    {
        if(strncmp(inObj->str+inObj->i, "null", 4)==0)
        {
            inObj->i += 4;
            //Deb_line("null");
            JsonStateInput_addEle(inObj, JsonType_null, NULL);

            retVal = JsonState_val_end;
        }
    }

    return retVal;
}
