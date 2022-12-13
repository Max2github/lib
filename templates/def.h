#ifndef DEF_H
#define DEF_H

#if !defined(__cplusplus) || !__cplusplus
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

/**
 * stringify anything at compile-time
 * does not resolve macros
 */
#define STR_IMPL(x) #x 
/**
 * stringify anything at compile-time
 * resolves macros
 */
#define STR(x) STR_IMPL(x)

/**
 * detect OS
 */

/**
 * detect UNIX and POSIX
 */

#if !defined(_WIN32) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
	// UNIX-style OS
    #define OS_UNIX 1
    #ifndef NO_STD_LIB
        #include <unistd.h>
    #endif

    #if defined(_POSIX_VERSION)
        #define OS_POSIX 1
    #else
        #define OS_POSIX 0
    #endif
#else
    #define OS_UNIX 0
#endif

// POSIX Windows
#if defined(__CYGWIN__)
    #define OS_POSIX 1
// if OS_POSIX was still not defined, define it as 0
#elif !defined(OS_POSIX)
    #define OS_POSIX 0
#endif

#if OS_POSIX
    #define LIB_PTHREAD 1
#elif defined(__MINGW32__) || defined(__MINGW64__)
    #define LIB_PTHREAD 1

    // #undef _WIN32_WINNT
    // #define _WIN32_WINNT 0x0600
#else
    #define LIB_PTHREAD 0
#endif

/**
 * detect WINDOWS
 */

#if defined(WINDOWS) || defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
    #define OS_WINDOWS 1
    #if defined(_WIN64)
        // 64 bit OS (CPU may be different?)
    #elif defined(_WIN32)
        // 32 bit OS (CPU may be different?)
    #endif
#else 
    #define OS_WINDOWS 0
#endif

/**
 * detect LINUX
 */

#if !OS_WINDOWS && (defined(__linux__))
    #define OS_LINUX 1
#else
    #define OS_LINUX 0
#endif

/**
 * detect APPLE
 */ 

#define OS_APPLE_IOS_SIMULATOR 0
#define OS_APPLE_IOS 0
#define OS_APPLE_MAC 0

#if !OS_WINDOWS && (defined(__APPLE__) || defined(__MACH__))
    #define OS_APPLE 1
    #ifndef NO_STD_LIB
        #include <TargetConditionals.h>
        #if TARGET_IPHONE_SIMULATOR == 1
            // iOS in XCode simulator 
            #undef OS_APPLE_IOS_SIMULATOR
            #define OS_APPLE_IOS_SIMULATOR 1
        #elif TARGET_OS_IPHONE == 1
            #undef OS_APPLE_IOS
            #define OS_APPLE_IOS 1
        #elif TARGET_OS_MAC == 1
            #undef OS_APPLE_MAC
            #define OS_APPLE_MAC 1
        #endif
    #endif
#else
    #define OS_APPLE 0
#endif

/**
 * detect CPU ARCH
 */

#if defined(__x86_64__) || defined(__x86_64) || defined(_M_X64)
    #define CPU_x86 1
    #define CPU_ARM 0
    #define CPU_32_BIT 0
    #define CPU_64_BIT 1
    #define SIZE_POINTER 64

    // in case we are on X32 linux: basically x86_64, but pointers are 32 Bit
    // also look at https://stackoverflow.com/questions/7635013/difference-between-x86-x32-and-x64-architectures
    #if defined(__ILP32__)
        #undef SIZE_POINTER
        #define SIZE_POINTER 32
    #endif
#elif defined(__i386) || defined(__i386__) || defined(_M_IX86) || defined(_x86_)
    #define CPU_x86 1
    #define CPU_ARM 0
    #define CPU_32_BIT 1
    #define CPU_64_BIT 0
    #define SIZE_POINTER 32
#elif defined(__arm64) || defined(__arm64__) || defined(__arm64) || defined(__aarch64__)
    #define CPU_ARM 1
    #define CPU_x86 0
    #define CPU_32_BIT 0
    #define CPU_64_BIT 1
    #define SIZE_POINTER 64
#elif defined(__arm) || defined(__arm__)
    #define CPU_ARM 1
    #define CPU_x86 0
    #define CPU_32_BIT 1
    #define CPU_64_BIT 0
    #define SIZE_POINTER 32
#else
    // unknown -> may change this
    #define CPU_x86 0
    #define CPU_ARM 0
    #define CPU_32_BIT 0
    #define CPU_64_BIT 0
    #define SIZE_POINTER 0
