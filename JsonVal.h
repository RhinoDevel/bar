
// MT, 2016mar07

#ifndef MT_JSONVAL
#define MT_JSONVAL

#include <stdbool.h>
#include "JsonType.h"

#ifdef	__cplusplus
extern "C" {
#endif //__cplusplus

struct JsonVal
{
    enum JsonType const type;
    void * val;
};

bool JsonVal_areEqual(struct JsonVal const * const inA, struct JsonVal const * const inB, bool const inIgnoreArrOrder);
    
/** Delete JSON value given.
 *
 *  - Deletes value object, too.
 */
void JsonVal_delete(struct JsonVal * const inJsonVal);

/** Create new JSON value.
 *
 *  - Takes ownership of given value object.
 */
struct JsonVal * JsonVal_create(enum JsonType const inType, void * const inVal);

#ifdef	__cplusplus
}
#endif //__cplusplus

#endif //MT_JSONELE
