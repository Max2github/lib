#ifndef SIMPLE_ARRAY_H
#define SIMPLE_ARRAY_H

#include "def.h"

#ifndef NO_STD_LIB
    #include <stdlib.h>
    #ifndef SIMPLE_ARRAY_H_MALLOC
    #define SIMPLE_ARRAY_H_MALLOC(size) malloc(size)
    #endif
    #ifndef SIMPLE_ARRAY_H_REALLOC
    #define SIMPLE_ARRAY_H_REALLOC(ptr, size, oldsize) realloc(ptr, size)
    #endif
    #ifndef SIMPLE_ARRAY_H_FREE
    #define SIMPLE_ARRAY_H_FREE(p) free(p)
    #endif
#endif

#define SIMPLE_ARRAY_DEFAULT_SIZE 10
#define SIMPLE_ARRAY_EXTEND_SIZE 10

#if LANG_CPP && LANG_CPP_STD >= 2011
    #define SIMPLE_ARRAY_DECLTYPE(var) decltype(var)
#else
    #define SIMPLE_ARRAY_DECLTYPE(var)
#endif

#define SIMPLE_ARRAY_TYPE_FROM_ARR(arr) SIMPLE_ARRAY_DECLTYPE((arr).data)
#define SIMPLE_ARRAY_CAST_TO_POINTER(arr, p) ((SIMPLE_ARRAY_TYPE_FROM_ARR(arr)) (p))

#define SIMPLE_ARRAY(type) \
struct { \
    type * data; \
    indexP count; \
    indexP written; \
}

#define SIMPLE_ARRAY_EXTEND(len) (((indexP) (((len) / SIMPLE_ARRAY_EXTEND_SIZE) * SIMPLE_ARRAY_EXTEND_SIZE + SIMPLE_ARRAY_EXTEND_SIZE)))

#ifndef SIMPLE_ARRAY_MEMCOPY
#define SIMPLE_ARRAY_MEMCOPY(dest, src, size) \
{ \
    for (indexP icp = 0; icp < (size); icp++) { \
        ((char *) (dest))[icp] = ((char *)(src))[icp]; \
    } \
}
#endif

#define SIMPLE_ARRAY_INIT(arr, type, size) \
{ \
    (arr).data = (type*) SIMPLE_ARRAY_H_MALLOC(sizeof(type) * size); \
    (arr).count = size; \
    (arr).written = 0; \
}

#define SIMPLE_ARRAY_CREATE_SIZE(type, size) \
{ \
    ((type *) SIMPLE_ARRAY_H_MALLOC(sizeof(type) * (size))), \
    (size), \
    0 \
}

#define SIMPLE_ARRAY_CREATE_SIZE_VAR(var, size) \
{ \
    ((SIMPLE_ARRAY_DECLTYPE(var) *) SIMPLE_ARRAY_H_MALLOC(sizeof(var) * (size))), \
    (size), \
    0 \
}

#define SIMPLE_ARRAY_CREATE(type) SIMPLE_ARRAY_CREATE_SIZE(type, SIMPLE_ARRAY_DEFAULT_SIZE)

#define SIMPLE_ARRAY_WRITE_NO_CHECK(arr, index, Data, len) \
{ \
    SIMPLE_ARRAY_MEMCOPY((void *) ((arr).data + index), (void *) Data, len * sizeof(*(arr).data)); \
    indexP newSize = ((index) + (len)); \
    if (newSize > (arr).written) { (arr).written = newSize; } \
}

#define SIMPLE_ARRAY_SET(arr, Data, len) \
{ \
    if ((arr).written + (len) > (arr).count) { \
        SIMPLE_ARRAY_H_FREE((void *) (arr).data); \
        arr = (SIMPLE_ARRAY_DECLTYPE(arr)) SIMPLE_ARRAY_CREATE_SIZE_VAR(*Data, (arr).count + SIMPLE_ARRAY_EXTEND(len)); \
        (arr).count = SIMPLE_ARRAY_EXTEND(len); \
    } \
    SIMPLE_ARRAY_WRITE_NO_CHECK(arr, 0, Data, len); \
}

#define SIMPLE_ARRAY_WRITE(arr, index, Data, len) \
{ \
    if ((arr).written + (len) > (index)) { \
        if ((index) + (len) > (arr).count) { \
            const indexP currentSize = (arr).count * sizeof(*Data); \
            void * newData = SIMPLE_ARRAY_H_REALLOC((void *) (arr).data, currentSize + SIMPLE_ARRAY_EXTEND(len) * sizeof(*Data), currentSize); \
            (arr).data = SIMPLE_ARRAY_CAST_TO_POINTER(arr, newData); \
            (arr).count += SIMPLE_ARRAY_EXTEND(len); \
        }\
        SIMPLE_ARRAY_WRITE_NO_CHECK(arr, index, Data, len); \
    } \
}

#define SIMPLE_ARRAY_APPEND_DATA(arr, Data, len) SIMPLE_ARRAY_WRITE(arr, (arr).written, Data, len)

#define SIMPLE_ARRAY_APPEND(arr, Data) SIMPLE_ARRAY_APPEND_DATA(arr, (&Data), 1)

#define SIMPLE_ARRAY_GET(arr, index) ((arr).data)[index]

#define SIMPLE_ARRAY_SHIFT_RIGHT(arr, index, amount) \
{ \
    if ((amount) > 0 && (index) < (arr).written) { \
        indexP endIndex = (arr).written; \
        indexP maxAmount = endIndex - (index); \
        indexP realAmount = (amount); \
        if (maxAmount < (amount)) { \
            realAmount = maxAmount; \
            (arr).written += (amount) - maxAmount; /* advance */ \
        } \
        /* append the last elements*/ \
        for (indexP curAmount = realAmount; curAmount > 0; curAmount--) { \
            SIMPLE_ARRAY_APPEND(arr, SIMPLE_ARRAY_GET(arr, endIndex - curAmount)); \
        } \
        /* move the previous element to the current index (from end to beginning) */ \
        for (indexP curIndex = endIndex - 1; curIndex >= (index + realAmount); curIndex--) { \
            SIMPLE_ARRAY_GET(arr, curIndex) = SIMPLE_ARRAY_GET(arr, curIndex - realAmount); \
        } \
    } \
}

#define SIMPLE_ARRAY_FREE(arr) \
{ \
    SIMPLE_ARRAY_H_FREE((void *)(arr).data); \
    (arr).data = 0; \
    (arr).written = 0; \
    (arr).count = 0; \
}

#define SIMPLE_ARRAY_COPY(dest, arr, type) \
{ \
    SIMPLE_ARRAY_INIT(dest, type, (arr).count); \
    for (indexP simpleArrayCopyI = 0; simpleArrayCopyI < (arr).written; simpleArrayCopyI++) { \
        SIMPLE_ARRAY_APPEND(dest, (SIMPLE_ARRAY_GET(arr, simpleArrayCopyI))); \
    } \
}

#endif