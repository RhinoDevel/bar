
// MT, 2016feb29

//NDEBUG

#include <errno.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "JsonObj.h"
#include "JsonEle.h"
#include "JsonArr.h"
#include "JsonProp.h"
#include "json.h"
#include "Str.h"
#include "Sys.h"
#include "Deb.h"
#include "Sha1.h"
#include "FileSys.h"
#include "Stack.h"
#include "ProgressBar.h"

static int reprint_count_val = 0;
static char const * reprint_count_prefix = NULL;

static int bar_val = -1;
static int bar_max = -1;

static struct JsonEle * create_json_dir(char const * const inPath, char const * const inRootSkipFileName, void (*inAddEntryFunc)(void));

static void reprint_count()
{
    ++reprint_count_val;

    printf("\r");
    if(reprint_count_prefix!=NULL)
    {
        printf("%s", reprint_count_prefix);
    }
    printf("%d", reprint_count_val);
    fflush(stdout);
}

static void print_next_bar()
{
    ++bar_val;
    ProgressBar_print(0, bar_val, bar_max, 40, true);
}

/** Get JSON element holding entry for file at given path with given name.
 *
 *  - Caller takes ownership of return value.
 */
static struct JsonEle * create_json_file(char const * const inPath, char const * const inName)
{
    struct JsonEle * retVal = NULL;
    struct JsonObj * const obj = JsonObj_create();
    char * const fullPath = FileSys_GetFullPath(inPath, inName);

    assert(obj!=NULL);
    assert((fullPath!=NULL)&&(fullPath[0]!='\0'));

    JsonObj_add(obj, Str_copy_create("name"), JsonType_string, Str_copy_create(inName));
    JsonObj_add(obj, Str_copy_create("sha1"), JsonType_string, Sha1_create_string_from_path(fullPath));

    retVal = (struct JsonEle *)(obj->ele->val->val);
    free(fullPath);
    obj->ele->val->val = NULL;
    JsonObj_delete(obj);

    return retVal;
}

/** Get JSON element holding entry for folder at given path with given name.
 *
 *  - Caller takes ownership of return value.
 *  - Returns NULL on error.
 */
static struct JsonEle * create_json_subdir(char const * const inPath, char const * const inName, void (*inAddEntryFunc)(void))
{
    struct JsonEle * retVal = NULL,
        * subEle = NULL;
    struct JsonObj * const obj = JsonObj_create();
    char * const fullPath = FileSys_GetFullPath(inPath, inName);

    assert(obj!=NULL);
    assert((fullPath!=NULL)&&(fullPath[0]!='\0'));

    JsonObj_add(obj, Str_copy_create("name"), JsonType_string, Str_copy_create(inName));

    subEle = create_json_dir(fullPath, NULL /* Never skip any file in sub directories. */, inAddEntryFunc); // *** "RECURSION" ***
    if(subEle!=NULL)
    {
        JsonObj_add(obj, Str_copy_create("content"), JsonType_arr, subEle->val->val);
        subEle->val->val = NULL;
        JsonEle_delete(subEle);
        subEle = NULL;

        retVal = (struct JsonEle *)(obj->ele->val->val);
        obj->ele->val->val = NULL;
    }
#ifndef NDEBUG
    else
    {
        Deb_line("Failed to read content of folder \"%s\"!", inPath);
    }
#endif //NDEBUG

    free(fullPath);
    JsonObj_delete(obj);
    return retVal;
}

static struct JsonEle * create_json_dir(char const * const inPath, char const * const inRootSkipFileName, void (*inAddEntryFunc)(void))
{
    struct JsonEle * retVal = NULL;
    struct JsonArr * const arr = JsonArr_create();
    DIR* folder = opendir(inPath);
    bool errOcc = false;

    //printf("%s\n", inPath);

