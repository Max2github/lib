#ifndef SMARTBUFFER_INTERN_H
#define SMARTBUFFER_INTERN_H

#include "../../smartbuffer.h"

CHEADER_START

//SMARTBUFFER_LEN_T sBuffer_single_usageCount_decrease(sBuffer_single_ptr buf);
//SMARTBUFFER_LEN_T sBuffer_single_usageCount_increase(sBuffer_single_ptr buf);

// sBuffer_single

sBuffer_single sBuffer_single_create_intern(SMARTBUFFER_LEN_T, SMARTBUFFER_BOOL_T, SMARTBUFFER_BOOL_T);
void sBuffer_usageCount_increaseAll(sBuffer *);
void sBuffer_usageCount_decreaseAll(sBuffer *);

CHEADER_END

#endif