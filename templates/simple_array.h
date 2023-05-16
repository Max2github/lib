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
    indexP data; \
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

#define SIMPLE_ARRAY_CREATE_SIZE(type, size) \
{ \
    (indexP) SIMPLE_ARRAY_H_MALLOC(sizeof(type) * (size)), \
    size, \
    0 \
}

#define SIMPLE_ARRAY_CREATE(type) SIMPLE_ARRAY_CREATE_SIZE(type, SIMPLE_ARRAY_DEFAULT_SIZE)

#define SIMPLE_ARRAY_WRITE_NO_CHECK(arr, index, Data, len) \
{ \
    indexP i = 0; \
    for (; i < (len); i++) { \
        SIMPLE_ARRAY_MEMCOPY(((arr).data + (((index) + i) * sizeof(*(Data)))), (Data) + i, sizeof(*(Data))); \
    } \
    (arr).written += i; \
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
        if ((arr).written + (len) > (arr).count) { \
            (arr).data = (indexP) SIMPLE_ARRAY_H_REALLOC((void *) (arr).data, (arr).count * sizeof(*Data) + SIMPLE_ARRAY_EXTEND(len) * sizeof(*Data)); \
            (arr).count += SIMPLE_ARRAY_EXTEND(len); \
        }\
        SIMPLE_ARRAY_WRITE_NO_CHECK(arr, index, Data, len); \
    } \
}

#define SIMPLE_ARRAY_APPEND_DATA(arr, Data, len) SIMPLE_ARRAY_WRITE(arr, (arr).written, Data, len)

#define SIMPLE_ARRAY_APPEND(arr, Data) SIMPLE_ARRAY_APPEND_DATA(arr, (&Data), 1)

#define SIMPLE_ARRAY_GET(arr, type, index) ((type *) (arr).data)[index]

#define SIMPLE_ARRAY_FREE(arr) \
{ \
    SIMPLE_ARRAY_H_FREE((void *)(arr).data); \
    (arr).data = 0; \
    (arr).written = 0; \
    (arr).count = 0; \
}

#endif