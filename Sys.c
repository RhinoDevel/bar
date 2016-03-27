
// MT, 2015oct26

#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include "Sys.h"

bool Sys_is_big_endian()
{
    assert(sizeof(unsigned int)==4);

    union
    {
        unsigned int uI;
        unsigned char uC[4];
    } u = { 0x01020304 };

    return u.uC[0]==1;
}

char* Sys_get_time_str(bool const inDate, bool const inSeconds)
{
    char* retVal = NULL;
    time_t t = time(NULL);
    struct tm const * const l = localtime(&t);
    size_t const dateLen = 11,
        timeLen = 5;
    size_t len = timeLen+1, // 23:59 and '\0'
        pos = 0;

    if(inDate)
    {
        len += dateLen; // 2016/03/20-
    }
    if(inSeconds)
    {
        len += 3; // :59
    }

    size_t const byteLen = len*(sizeof *retVal);

    retVal = malloc(byteLen);
    assert(retVal!=NULL);

    if(inDate)
    {
        snprintf(retVal+pos, byteLen-pos, "%d/%02d/%02d-", l->tm_year+1900, l->tm_mon+1, l->tm_mday);

        pos += dateLen;
    }

    snprintf(retVal+pos, byteLen-pos, "%02d:%02d", l->tm_hour, l->tm_min);
    pos += timeLen;

    if(inSeconds)
    {
        snprintf(retVal+pos, byteLen-pos, ":%02d", l->tm_sec);
    }

    return retVal;
}

/** Original source: http://stackoverflow.com/questions/7831755/what-is-the-simplest-way-of-getting-user-input-in-c
 */
char* Sys_get_stdin()
{
    size_t const granularity = 20;
    size_t len = granularity;
    char* retVal = malloc(len*(sizeof *retVal));
    int i = 0;
    assert(retVal!=NULL);

    while(true)
    {
        int const c = getchar();

        // At terminating zero at end of string:
        //
        if(c=='\n')
        {
            retVal[i] = '\0';
            break;
        }

        retVal[i] = c;

        // Expand buffer, if full:
        //
        if(i==(len-1))
        {
            char* buf = retVal;
            size_t const bufLen = len;

            len = len+granularity;
            retVal = malloc(len*(sizeof *retVal));
            assert(retVal!=NULL);

            strncpy(retVal, buf, bufLen);
            free(buf);
        }

        ++i;
    }

    return retVal;
}