    //folder = opendir("C:\\I do not exist"); // => Error 2.
    //folder = opendir("C:\\System Volume Information"); // => Error 13.
    if(folder!=NULL)
    {
        struct dirent * entry = NULL;

        entry = readdir(folder);
        while(entry!=NULL)
        {
            if((strcmp(entry->d_name, ".")!=0)&&(strcmp(entry->d_name, "..")!=0))
            {
                struct JsonEle * ele = NULL;
                char* fullPath = FileSys_GetFullPath(inPath, entry->d_name);

                assert((fullPath!=NULL)&&(fullPath[0]!='\0'));

                switch(FileSys_GetEntryType(fullPath))
                {
                    case FileSys_EntryType_Dir:
                        ele = create_json_subdir(inPath, entry->d_name, inAddEntryFunc); // *** "RECURSION" ***
                        if(ele==NULL)
                        {
                            errOcc = true;
                            break;
                        }
                        if(inAddEntryFunc!=NULL)
                        {
                            inAddEntryFunc();
                        }
                        break;
                    case FileSys_EntryType_File:
                        if((inRootSkipFileName==NULL)||(strcmp(inRootSkipFileName, entry->d_name)!=0))
                        {
                            //Deb_line("\"%s\"", entry->d_name);
                            ele = create_json_file(inPath, entry->d_name);
                            assert(ele!=NULL);

                            if(inAddEntryFunc!=NULL)
                            {
                                inAddEntryFunc();
                            }
                        }
//#ifndef NDEBUG
//                        else
//                        {
//                            Deb_line("Skipping file \"%s\".", fullPath);
//                        }
//#endif //NDEBUG
                        break;

                    case FileSys_EntryType_Invalid:
                        errOcc = true;
                        break;

                    case FileSys_EntryType_Unsupported: // (falls through)
                    default:
                        Deb_line("IGNORING unsupported file system entry at \"%s\".", fullPath);
                        break; // (no error)
                }
                free(fullPath);
                fullPath = NULL;

                if(errOcc)
                {
                    break;
                }

                if(ele!=NULL)
                {
                    JsonArr_push(arr, JsonType_obj, ele);
                    ele = NULL;
                }
                //
                // Otherwise: Ignored unsupported file system entry above.
            }
            entry = readdir(folder);
        }
        if(errno!=0)
        {
            Deb_line("readdir() ERROR: %d (in folder: \"%s\")!", errno, inPath);
            errno = 0;
            errOcc = true;
        }
        closedir(folder); // (return value ignored..)
        errno = 0;
    }
    else
    {
        Deb_line("opendir() ERROR: %d!", errno);
        errno = 0;
        errOcc = true;
    }

    if(errOcc)
    {
        JsonArr_delete(arr);
    }
    else
    {
        assert(arr->ele!=NULL);
        retVal = arr->ele;
        JsonArr_init(arr, false);
        JsonArr_delete(arr);
    }
    return retVal;
}

static struct JsonEle const * get_entry_with_name_from_content_arr(struct JsonEle const * const inArr, char const * const inName, bool * const inOutErrOcc)
{
    assert(inArr!=NULL);
    assert(inArr->val!=NULL);
    assert(inArr->val->type==JsonType_arr);
    assert((inName!=NULL)&&(inName[0]!='\0'));
    assert(inOutErrOcc!=NULL);

    struct JsonEle const * retVal = NULL,
        * cur = (struct JsonEle const *)(inArr->val->val);

    *inOutErrOcc = false;

    while(cur!=NULL)
    {
        assert(cur->val!=NULL);
        assert(cur->val->type==JsonType_obj);
        assert(cur->val->val!=NULL);

        struct JsonEle const * const entry = (struct JsonEle const *)(cur->val->val),
            * nameEle = NULL;
        char const * name = NULL;

        assert(entry->val!=NULL);
        assert(entry->val->type==JsonType_prop);
        assert(entry->val->val!=NULL);

        nameEle = JsonEle_objGetPropVal(entry, "name");
        assert(nameEle!=NULL);
        assert(nameEle->val!=NULL);

        if(nameEle->val->type!=JsonType_string)
        {
            *inOutErrOcc = true;
            break;
        }

        name = (char const *)(nameEle->val->val);
        assert((name!=NULL)&&(name[0]!='\0'));

        if(strcmp(inName, name)==0)
        {
            retVal = entry;
            break; // Found
        }

        cur = cur->next;
    }

