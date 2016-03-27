
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
#include "macros.h"

static bool const _fileCacheClearAfterUsage = false; // You need to manually delete cache after usage wit Sha1_clear_cache(), if this is set to false!
static size_t const _fileCacheSize = 16*1024*1024;

static FILE* _filePtr = NULL; // To be set by Sha1_create_from_file() and used in FillBuf().
static char* _fileCachePtr = NULL;
static size_t _fileCachePos = SIZE_MAX;                  //
static size_t _fileCacheLen = SIZE_MAX/*_fileCachePos*/; // Important to be equal (see implementation)!
static uint8_t _loopBuf[64]; // To be set by FillBuf() multiple times and used in Sha1_create_from_file().
static bool _endOfData = false; // To be set in FillBuf() and reacted on in Sha1_create_from_file().

static size_t FillCache()
{
    bool retVal = false;

    assert(_filePtr!=NULL);
    assert(_fileCachePtr!=NULL);
    assert(_endOfData==false);
    assert(_fileCacheSize%64==0); // [so cache will never be empty during a FillBus() run]

    _fileCacheLen = fread(_fileCachePtr, sizeof *_fileCachePtr, _fileCacheSize, _filePtr);
    _fileCachePos = 0;
    if(ferror(_filePtr)==0)
    {
        retVal = true;
    }
    return retVal;
}

/** Fill 64 byte long buffer for one iteration of SHA1 creation algorithm.
*
 *  Automatically appends buffer filled with necessary data and being of dynamic size
 *  (as requested by SHA1 algorithm) AFTER end of input file is reached.
 *
 *  This way (without trying to get the file size from somewhere before starting the algorithm)
 *  there will be no problems with big files (e.g. larger than 2GB) or OS compatibility issues
 *  (those problems may arise if using fseek & ftell or fstat).
 *
 * - Returns, if an error occured.
 */
static bool FillBuf()
{
    static size_t sFileSize = 0,
        sPos = 0,
        sByteCount = 0;
    static uint8_t* sBytePtr = NULL;

    assert(_fileCachePtr!=NULL);
    assert(_endOfData==false);

    bool retVal = true; // TRUE by default.

    for(size_t bytesRead = 0;bytesRead<64;++bytesRead)
    {
        if(sBytePtr==NULL) // => Still reading bytes from cache.
        {
            if(_fileCachePos==_fileCacheLen) // => Either end of file reached (cache size must be a multiple of 64 to be sure of that) or an error occurred.
            {
                size_t congruent448ByteCount = 0;

                if(ferror(_filePtr)!=0) // => An error occurred.
                {
                    retVal = false;
                    break;
                }

                // => End of file reached.

                sByteCount = 1; // 1 for bit 1 to be appended.
                sFileSize = sPos; // Saves file size.
                sPos = 0; // Resets position to relate to buffer for setup.

                while(((sFileSize+sByteCount+congruent448ByteCount)%(512/8))!=448/8)
                {
                    ++congruent448ByteCount;
                }
                sByteCount += congruent448ByteCount; // For bits "0" to append, such that the resulting length in bits is congruent to 448 (mod 512).
                sByteCount += 64/8; // For ml to append in a 64-bit big-endian integer. Thus, the total length is a multiple of 512 bits.

                sBytePtr = (uint8_t*)malloc(sByteCount); // Creates buffer.
                if(sBytePtr==NULL)
                {
                    retVal = false;
                    break;
                }

                sBytePtr[sPos] = 0x80;
                ++sPos;

                if(congruent448ByteCount>0) // => Set extra bytes to zero.
                {
                    memset(sBytePtr+sPos, 0, congruent448ByteCount);
                    sPos += congruent448ByteCount;
                }

                // Add bit length in big endian format to buffer:
                //
                *((uint64_t*)(sBytePtr+sPos)) = MT_BSWAP64(8*(uint64_t)sFileSize); // Input file's size in bits with bytes swapped to big endianess.
                sPos += 64/8; // => sPos should be equal to bByteCount.

                assert(sPos==sByteCount);
                sPos = 0; // Resets position to relate to buffer for reading.

                assert((sFileSize+sByteCount)%(512/8)==0);

                _loopBuf[bytesRead] = sBytePtr[sPos]; // Read first byte from buffer.
            }
            else // => Byte read from cache:
            {
                _loopBuf[bytesRead] = _fileCachePtr[_fileCachePos];

                ++_fileCachePos;
            }
        }
        else // => Already reading bytes from buffer.
        {
            _loopBuf[bytesRead] = sBytePtr[sPos];
        }

        ++sPos;
    }

    if(sByteCount==sPos)
    {
        _endOfData = true;

        sFileSize = 0;
        sPos = 0;
        sByteCount = 0;
        free(sBytePtr);
        sBytePtr = NULL;
    }
    return retVal;
}

