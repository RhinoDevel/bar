
// MT, 2016feb27

#ifndef MT_STACKELE
#define MT_STACKELE

#ifdef __cplusplus
extern "C" {
#endif

struct StackEle
{
    void * const val;
    struct StackEle * const below;
};

void StackEle_delete(struct StackEle * const inStackEle, bool const inFreeVal);
struct StackEle * StackEle_create(void * const inVal, struct StackEle * const inBelow);

#ifdef __cplusplus
}
#endif

#endif // MT_STACKELE
