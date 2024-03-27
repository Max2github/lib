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

CHEADER_START

// never call this - it does nothing
DLL_HIDDEN inline void smartbuffer_h_check_macro() {
    ASSERT_COMPILE_TIME(sizeof(SMARTBUFFER_CHAR) == 1, "SMARTBUFFER_CHAR must be of size 1!");
}

#define SMARTBUFFER_H_MEMCOPY(dest, src, size) SIMPLE_ARRAY_MEMCOPY(dest, src, size)

typedef SIMPLE_ARRAY(SMARTBUFFER_CHAR) sBuffer_single_data;

struct sBuffer_flags {
    SMARTBUFFER_BOOL_T is_child : 1;
    SMARTBUFFER_BOOL_T is_readonly : 1;  // can this change?
    SMARTBUFFER_BOOL_T is_data_allocated : 1; // does the data need to be freed?
    SMARTBUFFER_BOOL_T is_this_allocated : 1; // does the pointer to this (sBuffer_single) need to be freed?
};
typedef struct sBuffer_flags sBuffer_flags;



struct sBuffer_single {
    sBuffer_flags flags;

    union {
        // "real" data
        struct {
            sBuffer_single_data data;        // the real data
            SMARTBUFFER_LEN_T usage_count;   // how often is this used?

            #if 0
            #if SIZE_POINTER == 16
            index8 restbuf[1];
            #elif SIZE_POINTER == 32
            index8 restbuf[3];
            #elif SIZE_POINTER == 64
            index8 restbuf[7];
            #endif
            #endif
        };

        // this buffer is just a child of another buffer
        struct {
            struct sBuffer_single * parent;
            SMARTBUFFER_LEN_T begin;
            SMARTBUFFER_LEN_T size;
        } child;
    };
};
typedef struct sBuffer_single sBuffer_single;
typedef sBuffer_single * sBuffer_single_ptr;

typedef SIMPLE_ARRAY(sBuffer_single_ptr) sBuffer;

struct sBuffer_single_with_index {
    sBuffer_single_ptr buf;
    SMARTBUFFER_LEN_T index; // index in buf
};
typedef struct sBuffer_single_with_index sBuffer_single_with_index;

struct sBuffer_index_descr {
    sBuffer_single_with_index single;
    SMARTBUFFER_LEN_T index; // index inside sBuffer
};
typedef struct sBuffer_index_descr sBuffer_index_descr;

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

/**
 * @brief Create a new sBuffer_single, which is a child of another sBuffer_single
 * @param 0 (sBuffer_single_ptr) the parent buffer
 * @param 1 (SMARTBUFFER_LEN_T) at which index the child begins
 * @param 2 (SMARTBUFFER_BOOL_T) the size of the child buffer
 * @return sBuffer_single_ptr
 */
sBuffer_single_ptr sBuffer_single_create_child(const sBuffer_single_ptr, SMARTBUFFER_LEN_T, SMARTBUFFER_LEN_T);

SMARTBUFFER_BOOL_T sBuffer_single_add_char(sBuffer_single_ptr, const SMARTBUFFER_CHAR);
SMARTBUFFER_LEN_T sBuffer_single_add(sBuffer_single_ptr, const SMARTBUFFER_CHAR *, SMARTBUFFER_LEN_T);

/**
 * @brief Insert data into an sBuffer_single
 * 
 * @param buf
 * @param index
 * @param data
 * @param size
 * 
 * @return SMARTBUFFER_LEN_T amount inserted
 */
SMARTBUFFER_LEN_T sBuffer_single_insert(sBuffer_single_ptr, SMARTBUFFER_LEN_T, const SMARTBUFFER_CHAR *, SMARTBUFFER_LEN_T);

const SMARTBUFFER_CHAR * sBuffer_single_get(const sBuffer_single_ptr);
SMARTBUFFER_LEN_T sBuffer_single_count(const sBuffer_single_ptr);
SMARTBUFFER_BOOL_T sBuffer_single_is_empty(const sBuffer_single_ptr);
SMARTBUFFER_LEN_T sBuffer_single_count_remaining(const sBuffer_single_ptr);

