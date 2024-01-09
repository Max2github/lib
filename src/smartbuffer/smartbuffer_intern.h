#ifndef SMARTBUFFER_INTERN_H
#define SMARTBUFFER_INTERN_H

#include "../../smartbuffer.h"

// sBuffer_single_data

sBuffer_single_data sBuffer_single_data_create(SMARTBUFFER_LEN_T);
void sBuffer_single_data_add_char(sBuffer_single_data *, const SMARTBUFFER_CHAR);
void sBuffer_single_data_add(sBuffer_single_data *, const SMARTBUFFER_CHAR *, SMARTBUFFER_LEN_T);
void sBuffer_single_data_shift_right(sBuffer_single_data *, SMARTBUFFER_LEN_T, SMARTBUFFER_LEN_T);
SMARTBUFFER_CHAR * sBuffer_single_data_get(const sBuffer_single_data *);
SMARTBUFFER_LEN_T sBuffer_single_data_count(const sBuffer_single_data *);
SMARTBUFFER_LEN_T sBuffer_single_data_count_remaining(const sBuffer_single_data *);
void sBuffer_single_data_free(sBuffer_single_data *);

SMARTBUFFER_LEN_T sBuffer_single_usageCount_decrease(sBuffer_single_ptr buf);
SMARTBUFFER_LEN_T sBuffer_single_usageCount_increase(sBuffer_single_ptr buf);

// sBuffer_single

sBuffer_single sBuffer_single_create_intern(SMARTBUFFER_LEN_T, SMARTBUFFER_BOOL_T, SMARTBUFFER_BOOL_T);
void sBuffer_usageCount_increaseAll(sBuffer *);
void sBuffer_usageCount_decreaseAll(sBuffer *);

#endif