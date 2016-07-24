
// MT, 2016feb26

// MT_TODO: TEST: Not 100% like JSON standard:
//
// - Only " signs get escaped and unescaped in strings.
// - No UTF-8 / Unicode support.
// - JSON number format might not be "perfect" (formatting of floating-point numbers, no integer support).

#ifndef MT_JSON
#define MT_JSON

#include <stdbool.h>
#include "JsonEle.h"

#ifdef	__cplusplus
extern "C" {
#endif //__cplusplus

char* json_stringify(struct JsonEle * const inEle, bool const inTakeOwnership);
struct JsonEle * json_parse(char * const inStr, bool const inTakeOwnership);

bool json_write_to_file(struct JsonEle * inEle, char const * const inPath, bool const inTakeOwnership);
struct JsonEle * json_read_from_file(char const * const inPath);

#ifdef	__cplusplus
}
#endif //__cplusplus

#endif //MT_JSON
