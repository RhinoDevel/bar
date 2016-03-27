
// MT, 2015oct27

#ifndef MT_SHA1
#define MT_SHA1

#include <stdio.h>
#include <inttypes.h>

#ifdef	__cplusplus
extern "C" {
#endif //__cplusplus

char* Sha1_create_string(uint8_t const * const inHashPtr);

/** Print SHA1 hash given.
 *
 * - Given hash must be of expected size!
 * - No error checks (e.g., if NULL given).
 */
void Sha1_print(uint8_t const * const inHashPtr);

void Sha1_clear_cache();

/** SHA1.
 *
 * - Returns NULL on error.
 * - Given file needs to be opened for reading in binary mode (and closed later).
 * - Caller takes ownership of returned pointer.
 * - Hard-coded for little-endian systems.
 */
uint8_t* Sha1_create_from_file(FILE * const inFilePtr);

char* Sha1_create_string_from_file(FILE * const inFilePtr);

char* Sha1_create_string_from_path(char const * const inPathPtr);

#ifdef	__cplusplus
}
#endif //__cplusplus

#endif //MT_SHA1
