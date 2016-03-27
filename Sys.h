
// MT, 2015oct26

#ifndef MT_SYS
#define MT_SYS

#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif //__cplusplus

bool Sys_is_big_endian();

char* Sys_get_time_str(bool const inDate, bool const inSeconds);

char* Sys_get_stdin();

#ifdef	__cplusplus
}
#endif //__cplusplus

#endif //MT_SYS
