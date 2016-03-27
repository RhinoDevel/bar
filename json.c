
// MT, 2016feb26

#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "FileSys.h"
#include "Str.h"
#include "JsonState.h"
#include "JsonStateInput.h"
#include "JsonProp.h"
#include "json_state_begin.h"
#include "json_state_arr_begin.h"
#include "json_state_arr_end.h"
#include "json_state_val_next.h"
#include "json_state_val_begin.h"
#include "json_state_val_end.h"
#include "json_state_boolean.h"
#include "json_state_null.h"
#include "json_state_number.h"
#include "json_state_string.h"
#include "json_state_obj_begin.h"
#include "json_state_obj_end.h"
#include "json_state_prop_begin.h"
#include "json.h"

/** Takes ownership of given string.
 */
static char* stringify(struct JsonEle * const inEle, char * const inStr)
{
    char *retVal = inStr,
        *buf = NULL;
    struct JsonEle * cur = inEle;

    assert(inEle!=NULL);

    if(inStr==NULL)
    {
        retVal = malloc(sizeof(*retVal));
        assert(retVal!=NULL);
        *retVal = '\0';
    }

    do
    {
        assert(cur->val!=NULL);

        switch(cur->val->type)
        {
            case JsonType_null:
                assert(strlen(retVal)>0);
                buf = retVal;
                retVal = Str_concat_create(buf, "null");
                free(buf);
                buf = NULL;
                break;

            case JsonType_boolean:
                assert(strlen(retVal)>0);
                buf = retVal;
                retVal = Str_concat_create(buf, (*((bool*)(cur->val->val)))?"true":"false");
                free(buf);
                buf = NULL;
                break;

            case JsonType_string:
                assert(strlen(retVal)>0);

                buf = retVal;
                retVal = Str_concat_create(buf, "\"");
                free(buf);
                buf = NULL;

                buf = retVal;
                retVal = Str_concat_create(buf, (char*)(cur->val->val));
                free(buf);
                buf = NULL;

                buf = retVal;
                retVal = Str_concat_create(buf, "\"");
                free(buf);
                buf = NULL;
                break;

            case JsonType_number:
            {
                assert(strlen(retVal)>0);
                
                char * const dblStr = Str_from_double_create(*((double*)(cur->val->val)));
                
                buf = retVal;
                retVal = Str_concat_create(buf, dblStr);
                free(dblStr);
                free(buf);
                buf = NULL;
                break;
            }

            case JsonType_arr:
                buf = retVal;
                retVal = Str_concat_create(buf, "[");
                free(buf);
                buf = NULL;

                if(cur->val->val!=NULL)
                {
                    retVal = stringify((struct JsonEle *)(cur->val->val), retVal);
                }

                buf = retVal;
                retVal = Str_concat_create(buf, "]");
                free(buf);
                buf = NULL;
                break;

            case JsonType_obj:
                buf = retVal;
                retVal = Str_concat_create(buf, "{");
                free(buf);
                buf = NULL;

                if(cur->val->val!=NULL)
                {
                    retVal = stringify((struct JsonEle *)(cur->val->val), retVal);
                }

                buf = retVal;
                retVal = Str_concat_create(buf, "}");
                free(buf);
                buf = NULL;
                break;

            case JsonType_prop:
            {
                struct JsonProp * const jsonProp = (struct JsonProp *)(cur->val->val);

                assert(strlen(retVal)>0);

                buf = retVal;
                retVal = Str_concat_create(buf, "\"");
                free(buf);
                buf = NULL;

                buf = retVal;
                retVal = Str_concat_create(buf, jsonProp->name);
                free(buf);
                buf = NULL;

                buf = retVal;
                retVal = Str_concat_create(buf, "\"");
                free(buf);
                buf = NULL;

                buf = retVal;
                retVal = Str_concat_create(buf, ":");
                free(buf);
                buf = NULL;

                retVal = stringify(jsonProp->ele, retVal);
                break;
            }

            default:
                assert(false);
                break;
        }

        cur = cur->next;
        if(cur!=NULL)
        {
            buf = retVal;
            retVal = Str_concat_create(buf, ",");
            free(buf);
            buf = NULL;
        }
    }while(cur!=NULL);

    return retVal;
}