/**
 * @brief Shift all element to the "right" (the memory after -> appends)
 * The shifting starts a the index given (second argument).
 * The third arguments specifies by which amount it should be shifted (usually 1).
 * 
 * @details e.g.
 * buffer = Hello World
 * shift_righ(buffer, 0, 2)
 * -> buffer == HeHello World
 * "Hello World" was shifted by two, but the cells before are NOT overwritten!
 * 
 * This is mainly used for inserting a SMARTBUFFER_CHAR at the given index
 * 
 * @param buf sBuffer_single_ptr
 * @param index SMARTBUFFER_LEN_T
 * @param amount SMARTBUFFER_LEN_T
 * @return actual amount shifted
 */
SMARTBUFFER_LEN_T sBuffer_single_shift_right(sBuffer_single_ptr, SMARTBUFFER_LEN_T, SMARTBUFFER_LEN_T);

/**
 * @brief 
 * 
 * @return SMARTBUFFER_LEN_T size of SMARTBUFFER_CHAR's written
 */
SMARTBUFFER_LEN_T sBuffer_single_write(sBuffer_single_ptr,SMARTBUFFER_LEN_T, const SMARTBUFFER_CHAR *, SMARTBUFFER_LEN_T);

sBuffer_single_ptr sBuffer_single_copy(const sBuffer_single_ptr, SMARTBUFFER_LEN_T from, SMARTBUFFER_LEN_T to);

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
SMARTBUFFER_LEN_T sBuffer_append(sBuffer *, const SMARTBUFFER_CHAR *, SMARTBUFFER_LEN_T);
void sBuffer_add(sBuffer *, sBuffer_single_ptr);

/**
 * @brief Shift all element to the "right" (the memory after -> appends)
 * The shifting starts a the index given (second argument).
 * The third arguments specifies by which amount it should be shifted (usually 1).
 * 
 * This is mainly used for inserting a sBuffer_single_ptr at the given index
 * 
 * @param buf sBuffer *
 * @param index SMARTBUFFER_LEN_T
 * @param amount SMARTBUFFER_LEN_T
 */
void sBuffer_shift_right(sBuffer *, SMARTBUFFER_LEN_T, SMARTBUFFER_LEN_T);

/**
 * @brief Inserts an element without shifting the array
 * @attention This is dangerous, as it may overwrite data, which should NOT be overwritten and this may lead to data loss and memory leaks!
 * @attention Please used sBuffer_shift_right beforehand!
 * 
 */
void sBuffer_insert_single_noShift(sBuffer *, SMARTBUFFER_LEN_T, sBuffer_single_ptr);

/**
 * @brief Insert an sBuffer_single_ptr at the given index
 * 
 */
void sBuffer_insert_single(sBuffer *, SMARTBUFFER_LEN_T, sBuffer_single_ptr);

sBuffer_single_ptr sBuffer_get(const sBuffer *, SMARTBUFFER_LEN_T);

/**
 * @brief Search for a sBuffer_single_ptr in the buffer and remove it
 * @attention removes all occurences
 * 
 * @return SMARTBUFFER_BOOL_T how many buffers were removed
 */
SMARTBUFFER_LEN_T sBuffer_remove_single(sBuffer *, const sBuffer_single_ptr);

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

/**
 * @brief Search in which sBuffer_single a index is
 * 
 * 
 * @return sBuffer_index_tuple
 *         index1: the index of the sBuffer_single in the sBuffer
 *         index2: the local index (in the sBuffer_single) corresponding to the given index
 */
sBuffer_index_descr sBuffer_find_index(const sBuffer *, SMARTBUFFER_LEN_T);

/**
 * @brief Write data into an sBuffer starting at a given index.
 * This overwrites existing data and appends data,
 * when writing at/after the end of the buffer.
 * 
 * @param buffer
 * @param index where to start writing
 * @param data
 * @param len
 * 
 * @return SMARTBUFFER_LEN_T amount written
 */
SMARTBUFFER_LEN_T sBuffer_write(sBuffer *, SMARTBUFFER_LEN_T, const SMARTBUFFER_CHAR *, SMARTBUFFER_LEN_T);

/**
 * @brief Insert data into an sBuffer at a given index.
 * 
 * @param buffer
 * @param index where to start writing
 * @param data
 * @param len
 * 
 * @return SMARTBUFFER_LEN_T amount written
 */
SMARTBUFFER_LEN_T sBuffer_insert(sBuffer *, SMARTBUFFER_LEN_T, const SMARTBUFFER_CHAR *, SMARTBUFFER_LEN_T);

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

CHEADER_END

#endif