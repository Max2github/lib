#ifdef USE_CHAR_TRAITS

#include <string>
//#define SIMPLE_ARRAY_MEMCOPY(dest, src, size) __builtin_memcpy((void *) (dest), (const void *) (src), (unsigned long long) (size))
#define SIMPLE_ARRAY_MEMCOPY(dest, src, size) std::char_traits<char>::copy((char *)dest, (const char *)src, size)

#endif

#ifdef USE_SIZE_T

    // set to same as size_t, which std::string uses
    #define SMARTBUFFER_LEN_T std::size_t

    #define INCLUDE_C_SOURCES 1

#endif

#ifdef NO_REALLOC

#undef INCLUDE_C_SOURCES
#define INCLUDE_C_SOURCES 1

void * no_realloc(void* oldPtr, const size_t newsize, const size_t oldSize) {
    void * res = SMARTSTRING_H_MALLOC(newsize);
    SIMPLE_ARRAY_MEMCOPY(res, oldPtr, oldSize);
    SMARTSTRING_H_FREE(oldPtr);
    return res;
}

#undef SMARTSTRING_HPP_REALLOC
#define SMARTSTRING_HPP_REALLOC(oldPtr, newsize, oldSize) no_realloc(oldPtr, newsize, oldSize)

#endif

#define INCLUDE_CPP_SOURCES 1

#include "../../../../CPP/smartstring/smartstring.hpp"
#include "../../../../CPP/smartbuffer/smartbuffer.hpp"

#ifdef INCLUDE_C_SOURCES
#include "../../../../src/smartbuffer/smartbuffer.c"
#include "../../../../src/smartbuffer/sBuffer_single.c"
#include "../../../../src/smartstring/smartstring.c"
#endif 

#ifdef INCLUDE_CPP_SOURCES
#include "../../../../CPP/smartbuffer/smartbuffer.cpp"
#include "../../../../CPP/smartstring/smartstring.cpp"
#endif
