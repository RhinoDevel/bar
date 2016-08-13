
// MT, 2016mar01

#ifndef MT_STR
#define MT_STR

#include "stddef.h"

#ifdef __cplusplus
extern "C" {
#endif

char* Str_from_long_create(long const inVal);
    
char* Str_from_double_create(double const inVal);

/** Return a new string holding given string A plus string B.
 *
 * * Takes owneship of string A!
 * - Uses realloc() to append string B directly behind memory of string A.
 * - Returns NULL on error.
 */
char* Str_append_create(char * const inStrA, char const * const inStrB);

/** Return a new string holding given string A plus string B.
 *
 * - Does NOT take ownership of any given parameter.
 * - Does NOT use realloc().
 * - Returns NULL on error.
 */
char* Str_concat_create(char const * const inStrA, char const * const inStrB);

/** Return a copy of the given ('\0' terminated) string.
 *
 * - Returns NULL on error.
 */
char* Str_copy_create(char const * const inStr);

/** Tries to parse double value from given position in given string
 *  and returns a pointer to the double value.
 *  Increments index given on success.
 *
 *  - Caller takes ownership of return value.
 *  - Returns NULL, if parsing failed.
 */
double* Str_double_create(char const * const inStr, size_t const inLen, size_t * const inOutIndex);

/** Tries to parse string starting from given position in given string
 *  and returns a pointer to the parsed string.
 *
 *  - The string is expected to be enclosed in given tag characters (e.g. ..."the string to parse"...).
 *  - The tag characters behind a '\' will be added to the output string.
 *  - Returns NULL, if parsing failed.
 */
char* Str_string_create(char const * const inStr, size_t const inLen, char const inTag, size_t * const inOutIndex);

#ifdef __cplusplus
}
#endif

#endif // MT_STR