    return retVal;
}

static bool add_missing_paths_from_content_arr(
    struct JsonEle const * const inArr,
    struct JsonEle const * const inValues,
    char const * const inPath,
    struct Stack * * const inOutStack,
    struct Stack * * const inOutStackChanged)
{
    assert(inArr!=NULL);
    assert(inArr->val!=NULL);
    assert(inArr->val->type==JsonType_arr);
    assert(inValues!=NULL);
    assert(inValues->val!=NULL);
    assert(inValues->val->type==JsonType_arr);
    assert(inOutStack!=NULL);

    bool retVal = true; // TRUE by default.
    struct JsonEle const * cur = (struct JsonEle const *)(inValues->val->val); // Might be NULL.

    if(*inOutStack==NULL)
    {
        *inOutStack = Stack_create(true);
    }
    if(inOutStackChanged!=NULL)
    {
        if(*inOutStackChanged==NULL)
        {
            *inOutStackChanged = Stack_create(true);
        }
    }

    // Iterate through given values that are expected to be JSON elements holding JSON objects:
    //
    while(cur!=NULL)
    {
        assert(cur->val!=NULL);
        assert(cur->val->type==JsonType_obj);
        assert(cur->val->val!=NULL);

        struct JsonEle const * entry = NULL,
            * nameEle = NULL,
            * contentEntry = NULL,
            * subArr = NULL,
            * subValues = NULL;
        bool missing = false,
            isDir = false,
            errOcc = false;

        // Get file or folder name stored in current entry object:

        entry = (struct JsonEle const *)(cur->val->val);

        nameEle = JsonEle_objGetPropVal(entry, "name");
        assert(nameEle!=NULL);
        assert(nameEle->val!=NULL);

        if(nameEle->val->type!=JsonType_string) // Can happen, if data file was modified by someone/-thing.
        {
            retVal = false;
            break;
        }

        assert(nameEle->val->val!=NULL);
        char const * const name = (char const *)(nameEle->val->val);
        assert(name[0]!='\0');

        // Find entry object with same file (or folder) name in content array given:

        contentEntry = get_entry_with_name_from_content_arr(inArr, name, &errOcc);
        if(errOcc)
        {
            retVal = false;
            break;
        }
        if(contentEntry==NULL) // Not found.
        {
            missing = true;
        }
        else // Found
        {
            subValues = JsonEle_objGetPropVal(entry, "content");
            subArr = JsonEle_objGetPropVal(contentEntry, "content");

            isDir = subArr!=NULL;
            missing = isDir!=(subValues!=NULL); // <=> One is file, other directory.

            // If not missing, add entry to changed entries (if wanted),
            // if it is a file (not a directory) that has changed (determined by comparing checksums):
            //
            if((inOutStackChanged!=NULL)&&(!missing)&&(!isDir))
            {
                struct JsonEle const * checksum = JsonEle_objGetPropVal(entry, "sha1"),
                    * contentChecksum = JsonEle_objGetPropVal(contentEntry, "sha1");
                char const * checksumStr = NULL,
                    * contentChecksumStr = NULL;

                assert(checksum!=NULL);
                assert(checksum->val!=NULL);
                assert(checksum->val->val!=NULL);
                assert(contentChecksum!=NULL);
                assert(contentChecksum->val!=NULL);
                assert(contentChecksum->val->val!=NULL);

                if(checksum->val->type!=JsonType_string)
                {
                    retVal = false;
                    break;
                }
                if(contentChecksum->val->type!=JsonType_string)
                {
                    retVal = false;
                    break;
                }
                checksumStr = (char const *)(checksum->val->val);
                contentChecksumStr = (char const *)(contentChecksum->val->val);
                assert(strlen(checksumStr)==40);
                assert(strlen(contentChecksumStr)==40);

                if(strcmp(checksumStr, contentChecksumStr)!=0)
                {
                    if(inPath!=NULL)
                    {
                        Stack_push(*inOutStackChanged, FileSys_GetFullPath(inPath, name)); // (stack takes ownership of created string)
                    }
                    else
                    {
                        Stack_push(*inOutStackChanged, Str_copy_create(name)); // (stack takes ownership of created string)
                    }
                }
            }
        }

        if(missing) // => Add to output paths.
        {
            if(inPath!=NULL)
            {
                Stack_push(*inOutStack, FileSys_GetFullPath(inPath, name)); // (stack takes ownership of created string)
            }
            else
            {
                Stack_push(*inOutStack, Str_copy_create(name)); // (stack takes ownership of created string)
            }
        }
        else
        {
            if(isDir)
            {
                char* subPath = NULL;

                if(inPath!=NULL)
                {
                    subPath = FileSys_GetFullPath(inPath, name);
                }
                else
                {
                    subPath = Str_copy_create(name);
                }

                if(!add_missing_paths_from_content_arr(subArr, subValues, subPath, inOutStack, inOutStackChanged)) // *** RECURSION ***
                {
                    free(subPath);
                    retVal = false;
                    break;
                }
                free(subPath);
            }
        }

        cur = cur->next;
    }

