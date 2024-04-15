#ifndef SMARTBUFFER_SBUFFER_SINGLE_H
#define SMARTBUFFER_SBUFFER_SINGLE_H

#include "def.h"

CHEADER_START

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
            SMARTBUFFER_CHAR * data;
            SMARTBUFFER_LEN_T allocated;
        } own;

        // this buffer is just a child of another buffer
        struct {
            struct sBuffer_single * parent;
            SMARTBUFFER_LEN_T begin;
        } child;
    };

    SMARTBUFFER_LEN_T len;
    SMARTBUFFER_LEN_T usage_count;   // how often is this used?
};
typedef struct sBuffer_single sBuffer_single;
typedef sBuffer_single * sBuffer_single_ptr;

SMARTBUFFER_LEN_T sBuffer_single_usageCount_get(sBuffer_single_ptr);
SMARTBUFFER_LEN_T sBuffer_single_usageCount_increase(sBuffer_single_ptr);
SMARTBUFFER_LEN_T sBuffer_single_usageCount_decrease(sBuffer_single_ptr);

/**
 * @brief Allocate a sBuffer_single and return a pointer to it
 * @param 0 (SMARTBUFFER_LEN_T) the lenth / size (count - not bytes!) of the new buffer
 * @return sBuffer_single_ptr
 */
sBuffer_single_ptr sBuffer_single_create(SMARTBUFFER_LEN_T);

/**
 * @brief Allocate a sBuffer_single and return a pointer to it
 * This special buffer cannot reallocate, but it does just one allocation at initialization, making it more efficient.
 * @param 0 (SMARTBUFFER_LEN_T) the lenth / size (count - not bytes!) of the new buffer
 * @return sBuffer_single_ptr
 */
sBuffer_single_ptr sBuffer_single_create_once(SMARTBUFFER_LEN_T);

/**
 * @brief Allocate a sBuffer_single and return a pointer to it
 * The data will be copied, but the buffer will be readonly.
 * This is more efficient than sBuffer_single_create, as there is only one allocation!
 * @param 0 (const SMARTBUFFER_CHAR *) a pointer to the data
 * @param 1 (SMARTBUFFER_LEN_T) the lenth / size (count - not bytes!) of the data
 * @return sBuffer_single_ptr
 */
sBuffer_single_ptr sBuffer_single_create_readonly(const SMARTBUFFER_CHAR *, SMARTBUFFER_LEN_T);

/**
 * @brief The new buffer just points to the data.
 * @param 0 (const SMARTBUFFER_CHAR *) a pointer to the data
 * @param 1 (SMARTBUFFER_LEN_T) the lenth / size (count - not bytes!) of the data
 * @param 2 (SMARTBUFFER_LEN_T) the total size on which can be written (must be at least as big as length, can be greater)
 * @return sBuffer_single_ptr
 */
sBuffer_single_ptr sBuffer_single_create_unmananged(const SMARTBUFFER_CHAR *, SMARTBUFFER_LEN_T, SMARTBUFFER_LEN_T);

/**
 * @brief The new buffer just points to the data.
 * This is the same as unmanaged, except that it is readonly.
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
sBuffer_single_ptr sBuffer_single_create_child(sBuffer_single_ptr, SMARTBUFFER_LEN_T, SMARTBUFFER_LEN_T);

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
SMARTBUFFER_LEN_T sBuffer_single_size(const sBuffer_single_ptr);
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
SMARTBUFFER_LEN_T sBuffer_single_write(sBuffer_single_ptr, SMARTBUFFER_LEN_T, const SMARTBUFFER_CHAR *, SMARTBUFFER_LEN_T);

sBuffer_single_ptr sBuffer_single_copy(const sBuffer_single_ptr, SMARTBUFFER_LEN_T from, SMARTBUFFER_LEN_T to);

/**
 * @brief Clears the buffer, but does NOT free
 * 
 * @return SMARTBUFFER_LEN_T the bytes that were cleared
 */
SMARTBUFFER_LEN_T sBuffer_single_clear(sBuffer_single_ptr);

/**
 * @brief Free an sBuffer_single
 * Calls
 * @ref Buffer_single_usageCount_decrease,
 * @ref sBuffer_single_freeData and
 * @ref sBuffer_single_freePtr
 */
void sBuffer_single_free(sBuffer_single_ptr);

CHEADER_END

#endif
