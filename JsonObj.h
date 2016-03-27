
// MT, 2016mar17

#ifndef MT_JSONOBJ
#define MT_JSONOBJ

#include <stdbool.h>
#include "JsonType.h"
#include "JsonEle.h"

#ifdef	__cplusplus
extern "C" {
#endif //__cplusplus

struct JsonObj
{
    struct JsonEle * ele;
    struct JsonEle * last;
};

/** Add given value of given type with given property name to object.
 *
 *  - Takes ownership of objects at pointers given.
 */
void JsonObj_add(struct JsonObj * const inObj, char * const inPropName, enum JsonType const inType, void * const inVal);

/** (Re-)initializes internal object.
 *
 *  - Does not free internal object and properties' allocated memory, if inFree is set to false.
 */
void JsonObj_init(struct JsonObj * const inObj, bool const inFree);

/** Deletes JSON object.
 */
void JsonObj_delete(struct JsonObj * const inObj);

/** Creates new JSON object.
 */
struct JsonObj * JsonObj_create();

#ifdef	__cplusplus
}
#endif //__cplusplus

#endif //MT_JSONOBJ