    if(!retVal)
    {
        if(*inOutStack!=NULL)
        {
            Stack_delete(*inOutStack);
            *inOutStack = NULL;
        }
        if(*inOutStackChanged!=NULL)
        {
            Stack_delete(*inOutStackChanged);
            *inOutStackChanged = NULL;
        }
    }
    return retVal;
}

static void cmd_backup(char const * const inInputPath, char const * const inOutputPath, char const * * const inOutErrMsg)
{
    assert(inInputPath!=NULL);
    assert(inOutputPath!=NULL);

    char* dataFilePath = NULL;
    struct JsonEle * inputEle = NULL,
        * outputEle = NULL;
    struct Stack * toDelete = NULL, // MT_TODO: TEST: Replace stack with vector!
        * toCreate = NULL,
        * toUpdate = NULL;

    do
    {
        bool sameFile = false,
            exists = false,
            empty = false,
            errOcc = false;
        off_t size = 0;

        if(!FileSys_exists(inInputPath, &exists))
        {
            *inOutErrMsg = "Failed to get status of input folder!";
            break;
        }
        if(!exists)
        {
            *inOutErrMsg = "Input folder not found.";
            break;
        }

        if(!FileSys_exists(inOutputPath, &exists))
        {
            *inOutErrMsg = "Failed to get status of output folder!";
            break;
        }
        if(!exists) // MT_TODO: TEST: Create, if not existing?
        {
            *inOutErrMsg = "Output folder not found.";
            break;
        }

        if(!FileSys_arePathsToSameFile(inInputPath, inOutputPath, &sameFile))
        {
            *inOutErrMsg = "Failed to get status of input and/or output folder!";
            break;
        }
        if(sameFile)
        {
            *inOutErrMsg = "Same path for input and output folder given.";
            break;
        }

        if(!FileSys_isDirEmpty(inOutputPath, &empty))
        {
            *inOutErrMsg = "Failed to detect, if output folder is empty!";
            break;
        }

        dataFilePath = FileSys_GetFullPath(inOutputPath, ".pib");

        if(empty)
        {
            // Kind of hard-coded creation of data file with empty array as content:

            {
                struct JsonArr * const jsonArr = JsonArr_create();

                outputEle = jsonArr->ele;
                JsonArr_init(jsonArr, false);
                JsonArr_delete(jsonArr);
            }

//            if(!json_write_to_file(outputEle, dataFilePath, false))
//            {
//                *inOutErrMsg = "Failed to write empty data file to output folder!";
//                break;
//            }
        }
        else
        {
            if(!FileSys_exists(dataFilePath, &exists))
            {
                *inOutErrMsg = "Failed to detect, if data file exists in output folder!";
                break;
            }
            if(!exists)
            {
                // MT_TODO: TEST: Create automatically or asking user?

                *inOutErrMsg = "Output folder is not empty, but data file is missing.";
                break;
            }

            Sys_log_line(false, true, "Reading data from file \"%s\" ...", dataFilePath);
            outputEle = json_read_from_file(dataFilePath);
            if(outputEle==NULL)
            {
                *inOutErrMsg = "Failed to load data file from output folder!";
                break;
            }
        }

        reprint_count_val = 0;
        reprint_count_prefix = "Input folder content count (files & folders): ";

        int const count = FileSys_getContentCount(inInputPath, &size, reprint_count);
        //
        printf("\n");
        Sys_log_line(false, true, "%llu bytes.", (unsigned long long int)size);

        Sys_log_line(false, true, "Creating data from input folder \"%s\" ...", inInputPath);
        bar_max = count;
        bar_val = 0;
        inputEle = create_json_dir(inInputPath, ".pib", print_next_bar); // Skips all .pib files.
        printf("\n");
        if(inputEle==NULL)
        {
            *inOutErrMsg = "Failed to create data from input folder!";
            break;
        }

        if(JsonEle_areEqual(inputEle, outputEle, true))
        {
            printf("Nothing to do.\n");
            break; // Not an error.
        }

        if(!add_missing_paths_from_content_arr(inputEle, outputEle, NULL, &toDelete, NULL))
        {
            *inOutErrMsg = "Failed to get files and folders to delete!";
            break;
        }
        assert(toDelete!=NULL);

        if(!add_missing_paths_from_content_arr(outputEle, inputEle, NULL, &toCreate, &toUpdate))
        {
            *inOutErrMsg = "Failed to get files and folders to create and/or update!";
            break;
        }
        assert(toCreate!=NULL);
        assert(toUpdate!=NULL);

        // Workaround (we should use a vector..):
        //
        Stack_flip(toDelete);
        Stack_flip(toCreate);
        Stack_flip(toUpdate);
        //
        printf("\n");
        //
        if(Stack_isEmpty(toDelete))
        {
            printf("Nothing to delete.\n");
        }
        else
        {
            printf("*** TO DELETE: ***\n");
            Stack_print_str(toDelete);
        }
        printf("\n");
        //
        if(Stack_isEmpty(toCreate))
        {
            printf("Nothing to create.\n");
        }
        else
        {
            printf("*** TO CREATE: ***\n");
            Stack_print_str(toCreate);
        }
        printf("\n");
        //
        if(Stack_isEmpty(toUpdate))
        {
            printf("Nothing to update.\n");
        }
        else
        {
            printf("*** TO UPDATE: ***\n");
            Stack_print_str(toUpdate);
        }
        printf("\n");

        if(!(Stack_isEmpty(toDelete)&&Stack_isEmpty(toUpdate)))
        {
            char* answer = NULL;

            printf("Do you really want to delete and/or update above files/folders (enter \"Yes\" to proceed)? ");

            answer = Sys_get_stdin();
            if(strcmp(answer, "Yes")!=0)
            {
                printf("Stopped by your request.\n");
                free(answer);
                break; // Not an error.
            }
            free(answer);
        }

        // Delete files:
        //
        if(!Stack_isEmpty(toDelete))
        {
            Sys_log_line(false, true, "Deleting folders and files ...");
            do
            {
                char * const path = Stack_pop(toDelete), // (takes ownership)
                    * const fullPath = FileSys_GetFullPath(inOutputPath, path);

                if(!FileSys_delete(fullPath))
                {
                    *inOutErrMsg = "Failed to delete at least one file or folder from output folder!";
                    errOcc = true;
                    free(path);
                    free(fullPath);
                    break;
                }

                free(path);
                free(fullPath);
            }while(!Stack_isEmpty(toDelete));
            if(errOcc)
            {
                break;
            }
        }

        // Create files:
        //
        if(!Stack_isEmpty(toCreate))
        {
            Sys_log_line(false, true, "Creating folders and files ...");
            do
            {
                char * const path = Stack_pop(toCreate), // (takes ownership)
                    * const fullInputPath = FileSys_GetFullPath(inInputPath, path),
                    * const fullOutputPath = FileSys_GetFullPath(inOutputPath, path);

                Deb_line("\"%s\"", path);
                if(!FileSys_copy(fullInputPath, fullOutputPath))
                {
                    *inOutErrMsg = "Failed to create at least one file or folder from output folder!";
                    errOcc = true;
                    free(path);
                    free(fullInputPath);
                    free(fullOutputPath);
                    break;
                }

                free(path);
                free(fullInputPath);
                free(fullOutputPath);
            }while(!Stack_isEmpty(toCreate));
            if(errOcc)
            {
                break;
            }
        }

        // Update files:
        //
        if(!Stack_isEmpty(toUpdate))
        {
            Sys_log_line(false, true, "Updating folders and files ...");
            do
            {
                char * const path = Stack_pop(toUpdate), // (takes ownership)
                    * const fullInputPath = FileSys_GetFullPath(inInputPath, path),
                    * const fullOutputPath = FileSys_GetFullPath(inOutputPath, path);

                Deb_line("\"%s\"", path);
                if(!FileSys_copy(fullInputPath, fullOutputPath))
                {
                    *inOutErrMsg = "Failed to update at least one file or folder from output folder!";
                    errOcc = true;
                    free(path);
                    free(fullInputPath);
                    free(fullOutputPath);
                    break;
                }

                free(path);
                free(fullInputPath);
                free(fullOutputPath);
            }while(!Stack_isEmpty(toUpdate));
            if(errOcc)
            {
                break;
            }
        }

        if(!json_write_to_file(inputEle, dataFilePath, false)) // It is correct to use INPUT element, here.
        {
            *inOutErrMsg = "Failed to write data file to output folder!";
            break;
        }
    }while(false);

    if(toDelete!=NULL)
    {
        Stack_delete(toDelete);
    }
    if(toCreate!=NULL)
    {
        Stack_delete(toCreate);
    }
    if(toUpdate!=NULL)
    {
        Stack_delete(toUpdate);
    }
    free(dataFilePath);
    if(inputEle!=NULL)
    {
        JsonEle_delete(inputEle);
    }
    if(outputEle!=NULL)
    {
        JsonEle_delete(outputEle);
    }
}

