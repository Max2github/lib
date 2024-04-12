#ifndef SMARTBUFFER_DEF_H
#define SMARTBUFFER_DEF_H

#include <string.h>
#ifndef NO_STD_LIB
    #include<stdlib.h>

    #ifndef SMARTBUFFER_H_MALLOC
    #define SMARTBUFFER_H_MALLOC(size) malloc(size)
    #endif
    #ifndef SMARTBUFFER_H_REALLOC
    #define SMARTBUFFER_H_REALLOC(oldP, newSize, oldSize) realloc(oldP, newSize)
    #endif
    #ifndef SMARTBUFFER_H_FREE
    #define SMARTBUFFER_H_FREE(p) free(p)
    #endif
    #ifndef SMARTBUFFER_H_MEMCOPY
    #define SMARTBUFFER_H_MEMCOPY(dest, src, size) memcpy(dest, src, size)
    #endif

    // optional
    #ifndef SMARTBUFFER_H_OPT_MEMMOVE
    //#define SMARTBUFFER_H_OPT_MEMMOVE(dest, src, size) memmove(dest, src, size)
    #endif
#endif

// define SIMPLE_ARRAY allocation functions to be the same as ours
#ifndef SIMPLE_ARRAY_H_MALLOC
//#undef SIMPLE_ARRAY_H_MALLOC
    #define SIMPLE_ARRAY_H_MALLOC  SMARTBUFFER_H_MALLOC
#endif
#ifndef SIMPLE_ARRAY_H_REALLOC
//#undef SIMPLE_ARRAY_H_REALLOC
    #define SIMPLE_ARRAY_H_REALLOC SMARTBUFFER_H_REALLOC
#endif
#ifndef SIMPLE_ARRAY_H_FREE
//#undef SIMPLE_ARRAY_H_FREE
    #define SIMPLE_ARRAY_H_FREE    SMARTBUFFER_H_FREE
#endif
#ifndef SIMPLE_ARRAY_MEMCOPY
//#undef SIMPLE_ARRAY_MEMCOPY
    #define SIMPLE_ARRAY_MEMCOPY   SMARTBUFFER_H_MEMCOPY
#endif

#include "../../templates/def.h"
#include "../../templates/simple_array.h"

#ifndef SMARTBUFFER_CUSTOM
    #define SMARTBUFFER_CHAR index8
    #define SMARTBUFFER_LEN_T index64
    #define SMARTBUFFER_BOOL_T bool
    #define SMARTBUFFER_TRUE true
    #define SMARTBUFFER_FALSE false
#endif

CHEADER_START

// never call this - it does nothing
DLL_HIDDEN inline void smartbuffer_h_check_macro() {
    ASSERT_COMPILE_TIME(sizeof(SMARTBUFFER_CHAR) == 1, "SMARTBUFFER_CHAR must be of size 1!");
}

CHEADER_END

#endif
