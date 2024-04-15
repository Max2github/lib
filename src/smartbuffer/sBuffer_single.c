//#include "../../smartbuffer.h"
//#include "smartbuffer_intern.h"
#include "sBuffer_single.h"

// intern

sBuffer_single_ptr sBuffer_single_alloc_own(SMARTBUFFER_LEN_T size, SMARTBUFFER_BOOL_T is_readonly, SMARTBUFFER_BOOL_T is_allocated) {
    SMARTBUFFER_LEN_T allocSize = sizeof(sBuffer_single);
    // if the data should not be allocated, but we should alloc size then alloc size along with metadata alloc
    if (!is_allocated) {
        allocSize += size;
    }

    sBuffer_single_ptr buf = (sBuffer_single_ptr) SMARTBUFFER_H_MALLOC(allocSize);

    // set info / metadata
    if (size > 0) {
        if (is_allocated) {
            buf->own.data = (SMARTBUFFER_CHAR *) SMARTBUFFER_H_MALLOC(size);
            buf->own.allocated = size;
        } else {
            buf->own.data = ((SMARTBUFFER_CHAR *) buf) + sizeof(sBuffer_single);
            buf->own.allocated = 0;
        }
    } else {
        buf->own.allocated = 0;
    }
    buf->len = 0;
    buf->usage_count = 0;

    // set flags
    buf->flags.is_child = SMARTBUFFER_FALSE;
    buf->flags.is_readonly = is_readonly;
    buf->flags.is_data_allocated = is_allocated;
    buf->flags.is_this_allocated = SMARTBUFFER_TRUE;

    return buf;
}

SMARTBUFFER_LEN_T sBuffer_single_calc_extendSize(SMARTBUFFER_LEN_T lenToAdd, SMARTBUFFER_LEN_T stepSize, SMARTBUFFER_LEN_T mult) {
    //return SIMPLE_ARRAY_EXTEND(allocated);
    return ((lenToAdd / stepSize) * stepSize + stepSize) * mult;
}

SMARTBUFFER_BOOL_T sBuffer_single_realloc(sBuffer_single_ptr buf, SMARTBUFFER_LEN_T lenToAdd) {
    // if only data is allocated
    if (buf->flags.is_data_allocated) {
        SMARTBUFFER_LEN_T newlen = buf->own.allocated + sBuffer_single_calc_extendSize(lenToAdd, 10, 1);
        buf->own.data = (SMARTBUFFER_CHAR *) SMARTBUFFER_H_REALLOC(buf->own.data, newlen, buf->own.allocated);
        buf->own.allocated = newlen;
        return true;
    }
    return false;
}

SMARTBUFFER_BOOL_T sBuffer_single_check_realloc(sBuffer_single_ptr buf, SMARTBUFFER_LEN_T lenToAdd) {
    if (buf->flags.is_readonly) { return false; }
    if (buf->flags.is_child) { return false; }
    if(buf->len + lenToAdd > buf->own.allocated) {
        return sBuffer_single_realloc(buf, lenToAdd);
    }
    return true;
}

// extern

SMARTBUFFER_LEN_T sBuffer_single_usageCount_get(sBuffer_single_ptr buf) {
    if (buf == NULL) { return 0; }
    return buf->usage_count;
}

SMARTBUFFER_LEN_T sBuffer_single_usageCount_increase(sBuffer_single_ptr buf) {
    if (buf == NULL) { return 0; }
    return ++buf->usage_count;
}

SMARTBUFFER_LEN_T sBuffer_single_usageCount_decrease(sBuffer_single_ptr buf) {
    if (buf == NULL) { return 0; }
    if (buf->usage_count > 0) { buf->usage_count--; }
    return buf->usage_count;
}

sBuffer_single_ptr sBuffer_single_create(SMARTBUFFER_LEN_T size) {
    return sBuffer_single_alloc_own(size, SMARTBUFFER_FALSE, SMARTBUFFER_TRUE);
}

sBuffer_single_ptr sBuffer_single_create_once(SMARTBUFFER_LEN_T size) {
    return sBuffer_single_alloc_own(size, SMARTBUFFER_FALSE, SMARTBUFFER_FALSE);
}

