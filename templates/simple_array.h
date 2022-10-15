#ifndef SIMPLE_ARRAY_H
#define SIMPLE_ARRAY_H

#ifndef NO_STD_LIB
    #include <stdlib.h>
    #define SIMPLE_ARRAY_H_MALLOC(size) malloc(size)
    #define SIMPLE_ARRAY_H_REALLOC(ptr, size) realloc(ptr, size)
    #define SIMPLE_ARRAY_H_FREE(p) free(p)
#endif

#define SIMPLE_ARRAY_DEFAULT_SIZE 10
#define SIMPLE_ARRAY_EXTEND_SIZE 10

#define SIMPLE_ARRAY(type) \
struct { \
    long data; \
    long count; \
    long written; \
}

#define SIMPLE_ARRAY_EXTEND(len) (((long) ((len / SIMPLE_ARRAY_EXTEND_SIZE) * SIMPLE_ARRAY_EXTEND_SIZE)))

#define SIMPLE_ARRAY_MEMCOPY(dest, src, size) \
{ \
    for (long icp = 0; icp < size; icp++) { \
        ((char *) dest)[icp] = ((char *)src)[icp]; \
    } \
}

#define SIMPLE_ARRAY_CREATE_SIZE(type, size) \
{ \
    (long) SIMPLE_ARRAY_H_MALLOC(sizeof(type) * size), \
    size, \
    0 \
}

#define SIMPLE_ARRAY_CREATE(type) SIMPLE_ARRAY_CREATE_SIZE(type, SIMPLE_ARRAY_DEFAULT_SIZE)

#define SIMPLE_ARRAY_WRITE_NO_CHECK(arr, index, Data, len) \
{ \
    long i = 0; \
    for (; i < len; i++) { \
        /*char * dest = ((char *) arr.data) + ((index + i) * sizeof(*Data)); */ \
        SIMPLE_ARRAY_MEMCOPY((arr.data + ((index + i) * sizeof(*Data))), &Data[i], sizeof(*Data)); \
    } \
    arr.written += i; \
}

#define SIMPLE_ARRAY_SET(arr, Data, len) \
{ \
    if (arr.written + len > arr.count) { \
        free((void *) arr.data); \
        arr = SIMPLE_ARRAY_CREATE_SIZE(Data, arr.count + SIMPLE_ARRAY_EXTEND(len)); \
    } \
    SIMPLE_ARRAY_WRITE_NO_CHECK(arr, 0, Data, len); \
}

#define SIMPLE_ARRAY_WRITE(arr, index, Data, len) \
{ \
    if (arr.written + len > arr.count) { \
        arr.data = (long) SIMPLE_ARRAY_H_REALLOC((void *) arr.data, arr.count * sizeof(*Data) + SIMPLE_ARRAY_EXTEND(len) * sizeof(*Data)); \
    }\
    SIMPLE_ARRAY_WRITE_NO_CHECK(arr, index, Data, len); \
}

#define SIMPLE_ARRAY_APPEND_DATA(arr, Data, len) SIMPLE_ARRAY_WRITE(arr, arr.written, Data, len)

#define SIMPLE_ARRAY_APPEND(arr, Data) SIMPLE_ARRAY_APPEND_DATA(arr, (&Data), 1)

#define SIMPLE_ARRAY_GET(arr, type, index) ((type *) arr.data)[index]

#define SIMPLE_ARRAY_FREE(arr) SIMPLE_ARRAY_H_FREE((void *)arr.data)

#endif