#ifndef SMARTBUFFER_H
#define SMARTBUFFER_H

#ifndef NO_STD_LIB
    #include<stdlib.h>

    #ifndef SMARTBUFFER_H_MALLOC
    #define SMARTBUFFER_H_MALLOC(size) malloc(size)
    #endif
    #ifndef SMARTBUFFER_H_REALLOC
    #define SMARTBUFFER_H_REALLOC(oldP, size) realloc(oldP, size)
    #endif
    #ifndef SMARTBUFFER_H_FREE
    #define SMARTBUFFER_H_FREE(p) free(p)
    #endif
#endif

// define SIMPLE_ARRAY allocation functions to be the same as ours
#undef SIMPLE_ARRAY_H_MALLOC
#undef SIMPLE_ARRAY_H_REALLOC
#undef SIMPLE_ARRAY_H_FREE
#define SIMPLE_ARRAY_H_MALLOC  SMARTBUFFER_H_MALLOC
#define SIMPLE_ARRAY_H_REALLOC SMARTBUFFER_H_REALLOC
#define SIMPLE_ARRAY_H_FREE    SMARTBUFFER_H_FREE

#include "templates/def.h"
#include "templates/simple_array.h"

#ifndef SMARTBUFFER_CUSTOM
    #define SMARTBUFFER_CHAR index8
    #define SMARTBUFFER_LEN_T index64
    #define SMARTBUFFER_BOOL_T bool
    #define SMARTBUFFER_TRUE true
    #define SMARTBUFFER_FALSE false
#endif

#define SMARTBUFFER_H_MEMCOPY(dest, src, size) SIMPLE_ARRAY_MEMCOPY(dest, src, size)

typedef SIMPLE_ARRAY(SMARTBUFFER_CHAR) sBuffer_single_data;

struct sBuffer_flags {
    SMARTBUFFER_BOOL_T is_readonly : 1;  // can this change?
    SMARTBUFFER_BOOL_T is_data_allocated : 1; // does the data need to be freed?
    SMARTBUFFER_BOOL_T is_this_allocated : 1; // does the pointer to this (sBuffer_single) need to be freed?
};

struct sBuffer_single {
    sBuffer_single_data data;        // the real data
    SMARTBUFFER_LEN_T usage_count;   // how often is this used?

    //struct {
    SMARTBUFFER_BOOL_T is_readonly /*: 1*/;  // can this change?
    SMARTBUFFER_BOOL_T is_data_allocated /*: 1*/; // does the data need to be freed?
    SMARTBUFFER_BOOL_T is_this_allocated /*: 1*/; // does the pointer to this (sBuffer_single) need to be freed?
    //};
};
typedef struct sBuffer_single sBuffer_single;
typedef sBuffer_single * sBuffer_single_ptr;

typedef SIMPLE_ARRAY(sBuffer_single_ptr) sBuffer;

/**
 * @brief A reader funtion pointer used for sBuffer_read
 * @param 0 (void *) the user data, which was passed to sBuffer_read
 * @param 1 (const SMARTBUFFER_CHAR *) the data to read
 * @param 2 (SMARTBUFFER_LEN_T) how lon the data to read is
 */
typedef void (*sBuffer_readHandler)(void *, const SMARTBUFFER_CHAR *, SMARTBUFFER_LEN_T);

#define SMARTBUFFER_FOREACH_P(countVarName, iteratorVarName, bufferP, code) \
{ \
    for (SMARTBUFFER_LEN_T countVarName = 0; countVarName < sBuffer_count_single(bufferP); i++) { \
        sBuffer_single_ptr iteratorVarName = sBuffer_get((bufferP), (countVarName)); \
        code \
    } \
}

#define SMARTBUFFER_FOR_P(countVarName, bufferP, code) \
{ \
    for (SMARTBUFFER_LEN_T countVarName = 0; countVarName < sBuffer_count_single(bufferP); i++) { \
        code \
    } \
}

#define SMARTBUFFER_FOREACH_V(countVarName, iteratorVarName, bufferV, code) SMARTBUFFER_FOREACH_P(countVarName, iteratorVarName, &(bufferV), code)
#define SMARTBUFFER_FOR_V(countVarName, bufferV, code) SMARTBUFFER_FOR_P(countVarName, &(bufferV), code)

// sBuffer_single_ptr

/**
 * @brief Allocate a sBuffer_single and return a pointer to it
 * @param 0 (SMARTBUFFER_LEN_T) the lenth / size (count - not bytes!) of the new buffer
 * @param 1 (SMARTBUFFER_BOOL_T) if the buffer should be const / readonly (you will not be able to write to it)
 * @return sBuffer_single_ptr
 */
sBuffer_single_ptr sBuffer_single_create(SMARTBUFFER_LEN_T);

/**
 * @brief The new buffer just points to the data.
 * @param 0 (const SMARTBUFFER_CHAR *) a pointer to the data
 * @param 1 (SMARTBUFFER_LEN_T) the lenth / size (count - not bytes!) of the data
 * @return sBuffer_single_ptr
 */
