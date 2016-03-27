
// MT, 2016mar06

#ifndef MT_JSONTYPE
#define MT_JSONTYPE

#ifdef	__cplusplus
extern "C" {
#endif //__cplusplus

enum JsonType
{
    JsonType_arr = 1,
    JsonType_obj = 2,
    JsonType_boolean = 8,
    JsonType_number = 9,
    JsonType_string = 10,
    JsonType_null = 11,
    JsonType_prop = 12
};

#ifdef	__cplusplus
}
#endif //__cplusplus

#endif //MT_JSONTYPE