char* json_stringify(struct JsonEle * const inEle, bool const inTakeOwnership)
{
    char* retVal = stringify(inEle, NULL);

    if(inTakeOwnership)
    {
        JsonEle_delete(inEle);
    }
    return retVal;
}

struct JsonEle * json_parse(char * const inStr, bool const inTakeOwnership)
{
    struct JsonEle * retVal = NULL;
    bool loop = true;
    struct JsonStateInput * const obj = JsonStateInput_create(inStr, inTakeOwnership);
    enum JsonState state = JsonState_begin;

    do
    {
        switch(state)
        {
            case JsonState_begin:
                state = json_state_begin(obj);
                break;

            case JsonState_arr_begin:
                state = json_state_arr_begin(obj);
                break;

            case JsonState_arr_end:
                state = json_state_arr_end(obj);
                break;

            case JsonState_val_next:
                state = json_state_val_next(obj);
                break;

            case JsonState_val_begin:
                state = json_state_val_begin(obj);
                break;

            case JsonState_val_end:
                state = json_state_val_end(obj);
                break;

            case JsonState_boolean:
                state = json_state_boolean(obj);
                break;

            case JsonState_null:
                state = json_state_null(obj);
                break;

            case JsonState_number:
                state = json_state_number(obj);
                break;

            case JsonState_string:
                state = json_state_string(obj);
                break;

            case JsonState_obj_begin:
                state = json_state_obj_begin(obj);
                break;

            case JsonState_obj_end:
                state = json_state_obj_end(obj);
                break;

            case JsonState_prop_begin:
                state = json_state_prop_begin(obj);
                break;

            case JsonState_err:
            default:
                loop = false;
                break;

            case JsonState_done:
                loop = false;
                retVal = obj->root;
                obj->root = NULL;
                break;
        }
    }while(loop);

    JsonStateInput_delete(obj);
    return retVal;
}

struct JsonEle * json_read_from_file(char const * const inPath)
{
    struct JsonEle * retVal = NULL;

    if(inPath!=NULL)
    {
        off_t const size = FileSys_GetFileSize(inPath);

        if(size>/*=*/0)
        {
            FILE* f = fopen(inPath, "rb"); // Opening in BINARY mode.

            if(f!=NULL)
            {
                size_t const len = (size_t)size;
                char * const str = malloc((len+1)*(sizeof *str));

                if(str!=NULL)
                {
                    if(fread(str, sizeof *str, len, f)==len)
                    {
                        fclose(f);

                        str[len] = '\0';

                        retVal = json_parse(str, true);
                    }
                }

                if(f!=NULL)
                {
                    fclose(f); // Return value ignored.
                }
            }
        }
    }
    return retVal;
}

bool json_write_to_file(struct JsonEle * inEle, char const * const inPath, bool const inTakeOwnership)
{
    bool retVal = false;
    FILE* f = NULL;
    char* str = NULL;

    do
    {
        size_t strLen = 0;

        if(inEle==NULL)
        {
            break;
        }
        if(inPath==NULL)
        {
            break;
        }

        str = json_stringify(inEle, inTakeOwnership);
        inEle = NULL;
        if(str==NULL)
        {
            break;
        }

        strLen = strlen(str);

        f = fopen(inPath, "wb"); // Opening in BINARY mode.
        if(f==NULL)
        {
            break;
        }

        if(fwrite(str, sizeof *str, strLen, f)!=strLen)
        {
            break;
        }

        retVal = true;
    }while(false);

    if(f!=NULL)
    {
        fclose(f); // Return value ignored.
    }
    free(str);
    return retVal;
}