char* Sha1_create_string(uint8_t const * const inHashPtr)
{
    size_t const charCount = 5*4*2+1;
    char* retVal = malloc(charCount*(sizeof *retVal));

    assert(inHashPtr!=NULL);
    assert(retVal!=NULL);

    for(size_t i = 0;i<(5*4*2);i += 2)
    {
        size_t const hashPos = i/2;
        size_t pos = i;

        if(inHashPtr[hashPos]<0x10)
        {
            retVal[pos] = '0';
            ++pos;
        }

        sprintf(retVal+pos, "%x", (unsigned int)(inHashPtr[hashPos]));
    }
    retVal[charCount-1] = '\0';

    return retVal;
}

void Sha1_print(uint8_t const * const inHashPtr)
{
    char * const str = Sha1_create_string(inHashPtr);

    printf("%s", str);

    free(str);
}

void Sha1_clear_cache()
{
    free(_fileCachePtr);
    _fileCachePtr = NULL;
}

uint8_t* Sha1_create_from_file(FILE * const inFilePtr)
{
    uint8_t *retVal = NULL,
        *hashPtr = NULL;

    assert(CHAR_BIT==8);
    assert(!Sys_is_big_endian());
    assert(sizeof(size_t)>=64/8);

    do
    {
        bool errOcc = false;
        uint32_t h0 = 0x67452301,
            h1 = 0xEFCDAB89,
            h2 = 0x98BADCFE,
            h3 = 0x10325476,
            h4 = 0xC3D2E1F0;

        assert(_filePtr==NULL);
        assert(_endOfData==false);

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

        hashPtr = (uint8_t*)malloc(160/8);
        assert(hashPtr!=NULL);

        do
        {
            size_t i = 0;
            uint32_t w[80],
                a = h0,
                b = h1,
                c = h2,
                d = h3,
                e = h4;

            if(_fileCachePos==_fileCacheLen)
            {
                if(!FillCache()) // (sets cache position and length)
                {
                    errOcc = true;
                    break;
                }
            }

            if(!FillBuf()) // Dynamically fills _loopBuf with next 64 byte from cache OR from buffer (after EOF was reached). Sets _endOfData.
            {
                errOcc = true;
                break;
            }

            for(i = 0;i<16;++i)
            {
                w[i] = MT_BSWAP32(((uint32_t*)_loopBuf)[i]);
            }

            for(i = 16;i<80;++i)
            {
                w[i] = (w[i-3]^w[i-8]^w[i-14]^w[i-16]);
                w[i] = MT_ROL32(w[i], 1);
            }

            for(i = 0;i<80;++i)
            {
                uint32_t f = 0,
                    k = 0,
                    temp = 0;

                if(i<20)
                {
                    f = (b&c)|((~b)&d);
                    k = 0x5A827999;
                }
                else
                {
                    if(i<40)
                    {
                        f = b^c^d;
                        k = 0x6ED9EBA1;
                    }
                    else
                    {
                        if(i<60)
                        {
                            f = (b&c)|(b&d)|(c&d);
                            k = 0x8F1BBCDC;
                        }
                        else
                        {
                            f = b^c^d;
                            k = 0xCA62C1D6;
                        }
                    }
                }

                temp = MT_ROL32(a, 5)+f+e+k+w[i];
                e = d;
                d = c;
                c = MT_ROL32(b, 30);
                b = a;
                a = temp;
            }

            h0 += a;
            h1 += b;
            h2 += c;
            h3 += d;
            h4 += e;
        }while(!_endOfData);
        if(errOcc)
        {
            break;
        }

        *((uint32_t*)hashPtr) = MT_BSWAP32(h0);
        *((uint32_t*)(hashPtr+4)) = MT_BSWAP32(h1);
        *((uint32_t*)(hashPtr+8)) = MT_BSWAP32(h2);
        *((uint32_t*)(hashPtr+12)) = MT_BSWAP32(h3);
        *((uint32_t*)(hashPtr+16)) = MT_BSWAP32(h4);
        retVal = hashPtr;
        hashPtr = NULL;
    }while(false);

    _filePtr = NULL;
    _endOfData = false;
    if(_fileCacheClearAfterUsage)
    {
        Sha1_clear_cache();
    }
    _fileCachePos = SIZE_MAX;
    _fileCacheLen = _fileCachePos;
    free(hashPtr);
    return retVal;
}