sBuffer_single_ptr sBuffer_single_create_readonly(const SMARTBUFFER_CHAR * data, SMARTBUFFER_LEN_T len) {
    // readonly needs to be disabled for write
    sBuffer_single_ptr buf = sBuffer_single_create_once(len);
    sBuffer_single_write(buf, 0, data, len);
    // enable readonly
    buf->flags.is_readonly = true;
    return buf;
}

sBuffer_single_ptr sBuffer_single_create_unmananged(const SMARTBUFFER_CHAR * data, SMARTBUFFER_LEN_T len, SMARTBUFFER_LEN_T size) {
    sBuffer_single_ptr buf = sBuffer_single_create_once(0);
    buf->own.data = (SMARTBUFFER_CHAR *) data;
    buf->len = len;
    buf->own.allocated = size;
    return buf;
}

sBuffer_single_ptr sBuffer_single_create_static(const SMARTBUFFER_CHAR * data, SMARTBUFFER_LEN_T len) {
    sBuffer_single_ptr buf = sBuffer_single_alloc_own(0, SMARTBUFFER_TRUE, SMARTBUFFER_FALSE);
    buf->own.data = (SMARTBUFFER_CHAR *) data; // is_readonly is set, so this is safe
    buf->len = len;
    return buf;
}

sBuffer_single_ptr sBuffer_single_create_child(sBuffer_single_ptr parent, SMARTBUFFER_LEN_T begin, SMARTBUFFER_LEN_T len) {
    sBuffer_single_ptr buf = (sBuffer_single_ptr) SMARTBUFFER_H_MALLOC(sizeof(sBuffer_single));
    buf->child.parent = parent;
    buf->child.begin = begin;
    buf->len = len;

    buf->usage_count = 0;

    buf->flags.is_child = SMARTBUFFER_TRUE;
    buf->flags.is_data_allocated = SMARTBUFFER_FALSE;
    buf->flags.is_this_allocated = SMARTBUFFER_TRUE;

    // TODO: may need to change?
    buf->flags.is_readonly = SMARTBUFFER_TRUE;

    sBuffer_single_usageCount_increase(parent);
    return buf;
}

SMARTBUFFER_BOOL_T sBuffer_single_add_char(sBuffer_single_ptr buf, const SMARTBUFFER_CHAR c) {
    return sBuffer_single_add(buf, &c, 1);
}

SMARTBUFFER_LEN_T sBuffer_single_add(sBuffer_single_ptr buf, const SMARTBUFFER_CHAR * data, SMARTBUFFER_LEN_T len) {
    if (buf->flags.is_readonly) { return 0; }

    // TODO: check child?
    //if(buf->flags.is_child) {}

    return sBuffer_single_write(buf, buf->len, data, len);
}

SMARTBUFFER_LEN_T sBuffer_single_insert(sBuffer_single_ptr buf, SMARTBUFFER_LEN_T index, const SMARTBUFFER_CHAR * data, SMARTBUFFER_LEN_T len) {
    if (buf->flags.is_readonly) { return 0; }
    if (index < sBuffer_single_count(buf)) {
        sBuffer_single_shift_right(buf, index, len);
    }
    return sBuffer_single_write(buf, index, data, len);
}

const SMARTBUFFER_CHAR * sBuffer_single_get(const sBuffer_single_ptr buf) {
    if (buf->flags.is_child) {
        return sBuffer_single_get(buf->child.parent) + buf->child.begin;
    }
    return buf->own.data;
}

SMARTBUFFER_LEN_T sBuffer_single_count(const sBuffer_single_ptr buf) {
    return buf->len;
}

SMARTBUFFER_LEN_T sBuffer_single_size(const sBuffer_single_ptr buf) {
    if (buf->flags.is_data_allocated) {
        return buf->own.allocated;
    }
    return buf->len;
}

SMARTBUFFER_BOOL_T sBuffer_single_is_empty(const sBuffer_single_ptr buf) { return (sBuffer_single_count(buf) == 0); }