static void cmd_verify(char const * const inPath, char const * * const inOutErrMsg)
{
    assert(inPath!=NULL);

    char* fullPath = FileSys_GetFullPath(inPath, ".pib");

    Sys_log_line(false, true, "Reading data from file \"%s\" ...", fullPath);
    struct JsonEle * loadedEle = json_read_from_file(fullPath);

    free(fullPath);
    fullPath = NULL;

    if(loadedEle!=NULL)
    {
        off_t size = 0;
        reprint_count_val = 0;
        reprint_count_prefix = "Content count (files & folders): ";
        int const count = FileSys_getContentCount(inPath, &size, reprint_count);
        //
        printf("\n");
        Sys_log_line(false, true, "%llu bytes.", (unsigned long long int)size);

        Sys_log_line(false, true, "Creating data from folder \"%s\" ...", inPath);
        bar_max = count;
        bar_val = 0;
        struct JsonEle * ele = create_json_dir(inPath, ".pib", print_next_bar); // Skips all .pib files.
        printf("\n");

        if(ele!=NULL)
        {
            Sys_log_line(false, true, "Comparing data from file and folder ...");
            if(!JsonEle_areEqual(loadedEle, ele, true))
            {
                *inOutErrMsg = "Folder contents are different from saved data!";
            }

            JsonEle_delete(ele);
            ele = NULL;
        }
        else
        {
            *inOutErrMsg = "Failed to create data from folder!";
        }

        JsonEle_delete(loadedEle);
        loadedEle = NULL;
    }
    else
    {
        *inOutErrMsg = "Failed to load data from file!";
    }
}

