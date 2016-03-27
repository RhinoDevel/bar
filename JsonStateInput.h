
// MT, 2016feb26

#ifndef MT_JSONSTATEINPUT
#define MT_JSONSTATEINPUT

#include <stddef.h>
#include "Stack.h"
#include "JsonType.h"
#include "JsonEle.h"

#ifdef	__cplusplus
extern "C" {
#endif //__cplusplus

struct JsonStateInput
{
    char * const str;
    size_t const len;
    bool const takesOwnership;

    size_t i;
    struct Stack * const stack;
    struct JsonEle * root;
    struct JsonEle * * pos;
};

/** Create new JSON element with given type and value as input.
 *  Add that new element to current position in given JSON state input object.
 *  Set JSON state input object's position to behind new element (next).
 */
void JsonStateInput_addEle(struct JsonStateInput * const inJsonStateInput, enum JsonType const inType, void * const inVal);
struct JsonStateInput * JsonStateInput_create(char * const inStr, bool const inTakesOwnership);
void JsonStateInput_delete(struct JsonStateInput * const inJsonStateInput);

#ifdef	__cplusplus
}
#endif //__cplusplus

#endif //MT_JSONSTATEINPUT
