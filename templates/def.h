#ifndef DEF_H
#define DEF_H

#ifndef __cplusplus
    #ifndef true
        #define true 1
        #define false 0
        typedef unsigned char bool;
    #endif
    /** C versions
     * note: use 'c89', 'c90', or 'iso9899:1990' for 'ISO C 1990' standard
     * note: use 'iso9899:199409' for 'ISO C 1990 with amendment 1' standard
     * note: use 'gnu89' or 'gnu90' for 'ISO C 1990 with GNU extensions' standard
     * note: use 'c99' or 'iso9899:1999' for 'ISO C 1999' standard
     * note: use 'gnu99' for 'ISO C 1999 with GNU extensions' standard
     * note: use 'c11' or 'iso9899:2011' for 'ISO C 2011' standard
     * note: use 'gnu11' for 'ISO C 2011 with GNU extensions' standard
     * note: use 'c17', 'iso9899:2017', 'c18', or 'iso9899:2018' for 'ISO C 2017' standard
     * note: use 'gnu17' or 'gnu18' for 'ISO C 2017 with GNU extensions' standard
     * note: use 'c2x' for 'Working Draft for ISO C2x' standard
     * note: use 'gnu2x' for 'Working Draft for ISO C2x with GNU extensions' standard
    **/
    #define LANG_C 1
    #if defined(__STDC_VERSION__)
        #if __STDC_VERSION__ == 199901L
            // c99
            #define LANG_C_STD 99
        #elif __STDC_VERSION__ == 201112L
            // c11
            #define LANG_C_STD 11
        #elif __STDC_VERSION__ == 201112L
            // c11
            #define LANG_C_STD 11
        #elif __STDC_VERSION__ == 201710L
            // c17 or c18
            #define LANG_C_STD 17
        #else
            // unknown
            #define LANG_C_STD 0
        #endif
    #else
        // c89 or c90
        #define LANG_C_STD 90
    #endif
    #define LANG_CPP 0
    #define LANG_CPP_STD 0
#else
    /** C++ versions
     * note: use 'c++98' or 'c++03' for 'ISO C++ 1998 with amendments' standard
     * note: use 'gnu++98' or 'gnu++03' for 'ISO C++ 1998 with amendments and GNU extensions' standard
     * note: use 'c++11' for 'ISO C++ 2011 with amendments' standard
     * note: use 'gnu++11' for 'ISO C++ 2011 with amendments and GNU extensions' standard
     * note: use 'c++14' for 'ISO C++ 2014 with amendments' standard
     * note: use 'gnu++14' for 'ISO C++ 2014 with amendments and GNU extensions' standard
     * note: use 'c++17' for 'ISO C++ 2017 with amendments' standard
     * note: use 'gnu++17' for 'ISO C++ 2017 with amendments and GNU extensions' standard
     * note: use 'c++20' for 'ISO C++ 2020 DIS' standard
     * note: use 'gnu++20' for 'ISO C++ 2020 DIS with GNU extensions' standard
     * note: use 'c++2b' for 'Working draft for ISO C++ 2023 DIS' standard
     * note: use 'gnu++2b' for 'Working draft for ISO C++ 2023 DIS with GNU extensions' standard
     */
    #define LANG_CPP 1
    #if __cplusplus == 1
        // older than c++98
        #define LANG_CPP_STD -98
    #elif __STDC_VERSION__ == 199711L
        // c++98 or c++03
        #define LANG_CPP_STD 98
    #elif __STDC_VERSION__ == 201103L
        // c++11
        #define LANG_CPP_STD 11
    #elif __STDC_VERSION__ == 201402L
        // c++14
        #define LANG_CPP_STD 14
    #elif __STDC_VERSION__ == 201703L
        // c++17
        #define LANG_CPP_STD 17
    #elif __STDC_VERSION__ == 202002L
        // c++20
        #define LANG_CPP_STD 20
    #else
        // unknown
        #define LANG_CPP_STD 0
    #endif
    #define LANG_C 0
    #define LANG_C_STD 0
#endif

#ifndef NULL
    #define NULL ((void *)0)
#endif

#if defined(WINDOWS) || defined(WIN32) || defined(_WIN32)
    #define PF_WINDOWS 1
#else 
    #define PF_WINDOWS 0
#endif

#if defined(__GCC__) || defined(__APPLE_CC__)
    #define CC_GCC 1
    #define ATTRIBUTE_PACKED __attribute__ ((__packed__))
    #define ATTRIBUTE_DEPRECATED __attribute__((__deprecated__))
//#elif 
    //#define ATTRIBUTE_PACKED __attribute__ ((__packed__))
#else
    #define CC_GCC 0
    #define ATTRIBUTE_PACKED
    #define ATTRIBUTE_DEPRECATED
#endif

typedef signed char int8; // -128 - 127 // 1 byte <-> 8 bit
typedef signed short int16; // -32 768 - 32 767 // 2 byte <-> 16 bit
typedef signed int int32; // -2 147 483 648 - 2 147 483 647 // 4 byte <-> 32 bit
typedef signed long long int64; // -9 223 372 036 854 775 808 - 9 223 372 036 854 775 807 // 8 byte <-> 64 bit

typedef unsigned char index8; // 0 - 255
typedef unsigned short index16; // 0 - 65 535
typedef unsigned int index32; // 0 - 4 294 967 296
typedef unsigned long long index64; // 

#endif