
// MT, 2016feb28

#ifndef MT_OBJ
#define MT_OBJ

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Create and return object with given value (and type) on heap.
 */
bool* Obj_bool_create(bool const inVal);

/** Create and return object with given value (and type) on heap.
 */
char* Obj_char_create(char const inVal);

/** Create and return object with given value (and type) on heap.
 */
int* Obj_int_create(int const inVal);

/** Create and return object with given value (and type) on heap.
 */
double* Obj_double_create(double const inVal);

#ifdef __cplusplus
}
#endif

#endif // MT_OBJ
