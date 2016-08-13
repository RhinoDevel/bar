
// MT, 2016mar01

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "Obj.h"
#include "Str.h"

char* Str_from_long_create(long const inVal)
{
    char* retVal = NULL;
    char const * const formatStr = "%ld";
    char dummy[] = { '\0' };
    int const strLen = snprintf(dummy, sizeof(dummy)*sizeof(char), formatStr, inVal); // Without terminating '\0'.

    if(strLen>0)
    {
        retVal = malloc((((size_t)strLen)+1)*(sizeof *retVal));
        assert(retVal!=NULL);

        if(sprintf(retVal, formatStr, inVal)!=strLen)
        {
            free(retVal);
            retVal = NULL;
        }
    }

    return retVal;
}

char* Str_from_double_create(double const inVal)
{
    char* retVal = NULL;
    char const * const formatStr = "%f"; // Add some formatting?
    char dummy[] = { '\0' };
    int const strLen = snprintf(dummy, sizeof(dummy)*sizeof(char), formatStr, inVal); // Without terminating '\0'.

    if(strLen>0)
    {
        retVal = malloc((((size_t)strLen)+1)*(sizeof *retVal));
        assert(retVal!=NULL);

        if(sprintf(retVal, formatStr, inVal)!=strLen)
        {
            free(retVal);
            retVal = NULL;
        }
    }

    return retVal;
}

char* Str_append_create(char * const inStrA, char const * const inStrB)
{
    char* retVal = NULL;

    assert((inStrA!=NULL)&&(inStrB!=NULL));
    if((inStrA!=NULL)&&(inStrB!=NULL))
    {
        size_t const aLen = strlen(inStrA),
            bLen = strlen(inStrB),
            valLen = aLen+bLen+1;

        retVal = realloc(inStrA, valLen*(sizeof *retVal));
        assert(retVal!=NULL);

        if(valLen==1)
        {
            retVal[0] = '\0';
        }
        else
        {
            size_t pos = aLen;

            strncpy(retVal+pos, inStrB, bLen);
            pos += bLen;
            retVal[pos] = '\0';
            assert(pos==(valLen-1));
        }
    }

    return retVal;
}

char* Str_concat_create(char const * const inStrA, char const * const inStrB)
{
    char* retVal = NULL;

    assert((inStrA!=NULL)&&(inStrB!=NULL));
    if((inStrA!=NULL)&&(inStrB!=NULL))
    {
        size_t const aLen = strlen(inStrA),
            bLen = strlen(inStrB),
            valLen = aLen+bLen+1;

        retVal = malloc(valLen*(sizeof *retVal));
        assert(retVal!=NULL);

        if(valLen==1)
        {
            retVal[0] = '\0';
        }
        else
        {
            size_t pos = 0;

            strncpy(retVal+pos, inStrA, aLen);
            pos += aLen;
            strncpy(retVal+pos, inStrB, bLen);
            pos += bLen;
            retVal[pos] = '\0';
            assert(pos==(valLen-1));
        }
    }

    return retVal;
}

char* Str_copy_create(char const * const inStr)
{
    char* retVal = NULL;

    assert(inStr!=NULL);

    if(inStr!=NULL)
    {
        size_t const strLen = strlen(inStr)+1;

        retVal = malloc(strLen*(sizeof *retVal));
        assert(retVal!=NULL);
        strncpy(retVal, inStr, strLen);
    }

    return retVal;
}

double* Str_double_create(char const * const inStr, size_t const inLen, size_t * const inOutIndex)
{
    double *retVal = NULL,
        buf = 0.0;
    char * endPtr = NULL;

    assert(inStr!=NULL);
    assert(inOutIndex!=NULL);

    if((*inOutIndex)<inLen)
    {
        buf = strtod(inStr+(*inOutIndex), &endPtr);
    }

    size_t const consumeLen = (size_t)(endPtr-(inStr+(*inOutIndex)));

    if(consumeLen>0)
    {
        retVal = Obj_double_create(buf);
        *inOutIndex = (*inOutIndex)+consumeLen;
    }

    return retVal;
}

char* Str_string_create(char const * const inStr, size_t const inLen, char const inTag, size_t * const inOutIndex)
{
    char *retVal = NULL,
        *buf = NULL;

    do
    {
        size_t terminatorPos = 0,
            escapedCount = 0,
            bufPos = 0;

        if(((*inOutIndex)+2)>=inLen)
        {
            break;
        }
        if(inStr[*inOutIndex]!=inTag)
        {
            break;
        }

        ++(*inOutIndex);

        size_t const firstCharPos = *inOutIndex;

        while((*inOutIndex)<inLen)
        {
            if(inStr[*inOutIndex]==inTag)
            {
                if(inStr[(*inOutIndex)-1]!='\\')
                {
                    terminatorPos = *inOutIndex;
                    ++(*inOutIndex); // (consumes closing ")
                    break;
                }
                ++escapedCount;
            }
            ++(*inOutIndex);
        }
        if(terminatorPos==0)
        {
            break;
        }

        size_t const len = terminatorPos-firstCharPos, // (without terminator)
            bufLen = len-escapedCount+1,
            lastBufPos = bufLen-1;

        buf = malloc(bufLen*(sizeof *buf));
        assert(buf!=NULL);

        size_t const firstCharPosPlusLen = firstCharPos+len,
            lastCharPos = firstCharPosPlusLen-1;

        for(size_t i = firstCharPos;i<firstCharPosPlusLen;++i)
        {
            if(!((inStr[i]=='\\')&&(i<lastCharPos)&&(inStr[i+1]==inTag)))
            {
                buf[bufPos++] = inStr[i];
            }
        }
        assert(bufPos==lastBufPos);
        buf[lastBufPos] = '\0';

        retVal = buf;
        buf = NULL;
    }while(false);

    free(buf);
    return retVal;
}
