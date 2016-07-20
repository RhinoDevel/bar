
// MT, 2015oct27

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <inttypes.h>
#include <limits.h>

#include "Sys.h"
#include "Fnv1a.h"

static bool const _fileCacheClearAfterUsage = true; // You need to manually delete cache after usage with Fnv1a_clear_cache(), if this is set to false!
static size_t const _fileCacheSize = 16*1024*1024;

static FILE* _filePtr = NULL; // To be set by Fnv1a_create_from_file() and used in FillCache().
static char* _fileCachePtr = NULL;
static size_t _fileCachePos = SIZE_MAX;                  //
static size_t _fileCacheLen = SIZE_MAX/*_fileCachePos*/; // Important to be equal (see implementation)!

static bool FillCache()
{
    bool retVal = false;

    assert(_filePtr!=NULL);
    assert(_fileCachePtr!=NULL);

    _fileCacheLen = fread(_fileCachePtr, sizeof *_fileCachePtr, _fileCacheSize, _filePtr);
    _fileCachePos = 0;
    if(ferror(_filePtr)==0)
    {
        retVal = true;
    }
    return retVal;
}

char* Fnv1a_create_string(uint32_t const inHash)
{
    size_t const charCount = 4*2+1; // 4 byte with 2 characters each and trailing zero.
    char* retVal = malloc(charCount*(sizeof *retVal));

    assert(retVal!=NULL);
    sprintf(retVal, "%x", (unsigned int)inHash);
    retVal[charCount-1] = '\0';

    return retVal;
}

void Fnv1a_print(uint32_t const inHash)
{
    char * const str = Fnv1a_create_string(inHash);

    printf("%s", str);
    free(str);
}

void Fnv1a_clear_cache()
{
    free(_fileCachePtr);
    _fileCachePtr = NULL;
}

uint32_t* Fnv1a_create_from_file(FILE * const inFilePtr)
{
    uint32_t *retVal = NULL,
        *hashPtr = NULL;

    assert(CHAR_BIT==8);

    do
    {
        bool errOcc = false;

        assert(_filePtr==NULL);

        if(inFilePtr==NULL)
        {
            break;
        }
        _filePtr = inFilePtr;

        if(_fileCachePtr==NULL)
        {
            _fileCachePtr = malloc(_fileCacheSize*(sizeof *_fileCachePtr));
            assert(_fileCachePtr!=NULL);
        }

        assert(_fileCachePos==_fileCacheLen);
        if(!FillCache()) // (sets cache position and length)
        {
            break;
        }

        hashPtr = malloc(sizeof *hashPtr);
        assert(hashPtr!=NULL);

        *hashPtr = 2166136261u; // 32-bit offset basis.
        while(_fileCacheLen>0)
        {
            while(_fileCachePos<_fileCacheLen)
            {
                *hashPtr ^= (uint32_t)(_fileCachePtr[_fileCachePos]); // XOR
                *hashPtr *= 16777619u; // Multiply with 32-bit prime.
                ++_fileCachePos;
            }
            if(!FillCache()) // (sets cache position and length)
            {
                errOcc = true;
                break;
            }
        }
        if(errOcc)
        {
            break;
        }

        retVal = hashPtr;
        hashPtr = NULL;
    }while(false);

    free(hashPtr);
    hashPtr = NULL;
    _filePtr = NULL;
    if(_fileCacheClearAfterUsage)
    {
        Fnv1a_clear_cache();
    }
    _fileCachePos = SIZE_MAX;
    _fileCacheLen = _fileCachePos;
    return retVal;
}

char* Fnv1a_create_string_from_file(FILE * const inFilePtr)
{
    char* retVal = NULL;
    uint32_t * const hashPtr = Fnv1a_create_from_file(inFilePtr);

    if(hashPtr!=NULL)
    {
        retVal = Fnv1a_create_string(*hashPtr);
        free(hashPtr);
    }
    return retVal;
}

char* Fnv1a_create_string_from_path(char const * const inPathPtr)
{
    char* retVal = NULL;

    if(inPathPtr!=NULL)
    {
        FILE* filePtr = fopen(inPathPtr, "rb");

        if(filePtr!=NULL)
        {
            retVal = Fnv1a_create_string_from_file(filePtr);
            fclose(filePtr);
            filePtr = NULL;
        }
    }

    return retVal;
}
