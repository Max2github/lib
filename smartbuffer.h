#ifndef SMARTBUFFER_H
#define SMARTBUFFER_H

#include "src/smartbuffer/def.h"
#include "src/smartbuffer/sBuffer_single.h"

CHEADER_START

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