#endif

/**
 * detect COMPILER
 */
#define CC_CLANG 0
#define CC_CLANG_CL 0
#define CC_MSC 0
#define CC_MINGW 0
#define CC_GCC 0
#define CC_GXX 0

#if defined(_MSC_VER)
    /* Microsoft Visual Studio */
    #if defined(__clang__)
        #undef CC_CLANG_CL
        #define CC_CLANG_CL 1
    #else
        #undef CC_MSC
        #define CC_MSC 1
    #endif

    #define COMPILER_VERSION_PATCHLEVEL 0 // don't know what I should paste here, maybe the bukd number
    #if _MSC_VER >= 600 && _MSC_VER < 700
        #define COMPILER_VERSION_DEFAULT_FULL "Microsoft C 6.0"
        #define COMPILER_VERSION_MAJOR 0
        #define COMPILER_VERSION_MINOR 6
    #elif _MSC_VER >= 700 && _MSC_VER < 800
        #define COMPILER_VERSION_DEFAULT_FULL "Microsoft C/C++ 7.0"
        #define COMPILER_VERSION_MAJOR 0
        #define COMPILER_VERSION_MINOR 7
    #elif _MSC_VER >= 800 && _MSC_VER < 900
        #define COMPILER_VERSION_DEFAULT_FULL "Visual C++ 1.0"
        #define COMPILER_VERSION_MAJOR 1
        #define COMPILER_VERSION_MINOR 0
    #elif _MSC_VER >= 900 && _MSC_VER < 1000
        #define COMPILER_VERSION_DEFAULT_FULL "Visual C++ 2.0"
        #define COMPILER_VERSION_MAJOR 2
        #define COMPILER_VERSION_MINOR 0
    #elif _MSC_VER >= 1000 && _MSC_VER < 1010
        #define COMPILER_VERSION_DEFAULT_FULL "Visual C++ 4.0"
        #define COMPILER_VERSION_MAJOR 4
        #define COMPILER_VERSION_MINOR 0
    #elif _MSC_VER >= 1010 && _MSC_VER < 1020
        #define COMPILER_VERSION_DEFAULT_FULL "Visual C++ 4.1"
        #define COMPILER_VERSION_MAJOR 4
        #define COMPILER_VERSION_MINOR 1
    #elif _MSC_VER >= 1020 && _MSC_VER < 1100
        #define COMPILER_VERSION_DEFAULT_FULL "Visual C++ 4.2"
        #define COMPILER_VERSION_MAJOR 4
        #define COMPILER_VERSION_MINOR 2
    #elif _MSC_VER >= 1100 && _MSC_VER < 1200
        #define COMPILER_VERSION_DEFAULT_FULL "Visual Studio 97 [5.0] Visual C++ 5.0"
        #define COMPILER_VERSION_MAJOR 5
        #define COMPILER_VERSION_MINOR 0
    #elif _MSC_VER >= 1200 && _MSC_VER < 1300
        #define COMPILER_VERSION_DEFAULT_FULL "Visual Studio 6.0 SP5/SP6 Visual C++ 6.0 SP5/SP6"
        #define COMPILER_VERSION_MAJOR 6
        #define COMPILER_VERSION_MINOR 0
    #elif _MSC_VER >= 1300 && _MSC_VER < 1310 && _MSC_FULL_VER == 13009176
        #define COMPILER_VERSION_DEFAULT_FULL "Windows XP SP1 DDK"
        #define COMPILER_VERSION_MAJOR 6
        #define COMPILER_VERSION_MINOR 0
    #elif _MSC_VER >= 1300 && _MSC_VER < 1310
        #define COMPILER_VERSION_DEFAULT_FULL "Visual Studio .NET 2002 [7.0] Visual C++ .NET 2002 [7.0]"
        #define COMPILER_VERSION_MAJOR 7
        #define COMPILER_VERSION_MINOR 0
    #elif _MSC_VER >= 1310 && _MSC_VER < 1400 && _MSC_FULL_VER == 13102179
        #define COMPILER_VERSION_DEFAULT_FULL "Windows Server 2003 DDK"
        #define COMPILER_VERSION_MAJOR 7
        #define COMPILER_VERSION_MINOR 0
    #elif _MSC_VER >= 1310 && _MSC_VER < 1400 && _MSC_FULL_VER == 13102292
        #define COMPILER_VERSION_DEFAULT_FULL "Visual Studio .NET 2003 Beta [7.1]"
        #define COMPILER_VERSION_MAJOR 7
        #define COMPILER_VERSION_MINOR 1
    #elif _MSC_VER >= 1310 && _MSC_VER < 1400 && _MSC_FULL_VER == 13103052
        #define COMPILER_VERSION_DEFAULT_FULL "Visual Studio Toolkit 2003 [7.1]"
        #define COMPILER_VERSION_MAJOR 7
        #define COMPILER_VERSION_MINOR 1
    #elif _MSC_VER >= 1310 && _MSC_VER < 1400 && _MSC_FULL_VER == 13103077
        #define COMPILER_VERSION_DEFAULT_FULL "Visual Studio .NET 2003 [7.1] Visual C++ .NET 2003 [7.1]"
        #define COMPILER_VERSION_MAJOR 7
        #define COMPILER_VERSION_MINOR 1
    #elif _MSC_VER >= 1310 && _MSC_VER < 1400 && _MSC_FULL_VER == 13104035
        #define COMPILER_VERSION_DEFAULT_FULL "Windows Server 2003 SP1 DDK"
        #define COMPILER_VERSION_MAJOR 7
        #define COMPILER_VERSION_MINOR 1
    #endif

    #define COMPILER_VERSION_DEFAULT_FULL STR(_MSC_FULL_VER)
    #define COMPILER_VERSION_DEFAULT_MINI STR(_MSC_VER)
    #define COMPILER_VERSION STR(_MSC_VER)