char* Sha1_create_string_from_file(FILE * const inFilePtr)
{
    char* retVal = NULL;
    uint8_t * const hash = Sha1_create_from_file(inFilePtr);

    if(hash!=NULL)
    {
        retVal = Sha1_create_string(hash);
        free(hash);
    }

    return retVal;
}

char* Sha1_create_string_from_path(char const * const inPathPtr)
{
    char* retVal = NULL;

    if(inPathPtr!=NULL)
    {
        FILE* filePtr = fopen(inPathPtr, "rb");

        if(filePtr!=NULL)
        {
            retVal = Sha1_create_string_from_file(filePtr);

            fclose(filePtr);
            filePtr = NULL;
        }
    }

    return retVal;
}

/* WIKIPEDIA:

Note 1: All variables are unsigned 32-bit quantities and wrap modulo 232 when calculating, except for
        ml, the message length, which is a 64-bit quantity, and
        hh, the message digest, which is a 160-bit quantity.
Note 2: All constants in this pseudo code are in big endian.
        Within each word, the most significant byte is stored in the leftmost byte position

Initialize variables:

h0 = 0x67452301
h1 = 0xEFCDAB89
h2 = 0x98BADCFE
h3 = 0x10325476
h4 = 0xC3D2E1F0

ml = message length in bits (always a multiple of the number of bits in a character).

Pre-processing:
append the bit '1' to the message e.g. by adding 0x80 if message length is a multiple of 8 bits.
append 0 ≤ k < 512 bits '0', such that the resulting message length in bits
   is congruent to 448 (mod 512)
append ml, in a 64-bit big-endian integer. Thus, the total length is a multiple of 512 bits.

Process the message in successive 512-bit chunks:
break message into 512-bit chunks
for each chunk
    break chunk into sixteen 32-bit big-endian words w[i], 0 ≤ i ≤ 15

    Extend the sixteen 32-bit words into eighty 32-bit words:
    for i from 16 to 79
        w[i] = (w[i-3] xor w[i-8] xor w[i-14] xor w[i-16]) leftrotate 1

    Initialize hash value for this chunk:
    a = h0
    b = h1
    c = h2
    d = h3
    e = h4

    Main loop:[46]
    for i from 0 to 79
        if 0 ≤ i ≤ 19 then
            f = (b and c) or ((not b) and d)
            k = 0x5A827999
        else if 20 ≤ i ≤ 39
            f = b xor c xor d
            k = 0x6ED9EBA1
        else if 40 ≤ i ≤ 59
            f = (b and c) or (b and d) or (c and d)
            k = 0x8F1BBCDC
        else if 60 ≤ i ≤ 79
            f = b xor c xor d
            k = 0xCA62C1D6

        temp = (a leftrotate 5) + f + e + k + w[i]
        e = d
        d = c
        c = b leftrotate 30
        b = a
        a = temp

    Add this chunk's hash to result so far:
    h0 = h0 + a
    h1 = h1 + b
    h2 = h2 + c
    h3 = h3 + d
    h4 = h4 + e

Produce the final hash value (big-endian) as a 160 bit number:
hh = (h0 leftshift 128) or (h1 leftshift 96) or (h2 leftshift 64) or (h3 leftshift 32) or h4

*/
