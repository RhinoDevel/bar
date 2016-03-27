
// MT, 2016mar15

#ifndef MT_JSONARR
#define MT_JSONARR

#include <stdbool.h>
#include "JsonType.h"
#include "JsonEle.h"

#ifdef	__cplusplus
extern "C" {
#endif //__cplusplus

struct JsonArr
{
    struct JsonEle * ele;
    struct JsonEle * last;
};

/** Add given value of given type to end of array.
 *
 *  - Takes ownership of object at pointer given.
 */
void JsonArr_push(struct JsonArr * const inArr, enum JsonType const inType, void * const inVal);
    
/** (Re-)initializes internal array.
 *
 *  - Does not free internal array and elements' allocated memory, if inFree is set to false.
 */
void JsonArr_init(struct JsonArr * const inArr, bool const inFree);

/** Deletes JSON array.
 */
void JsonArr_delete(struct JsonArr * const inArr);
    
/** Creates new JSON array.
 */
struct JsonArr * JsonArr_create();

#ifdef	__cplusplus
}
#endif //__cplusplus

#endif //MT_JSONARR
