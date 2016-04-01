
// MT, 2015oct26

#ifndef MT_SYS
#define MT_SYS

#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif //__cplusplus

bool Sys_is_big_endian();

/** Return "time" string.
 *
 * - Caller takes ownership of return value.
 */
char* Sys_create_time_str(bool const inDate, bool const inSeconds);

#ifdef	__cplusplus
}
#endif //__cplusplus

#endif //MT_SYS
