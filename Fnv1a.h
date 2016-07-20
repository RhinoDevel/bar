
// MT, 2016jul19

#ifndef MT_FNV1A
#define MT_FNV1A

#include <stdio.h>
#include <inttypes.h>

#ifdef	__cplusplus
extern "C" {
#endif //__cplusplus

char* Fnv1a_create_string(uint32_t const inHash);

/** Print FNV-1a hash given.
 */
void Fnv1a_print(uint32_t const inHash);

void Fnv1a_clear_cache();

/** FNV-1a 32-bit.
 *
 * - Returns NULL on error.
 * - Given file needs to be opened for reading in binary mode (and closed later).
 * - Caller takes ownership of returned pointer.
 * - Hard-coded for little-endian systems.
 */
uint32_t* Fnv1a_create_from_file(FILE * const inFilePtr);

char* Fnv1a_create_string_from_file(FILE * const inFilePtr);

char* Fnv1a_create_string_from_path(char const * const inPathPtr);

#ifdef	__cplusplus
}
#endif //__cplusplus

#endif //MT_FNV1A
