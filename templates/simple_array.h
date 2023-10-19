#ifndef SIMPLE_ARRAY_H
#define SIMPLE_ARRAY_H

#include "def.h"

#ifndef NO_STD_LIB
    #include <stdlib.h>
    #ifndef SIMPLE_ARRAY_H_MALLOC
    #define SIMPLE_ARRAY_H_MALLOC(size) malloc(size)
    #endif
    #ifndef SIMPLE_ARRAY_H_REALLOC
    #define SIMPLE_ARRAY_H_REALLOC(ptr, size) realloc(ptr, size)
    #endif
    #ifndef SIMPLE_ARRAY_H_FREE
    #define SIMPLE_ARRAY_H_FREE(p) free(p)
    #endif
#endif

#define SIMPLE_ARRAY_DEFAULT_SIZE 10
#define SIMPLE_ARRAY_EXTEND_SIZE 10

#define SIMPLE_ARRAY(type) \
struct { \
    union { \
        type * pointer; \
        indexP data; \
    }; \
    indexP count; \
    indexP written; \
}

#define SIMPLE_ARRAY_EXTEND(len) (((indexP) (((len) / SIMPLE_ARRAY_EXTEND_SIZE) * SIMPLE_ARRAY_EXTEND_SIZE + SIMPLE_ARRAY_EXTEND_SIZE)))

#define SIMPLE_ARRAY_MEMCOPY(dest, src, size) \
{ \
    for (indexP icp = 0; icp < (size); icp++) { \
        ((char *) (dest))[icp] = ((char *)(src))[icp]; \
    } \
}

#define SIMPLE_ARRAY_INIT(arr, type, size) \
{ \
    (arr).data = (indexP) SIMPLE_ARRAY_H_MALLOC(sizeof(type) * size); \
    (arr).count = size; \
    (arr).written = 0; \
}

#define SIMPLE_ARRAY_CREATE_SIZE(type, size) \
{ \
    ((type *) SIMPLE_ARRAY_H_MALLOC(sizeof(type) * (size))), \
    (size), \
    0 \
}

#define SIMPLE_ARRAY_CREATE(type) SIMPLE_ARRAY_CREATE_SIZE(type, SIMPLE_ARRAY_DEFAULT_SIZE)

#define SIMPLE_ARRAY_WRITE_NO_CHECK(arr, index, Data, len) \
{ \
    indexP i = 0; \
    for (; i < (len); i++) { \
        SIMPLE_ARRAY_MEMCOPY(((arr).data + (((index) + i) * sizeof(*(Data)))), (Data) + i, sizeof(*(Data))); \
    } \
    indexP newSize = ((index) + (len)); \
    if (newSize > (arr).written) { (arr).written = newSize; } \
}

#define SIMPLE_ARRAY_SET(arr, Data, len) \
{ \
    if ((arr).written + (len) > (arr).count) { \
        SIMPLE_ARRAY_FREE((void *) (arr).data); \
        arr = SIMPLE_ARRAY_CREATE_SIZE(Data, (arr).count + SIMPLE_ARRAY_EXTEND(len)); \
        (arr).count = SIMPLE_ARRAY_EXTEND(len); \
    } \
    SIMPLE_ARRAY_WRITE_NO_CHECK(arr, 0, Data, len); \
}

#define SIMPLE_ARRAY_WRITE(arr, index, Data, len) \
{ \
    if ((arr).written + (len) > (index)) { \
        if ((index) + (len) > (arr).count) { \
            (arr).data = (indexP) SIMPLE_ARRAY_H_REALLOC((void *) (arr).data, (arr).count * sizeof(*Data) + SIMPLE_ARRAY_EXTEND(len) * sizeof(*Data)); \
            (arr).count += SIMPLE_ARRAY_EXTEND(len); \
        }\
        SIMPLE_ARRAY_WRITE_NO_CHECK(arr, index, Data, len); \
    } \
}

#define SIMPLE_ARRAY_APPEND_DATA(arr, Data, len) SIMPLE_ARRAY_WRITE(arr, (arr).written, Data, len)

#define SIMPLE_ARRAY_APPEND(arr, Data) SIMPLE_ARRAY_APPEND_DATA(arr, (&Data), 1)

#define SIMPLE_ARRAY_GET(arr, index) ((arr).pointer)[index]

#define SIMPLE_ARRAY_SHIFT_RIGHT(arr, index, amount) \
{ \
    if ((amount) > 0) { \
        SMARTBUFFER_LEN_T endIndex = (arr).written; \
        /* append the last elements*/ \
        for (SMARTBUFFER_LEN_T curAmount = (amount); curAmount > 0; curAmount--) { \
            SIMPLE_ARRAY_APPEND(arr, SIMPLE_ARRAY_GET(arr, endIndex - curAmount)); \
        } \
        /* move the previous element to the current index (from end to beginning) */ \
        for (SMARTBUFFER_LEN_T curIndex = endIndex - 1; curIndex > (index) && curIndex >= (amount); curIndex--) { \
            SIMPLE_ARRAY_GET(arr, curIndex) = SIMPLE_ARRAY_GET(arr, curIndex - (amount)); \
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
    for (indexP i = 0; i < (arr).written; i++) { \
        SIMPLE_ARRAY_APPEND(dest, (SIMPLE_ARRAY_GET(arr, i))); \
    } \
}

#endif