#elif defined(__MINGW32__) || defined(__MINGW64__)
    #undef CC_MINGW
    #define CC_MINGW 1
#elif defined(__clang__)
    /* Clang/LLVM */
    #undef CC_CLANG
    #define CC_CLANG 1

    #define COMPILER_VERSION_DEFAULT_FULL __VERSION__
    #define COMPILER_VERSION_DEFAULT_MINI __clang_version__
    #define COMPILER_VERSION_MAJOR __clang_major__
    #define COMPILER_VERSION_MINOR __clang_minor__
    #define COMPILER_VERSION_PATCHLEVEL __clang_patchlevel__
    #define COMPILER_VERSION STR(__clang_major__ ) "." STR(__clang_minor__) "." STR(__clang_patchlevel__)
#elif defined(__GNUC__) || defined(__GNUG__)
    /* GNU GCC/G++ */
    // #define CC_GCC 1
    // #define CC_GCXX 1

    #define COMPILER_VERSION_DEFAULT_FULL __VERSION__
    #define COMPILER_VERSION_DEFAULT_MINI __VERSION__
    #define COMPILER_VERSION_MAJOR __GNUC__
    #define COMPILER_VERSION_MINOR __GNUC_MINOR__
    #define COMPILER_VERSION_PATCHLEVEL __GNUC_PATCHLEVEL__
    #if defined(__GNUC__)
        #define COMPILER_VERSION STR(__GNUC__) "." STR(__GNUC_MINOR__) "." STR(__GNUC_PATCHLEVEL__)
    #elif defined(__GNUG__)

    #endif
#else
    
#endif

#if defined(__GCC__) || defined(__APPLE_CC__)
    #undef CC_GCC
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

/**
 * general typedefs
 */

typedef signed char int8; // -128 - 127 // 1 byte <-> 8 bit
typedef signed short int16; // -32 768 - 32 767 // 2 byte <-> 16 bit
typedef signed int int32; // -2 147 483 648 - 2 147 483 647 // 4 byte <-> 32 bit
typedef signed long long int64; // -9 223 372 036 854 775 808 - 9 223 372 036 854 775 807 // 8 byte <-> 64 bit

typedef unsigned char index8; // 0 - 255
typedef unsigned short index16; // 0 - 65 535
typedef unsigned int index32; // 0 - 4 294 967 296
typedef unsigned long long index64; // 

#if SIZE_POINTER == 32
    typedef int32 intP;
    typedef index32 indexP;
#elif SIZE_POINTER == 64
    typedef int64 intP;
    typedef index64 indexP;
#else

#endif

// not really important / needed stuff
#define TRUE_STRING "true"
#define FALSE_STRING "false"
#define BOOL_1 TRUE_STRING
#define BOOL_0 FALSE_STRING
#define BOOL_TO_STRING_IMPL(bool) BOOL_##bool
#define BOOL_TO_STRING(bool) BOOL_TO_STRING_IMPL(bool)

#endif