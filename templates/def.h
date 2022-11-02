#ifndef DEF_H
#define DEF_H

#ifndef __cplusplus
    #ifndef true
        #define true 1
        #define false 0
        typedef unsigned char bool;
    #endif
#endif

#ifndef NULL
    #define NULL 0
#endif

#if defined(__GCC__) || defined(__APPLE_CC__)
    #define ATTRIBUTE_PACKED __attribute__ ((__packed__))
//#elif 
    //#define ATTRIBUTE_PACKED __attribute__ ((__packed__))
#else
    #define ATTRIBUTE_PACKED 
#endif

typedef signed char int8; // -128 - 127 // 1 byte <-> 8 bit
typedef signed short int16; // -32 768 - 32 767 // 2 byte <-> 16 bit
typedef signed int int32; // -2 147 483 648 - 2 147 483 647 // 4 byte <-> 32 bit
typedef signed long int64; // -9 223 372 036 854 775 808 - 9 223 372 036 854 775 807 // 8 byte <-> 64 bit

typedef unsigned char index8; // 0 - 255
typedef unsigned short index16; // 0 - 65 535
typedef unsigned int index32; // 0 - 4 294 967 296
typedef unsigned long index64; // 

#endif