
// MT, 2016mar06

#ifndef MT_JSONELE
#define MT_JSONELE

#include <stdbool.h>
#include "JsonType.h"
#include "JsonVal.h"

#ifdef	__cplusplus
extern "C" {
#endif //__cplusplus

struct JsonEle
{
    struct JsonVal * val;
    struct JsonEle * next;
};

/** Expects the first element of an object or NULL for an empty object given.
 */
struct JsonEle * JsonEle_objGetPropVal(struct JsonEle const * const inFirstEle, char const * const inPropName);

/** Expects the first elements of both arrays or NULL for an empty array.
 */
bool JsonEle_arrAreEqual(struct JsonEle const * const inA, struct JsonEle const * const inB, bool const inIgnoreArrOrder);

/** Expects the first elements (holding JSON properties) of both objects or NULL for an empty object.
 */
bool JsonEle_objAreEqual(struct JsonEle const * const inA, struct JsonEle const * const inB, bool const inIgnoreArrOrder);

bool JsonEle_areEqual(struct JsonEle const * const inA, struct JsonEle const * const inB, bool const inIgnoreArrOrder);

/** Deletes JSON element.
 *
 *  - Also deletes its JSON value object.
 */
void JsonEle_delete(struct JsonEle * const inJsonEle);

/** Creates new JSON element.
 *
 *  - Takes ownership of given value.
 */
struct JsonEle * JsonEle_create(enum JsonType const inType, void * const inVal);

#ifdef	__cplusplus
}
#endif //__cplusplus

#endif //MT_JSONELE
