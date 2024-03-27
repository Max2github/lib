#if 0

#include "../../../src/general_alloc.h"

struct InitGlobalFastAlloc {
    InitGlobalFastAlloc() : memsize(4000000), mem(malloc(memsize)) {
        general_alloc_init(mem, memsize);
    }

    ~InitGlobalFastAlloc() {
        free(mem);
    }

    void memcopy(void * dest, const void * src, index64 size) {
        std::char_traits<char>::copy((char *)dest, (const char *)src, size);
    }

    void * alloc(index64 size) { return general_alloc(mem, size); }
    void * realloc(void * oldP, index64 size) {
        void * p = this->alloc(size);
        GENERAL_ALLOC_NODE * node = (GENERAL_ALLOC_NODE *) GENERAL_ALLOC_POINTER_PLUS(p, -GENERAL_ALLOC_NODE_SIZE);
        this->memcopy(p, oldP, node->size);
        return p;
    }
    void free(void * p) { general_free(mem, p); }

    /*static*/ const index64 memsize;
    void * mem;
};

InitGlobalFastAlloc globalAlloc;

#define SMARTSTRING_HPP_MALLOC(size) globalAlloc.alloc(size)
#define SMARTSTRING_HPP_REALLOC(oldP, size) globalAlloc.realloc(oldP, size)
#define SMARTSTRING_HPP_FREE(p) globalAlloc.free(p);

#endif

#define SIMPLE_ARRAY_MEMCOPY(dest, src, size) __builtin_memcpy((void *) (dest), (const void *) (src), (unsigned long long) (size))
//#define SIMPLE_ARRAY_MEMCOPY(dest, src, size) std::char_traits<char>::copy((char *)dest, (const char *)src, size)