static void cmd_create(char const * const inPath, char const * * const inOutErrMsg)
{
    off_t size = 0;

    assert(inPath!=NULL);

    reprint_count_val = 0;
    reprint_count_prefix = "Content count (files & folders): ";
    int const count = FileSys_getContentCount(inPath, &size, reprint_count);
    //
    printf("\n");
    Sys_log_line(false, true, "%llu bytes.", (unsigned long long int)size);

    if(count>=0)
    {
        Sys_log_line(false, true, "Creating data from folder \"%s\" ...", inPath);
        bar_max = count;
        bar_val = 0;
        struct JsonEle* ele = create_json_dir(inPath, ".pib", print_next_bar);
        printf("\n");

        if(ele!=NULL)
        {
            char * const fullPath = FileSys_GetFullPath(inPath, ".pib");

            Sys_log_line(false, true, "Writing data to file \"%s\" ...", fullPath);
            if(!json_write_to_file(ele, fullPath, true))
            {
                *inOutErrMsg = "Failed to write to file!";
            }

            free(fullPath);
        }
        else
        {
            *inOutErrMsg = "Failed to create data!";
        }
    }
    else
    {
        *inOutErrMsg = "Failed to count files and folders!";
    }


}

int main(int argc, char* argv[])
{
    int retVal = EXIT_FAILURE;
    char const * errMsg = NULL;

    // *** PARSED STUFF ***

    char cmd = 0;
    char const * inputDirPath = NULL,
        * outputDirPath = NULL;

    // ********************

    do
    {
        if(argc<(1+2))
        {
            errMsg = "Please enter at least two parameters like: pib <command> <path>";
            break;
        }
        if(strlen(argv[1])!=1)
        {
            errMsg = "Please enter a supported command (all commands have just one letter).";
            break;
        }
        cmd = argv[1][0];
        switch(cmd)
        {
            case 'b': // Backup (from given input to given output folder with automatic ".pib" file creation, if not existing).
                if(argc!=(1+3))
                {
                    errMsg = "Please enter backup command and path to input and output folder like: pib v <input path> <output path>";
                    break;
                }
                inputDirPath = argv[2];
                outputDirPath = argv[3];
                cmd_backup(inputDirPath, outputDirPath, &errMsg);
                if(errMsg!=NULL)
                {
                    break;
                }
                Sys_log_line(false, true, "Successfully backed up.");
                break;

            case 'v': // Verify (that ".pib" file exists in given folder and that its content is correctly describing the folder's content).
                if(argc!=(1+2))
                {
                    errMsg = "Please enter verify command and path to folder like: pib v <path>";
                    break;
                }
                outputDirPath = argv[2];
                cmd_verify(outputDirPath, &errMsg);
                if(errMsg!=NULL)
                {
                    break;
                }
                Sys_log_line(false, true, "Successfully verified.");
                break;

            case 'c': // Create (a ".pib" file inside given folder that describes its content).
                if(argc!=(1+2))
                {
                    errMsg = "Please enter create command and path to folder like: pib c <path>";
                    break;
                }
                outputDirPath = argv[2];
                cmd_create(outputDirPath, &errMsg);
                if(errMsg!=NULL)
                {
                    break;
                }
                Sys_log_line(false, true, "Successfully created.");
                break;

            case 'i': // Gather and print information about folder at path given.
            {
                off_t size = 0;

                if(argc!=(1+2))
                {
                    errMsg = "Please enter info command and path to folder like: pib i <path>";
                    break;
                }
                inputDirPath = argv[2];

                reprint_count_val = 0;
                reprint_count_prefix = "Content count (files & folders): ";
                int const count = FileSys_getContentCount(inputDirPath, &size, reprint_count);
                //
                printf("\n");

                if(count<0)
                {
                    Sys_log_line(false, true, "Failed to count content of \"%s\"!", inputDirPath);
                    break;
                }
                Sys_log_line(false, true, "%llu bytes.", (unsigned long long int)size);
                break;
            }

            default:
                errMsg = "Please enter a supported command.";
                break;
        }
        if(errMsg!=NULL)
        {
            break;
        }

        retVal = EXIT_SUCCESS;
    }while(false);

    if(retVal!=EXIT_SUCCESS)
    {
        assert(errMsg!=NULL);
        printf("%s\n", errMsg);
    }
    return retVal;
}