sBuffer_single_ptr sBuffer_single_create_static(const SMARTBUFFER_CHAR *, SMARTBUFFER_LEN_T);
SMARTBUFFER_BOOL_T sBuffer_single_add_char(sBuffer_single_ptr, const SMARTBUFFER_CHAR);
SMARTBUFFER_LEN_T sBuffer_single_add(sBuffer_single_ptr, const SMARTBUFFER_CHAR *, SMARTBUFFER_LEN_T);
const SMARTBUFFER_CHAR * sBuffer_single_get(const sBuffer_single_ptr);
SMARTBUFFER_LEN_T sBuffer_single_count(const sBuffer_single_ptr);

sBuffer_single_ptr sBuffer_single_copy(const sBuffer_single_ptr, SMARTBUFFER_LEN_T from, SMARTBUFFER_LEN_T to);

/**
 * @brief 
 * 
 * @return SMARTBUFFER_LEN_T size of SMARTBUFFER_CHAR's written
 */
SMARTBUFFER_LEN_T sBuffer_single_write(const sBuffer_single_ptr,SMARTBUFFER_LEN_T, const SMARTBUFFER_CHAR *, SMARTBUFFER_LEN_T);

/**
 * @brief Clears the buffer, but does NOT free
 * 
 * @return SMARTBUFFER_LEN_T the bytes that were cleared
 */
SMARTBUFFER_LEN_T sBuffer_single_clear(sBuffer_single_ptr);

/**
 * @brief Free the data if the usage counter reaches 0 and the content was allocated
 * 
 * @return SMARTBUFFER_TRUE if the data was freed
 * @return SMARTBUFFER_FALSE if no data was freed
 */
SMARTBUFFER_BOOL_T sBuffer_single_freeData(sBuffer_single_ptr);
/**
 * @brief Free the pointer if it was allocated
 * This has no effect on the content.
 */
void sBuffer_single_freePtr(sBuffer_single_ptr);

/**
 * @brief Free an sBuffer_single
 * Calls
 * @ref Buffer_single_usageCount_decrease,
 * @ref sBuffer_single_freeData and
 * @ref sBuffer_single_freePtr
 */
void sBuffer_single_free(sBuffer_single_ptr);

sBuffer sBuffer_create(SMARTBUFFER_LEN_T size);
void sBuffer_addStr(sBuffer *, const SMARTBUFFER_CHAR *, SMARTBUFFER_LEN_T);
void sBuffer_add(sBuffer *, sBuffer_single_ptr);
sBuffer_single_ptr sBuffer_get(const sBuffer *, SMARTBUFFER_LEN_T);

/**
 * @brief Search for a sBuffer_single_ptr in the buffer and remove it
 * @attention removes all occurences
 * 
 * @return SMARTBUFFER_BOOL_T if the buffer was removed
 */
SMARTBUFFER_BOOL_T sBuffer_remove_single(sBuffer *, const sBuffer_single_ptr);

SMARTBUFFER_LEN_T sBuffer_count_single(const sBuffer *);
SMARTBUFFER_LEN_T sBuffer_count(const sBuffer *);

/**
 * @brief Copy a sBuffer, but does not copy the content
 * The usage conut of the elements is increased
 * @return a new sBuffer
 */
sBuffer sBuffer_copy(const sBuffer *);
/**
 * @brief Copy a sBuffer, including the contents
 * All contents are duplicated.
 * @return a new sBuffer 
 */
sBuffer sBuffer_copy_deep(const sBuffer *);

/**
 * @brief Read from the buffer
 * @param 0 (const sBuffer *) a pointer to the buffer
 * @param 1 (sBuffer_readHandler) a function pointer - the reader function to whom the data will be passed
 * @param 2 (SMARTBUFFER_LEN_T) the maximum lenth to read - please only up to sBuffer_count()
 * @param 3 (void *) some data the user wants to pass to the reader function
 * @return sBuffer_single_ptr
 */
SMARTBUFFER_LEN_T sBuffer_read(const sBuffer *, sBuffer_readHandler, SMARTBUFFER_LEN_T, void *);

SMARTBUFFER_LEN_T sBuffer_write(sBuffer *, SMARTBUFFER_LEN_T, const SMARTBUFFER_CHAR *, SMARTBUFFER_LEN_T);

sBuffer_single_ptr sBuffer_join(const sBuffer *);

/**
 * @brief Clear the buffer, but does NOT free
 * - All readonly buffers will be removed
 * - All shared buffers (usagecount > 1) will be removed
 * - All non-shared buffers (usagecount == 1) will be cleared
 */
void sBuffer_clear(sBuffer *);

/**
 * @brief free the whole buffer
 * calls sBuffer_single_freeData and sBuffer_single_freePtr on each element
 */
void sBuffer_free(sBuffer *);

#endif