SMARTBUFFER_LEN_T sBuffer_single_count_remaining(const sBuffer_single_ptr buf) {
    if (buf->flags.is_child) { return 0; }
    return sBuffer_single_size(buf) - buf->len;
}

SMARTBUFFER_LEN_T sBuffer_single_shift_right(sBuffer_single_ptr buf, SMARTBUFFER_LEN_T index, SMARTBUFFER_LEN_T amount) {
    if (buf->flags.is_readonly) { return 0; }

    // TODO: check child?

    SMARTBUFFER_LEN_T len = sBuffer_single_count(buf);

    if (amount <= 0 || index >= len) { return 0; }

    const SMARTBUFFER_LEN_T toShift = len - index;
    SMARTBUFFER_LEN_T realAmount = amount;
    // in case that there are less elements to shift then we should shift
    if (toShift < amount) {
        realAmount = toShift;
    }

    SMARTBUFFER_CHAR * writeP = buf->own.data;
    SMARTBUFFER_LEN_T written = 0;

    #ifdef SMARTBUFFER_H_OPT_MEMMOVE
        sBuffer_single_check_realloc(buf, realAmount);
        SMARTBUFFER_H_OPT_MEMMOVE(writeP, writeP + realAmount, toShift);
        written = toShift;
    #else
        // append the last elements
        written += sBuffer_single_add(buf, writeP + (len - realAmount), realAmount);
        // move the previous element to the current index (from end to beginning)
        for (SMARTBUFFER_LEN_T curIndex = len - 1; curIndex >= (index + realAmount); curIndex--) {
            writeP[curIndex] = writeP[curIndex - realAmount];
        }
        written += len - (index + realAmount);
    #endif

    return written;
}

SMARTBUFFER_LEN_T sBuffer_single_write(sBuffer_single_ptr buf, SMARTBUFFER_LEN_T index, const SMARTBUFFER_CHAR * data, SMARTBUFFER_LEN_T len) {
    if (buf->flags.is_readonly) { return 0; }

    // TODO: check child
    if(buf->flags.is_child) { return 0; }

    // index cannot be bigger than the length
    if (index > buf->len) { return 0; }

    const SMARTBUFFER_LEN_T lenToAdd = (index + len) - buf->len;

    // check if we need to reallocate
    sBuffer_single_check_realloc(buf, lenToAdd);

    // TODO: check if we can write and how much we can write!!!

    // copy
    SMARTBUFFER_H_MEMCOPY(&(buf->own.data[index]), data, len);

    buf->len += lenToAdd;

    return len;
}

sBuffer_single_ptr sBuffer_single_copy(const sBuffer_single_ptr buf, SMARTBUFFER_LEN_T from, SMARTBUFFER_LEN_T to) {
    const SMARTBUFFER_LEN_T len = to - from;
    const SMARTBUFFER_LEN_T maxLen = sBuffer_single_count(buf);
    if (to > maxLen || maxLen < len) { return NULL; }
    sBuffer_single_ptr newBuf = sBuffer_single_create(len);
    sBuffer_single_add(newBuf, sBuffer_single_get(buf) + from, len);
    return newBuf;
}

SMARTBUFFER_LEN_T sBuffer_single_clear(sBuffer_single_ptr buf) {
    if (buf->flags.is_readonly) { return 0; }
    const SMARTBUFFER_LEN_T size = sBuffer_single_count(buf);

    if (buf->flags.is_child) {
        buf->len = 0;
    } else {
        buf->len = 0;
    }

    return size;
}

void sBuffer_single_free(sBuffer_single_ptr buf) {
    if (buf) {
        sBuffer_single_usageCount_decrease(buf);

        if (sBuffer_single_usageCount_get(buf) == 0) {
            if (buf->flags.is_child) {
                sBuffer_single_free(buf->child.parent);
            }
            if (buf->flags.is_data_allocated) {
                SMARTBUFFER_H_FREE(buf->own.data);
            }
            if (buf->flags.is_this_allocated) {
                SMARTBUFFER_H_FREE(buf);
            }
        }
    }
}
