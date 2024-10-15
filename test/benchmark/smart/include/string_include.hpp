#ifdef USE_CHAR_TRAITS

#include <string>
//#define SIMPLE_ARRAY_MEMCOPY(dest, src, size) __builtin_memcpy((void *) (dest), (const void *) (src), (unsigned long long) (size))
#define SIMPLE_ARRAY_MEMCOPY(dest, src, size) std::char_traits<char>::copy((char *)dest, (const char *)src, size)

#endif

#ifdef MEMORY_TEST
#include "MemoryCounter.hpp"
MemoryCounter MemoryCounter::mm;

#define SMARTSTRING_HPP_MALLOC(size) MemoryCounter::mm.Alloc(size, malloc);
#define SMARTSTRING_HPP_REALLOC(oldP, size, oldSize) MemoryCounter::mm.Realloc(oldP, size, realloc);
#define SMARTSTRING_HPP_FREE(p) MemoryCounter::mm.Free(p, free);

#define SMARTBUFFER_HPP_MALLOC(size) MemoryCounter::mm.Alloc(size, malloc);
#define SMARTBUFFER_HPP_REALLOC(oldP, size, oldSize) MemoryCounter::mm.Realloc(oldP, size, realloc);
#define SMARTBUFFER_HPP_FREE(p) MemoryCounter::mm.Free(p, free);

#define SMARTBUFFER_H_MALLOC(size) MemoryCounter::mm.Alloc(size, malloc);
#define SMARTBUFFER_H_REALLOC(oldP, size, oldSize) MemoryCounter::mm.Realloc(oldP, size, realloc);
#define SMARTBUFFER_H_FREE(p) MemoryCounter::mm.Free(p, free);

void * MM_alloc(size_t size) { return MemoryCounter::mm.Alloc(size, malloc); }
void * MM_realloc(void * oldP, size_t size) { return MemoryCounter::mm.Realloc(oldP, size, realloc); }
void MM_free(void * p) { return MemoryCounter::mm.Free(p, free); }

#define SIMPLE_ARRAY_H_MALLOC(size) MM_alloc(size)
#define SIMPLE_ARRAY_H_REALLOC(oldP, size, oldSize) MM_realloc(oldP, size)
#define SIMPLE_ARRAY_H_FREE(p) MM_free(p)

#define MM_START MemoryCounter::mm.Start()
#define MM_STOP  MemoryCounter::mm.Stop()
#else
#define MM_START
#define MM_STOP
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
#define SMARTBUFFER_HPP_REALLOC(oldPtr, newsize, oldSize) no_realloc(oldPtr, newsize, oldSize)

#endif

#define INCLUDE_CPP_SOURCES 1

#include "../../../../CPP/smartstring/smartstring.hpp"
#include <m/smart/buffer.hpp>

#ifdef INCLUDE_C_SOURCES
#include "../../../../smart/buffer/src/smartbuffer.c"
#include "../../../../smart/buffer/src/sBuffer_single.c"
#include "../../../../src/smartstring/smartstring.c"
#endif 

#ifdef INCLUDE_CPP_SOURCES
#include "../../../../smart/buffer/c++/src/smartbuffer.cpp"
#include "../../../../CPP/smartstring/smartstring.cpp"
#endif
