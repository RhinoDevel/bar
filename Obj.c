
// MT, 2016feb28

#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include "Obj.h"

bool* Obj_bool_create(bool const inVal)
{
    bool * const retVal = malloc(sizeof *retVal);

    assert(retVal!=NULL);

    *retVal = inVal;

    return retVal;
}

char* Obj_char_create(char const inVal)
{
    char * const retVal = malloc(sizeof *retVal);

    assert(retVal!=NULL);

    *retVal = inVal;

    return retVal;
}

int* Obj_int_create(int const inVal)
{
    int * const retVal = malloc(sizeof *retVal);

    assert(retVal!=NULL);

    *retVal = inVal;

    return retVal;
}

double* Obj_double_create(double const inVal)
{
    double * const retVal = malloc(sizeof *retVal);

    assert(retVal!=NULL);

    *retVal = inVal;

    return retVal;
}
