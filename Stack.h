
// MT, 2016feb27

#ifndef MT_STACK
#define MT_STACK

#include <stdbool.h>
#include "StackEle.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Stack
{
    bool const takesOwnership;
    struct StackEle * top;
};

/** Push given pointer on top of stack.
 */
void Stack_push(struct Stack * const inStack, void * const inVal);

/** Pop and return pointer from top of stack.
 *
 * - Returns NULL, if nothing on stack.
 * - Caller always takes ownership of object at returned pointer.
 */
void* Stack_pop(struct Stack * const inStack);

/** Return pointer from top of stack.
 *
 * - Returns NULL, if nothing on stack.
 * - Stack keeps ownership of object at returned pointer.
 */
void* Stack_top(struct Stack const * const inStack);

/** All values on stack get removed.
 *
 * - Additionally frees memory at pointers on stack,
 *   if stack was created with ownership parameter set to true.
 */
void Stack_clear(struct Stack * const inStack);

/** DANGEROUS: Assumes all entries to be C strings (!) and prints them.
 */
void Stack_print_str(struct Stack * const inStack);

/** All values on stack get removed and memory at given pointer gets freed.
 *
 * - Additionally frees memory at pointers on stack,
 *   if stack was created with ownership parameter set to true.
 */
void Stack_delete(struct Stack * const inStack);

/** Creates and returns new stack object on heap.
 *
 * - If given ownership parameter is true,
 *   memory at pointers on stack will also be freed
 *   during stack clearing and deletion.
 */
struct Stack * Stack_create(bool const inTakesOwnership);

/** Reverse order of all elements on stack (top gets to bottom, bottom to top, etc.).
 */
void Stack_flip(struct Stack * const inStack);

/** Return true, if no elements on given stack.
 */
bool Stack_isEmpty(struct Stack const * const inStack);

/** Return count of objects on stack given.
 */
int Stack_count(struct Stack const * const inStack);

#ifdef __cplusplus
}
#endif

#endif // MT_STACK
