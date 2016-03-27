
// MT, 2016feb29

#include <assert.h>
#include "Deb.h"
#include "Stack.h"
#include "json_state_val_next.h"

enum JsonState json_state_val_next(struct JsonStateInput * const inObj)
{
    assert(inObj!=NULL);
    assert(inObj->i<inObj->len);
    assert(inObj->str[inObj->i]==',');
    assert(!Stack_isEmpty(inObj->stack));

    ++inObj->i;
    //Deb_line(",");

    return JsonState_val_begin;
}
