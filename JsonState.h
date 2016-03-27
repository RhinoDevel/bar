
// MT, 2016feb26

#ifndef MT_JSONSTATE
#define MT_JSONSTATE

#ifdef	__cplusplus
extern "C" {
#endif //__cplusplus

enum JsonState
{
    JsonState_err = -1,
    JsonState_begin = 0,
    JsonState_arr_begin = 1,
    JsonState_obj_begin = 2,
    JsonState_arr_end = 3,
    JsonState_obj_end = 4,
    JsonState_val_next = 5,
    JsonState_val_begin = 6,
    JsonState_val_end = 7,
    JsonState_boolean = 8,
    JsonState_number = 9,
    JsonState_string = 10,
    JsonState_null = 11,
    JsonState_prop_begin = 12,
    JsonState_done = 99
};

#ifdef	__cplusplus
}
#endif //__cplusplus

#endif //MT_JSONSTATE
