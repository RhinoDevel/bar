
// MT, 2016feb28

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "StackEle.h"

void StackEle_delete(struct StackEle * const inStackEle, bool const inFreeVal)
{
    assert(inStackEle!=NULL);

    if(inFreeVal)
    {
        free(inStackEle->val);
    }
    free(inStackEle);
}

struct StackEle * StackEle_create(void * const inVal, struct StackEle * const inBelow)
{
    struct StackEle * const retVal = malloc(sizeof *retVal),
        buf = (struct StackEle){ .val = inVal, .below = inBelow };

    assert(inVal!=NULL);
    assert(retVal!=NULL);

    memcpy(retVal, &buf, sizeof *retVal);

    return retVal;
}
