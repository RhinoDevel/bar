
// MT, 2015oct26

#ifndef MT_MACROS
#define MT_MACROS

#ifdef	__cplusplus
extern "C" {
#endif //__cplusplus

#define MT_BSWAP32(x) \
    ((((x)>>24)&0x000000FFu) \
    |(((x)<<8)&0x00FF0000u) \
    |(((x)>>8)&0x0000FF00u) \
    |(((x)<<24)&0xFF000000u))

#define MT_BSWAP64(x) \
    ((((x)&0xFF00000000000000ull)>>56) \
    |(((x)&0x00FF000000000000ull)>>40) \
    |(((x)&0x0000FF0000000000ull)>>24) \
    |(((x)&0x000000FF00000000ull)>>8) \
    |(((x)&0x00000000FF000000ull)<<8) \
    |(((x)&0x0000000000FF0000ull)<<24) \
    |(((x)&0x000000000000FF00ull)<<40) \
    |(((x)&0x00000000000000FFull)<<56))

#define MT_ROL32(x, b) ((((x)<<(b))|((x)>>(32-(b)))))

#ifdef	__cplusplus
}
#endif //__cplusplus

#endif //MT_MACROS
