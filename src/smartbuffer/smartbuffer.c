#include "../../smartbuffer.h"
#include "smartbuffer_intern.h"

// intern

// sBuffer_single_data

sBuffer_single_data sBuffer_single_data_create(SMARTBUFFER_LEN_T size) { return (sBuffer_single_data) SIMPLE_ARRAY_CREATE_SIZE(SMARTBUFFER_CHAR, size); }
sBuffer_single_data sBuffer_single_data_create_static(SMARTBUFFER_CHAR * buf, SMARTBUFFER_LEN_T len) {
    sBuffer_single_data data;
    data.pointer = buf;
    data.written = len;
    data.count = 0;
    return data;
}
void sBuffer_single_data_add_char(sBuffer_single_data * buf, const SMARTBUFFER_CHAR c) { SIMPLE_ARRAY_APPEND(*buf, c); }
void sBuffer_single_data_add(sBuffer_single_data * buf, const SMARTBUFFER_CHAR * data, SMARTBUFFER_LEN_T len) { SIMPLE_ARRAY_APPEND_DATA(*buf, data, len); }
SMARTBUFFER_CHAR * sBuffer_single_data_get(const sBuffer_single_data * buf) { return (SMARTBUFFER_CHAR *) buf->data; }
SMARTBUFFER_LEN_T sBuffer_single_data_count(const sBuffer_single_data * buf) { return buf->written; }
void sBuffer_single_data_free(sBuffer_single_data * buf) { SIMPLE_ARRAY_FREE(*buf); }

// sBuffer_single

sBuffer_single sBuffer_single_create_intern(SMARTBUFFER_LEN_T size, SMARTBUFFER_BOOL_T readonly, SMARTBUFFER_BOOL_T allocated) {
    sBuffer_single neu;
    neu.data = sBuffer_single_data_create(size);
    neu.is_readonly = readonly;
    neu.is_data_allocated = allocated;
    neu.usage_count = 0;
    return neu;
}

sBuffer_single sBuffer_single_create_static_intern(const SMARTBUFFER_CHAR * buf, SMARTBUFFER_LEN_T size) {
    sBuffer_single neu;
    neu.data = sBuffer_single_data_create_static((SMARTBUFFER_CHAR *) buf, size);
    neu.is_readonly = SMARTBUFFER_TRUE;
    neu.is_data_allocated = SMARTBUFFER_FALSE;
    neu.usage_count = 0;
    return neu;
}

sBuffer_single_ptr sBuffer_single_create_outer(sBuffer_single data) {
    sBuffer_single_ptr new_ptr = (sBuffer_single_ptr) SMARTBUFFER_H_MALLOC(sizeof(sBuffer_single));
    *new_ptr = data;
    new_ptr->is_this_allocated = true;
    return new_ptr;
}

// exported

sBuffer_single_ptr sBuffer_single_create_static(const SMARTBUFFER_CHAR * buf, SMARTBUFFER_LEN_T len) { return sBuffer_single_create_outer(sBuffer_single_create_static_intern(buf, len)); }
sBuffer_single_ptr sBuffer_single_create(SMARTBUFFER_LEN_T size) { return sBuffer_single_create_outer(sBuffer_single_create_intern(size, SMARTBUFFER_FALSE, SMARTBUFFER_TRUE)); }

SMARTBUFFER_BOOL_T sBuffer_single_add_char(sBuffer_single * buf, const SMARTBUFFER_CHAR c) {
    if (buf->is_readonly) { return false; }
    SMARTBUFFER_LEN_T oldCount = sBuffer_single_data_count(&(buf->data));
    sBuffer_single_data_add_char(&(buf->data), c);
    return (sBuffer_single_data_count(&(buf->data)) == oldCount + 1);
}
SMARTBUFFER_LEN_T sBuffer_single_add(sBuffer_single * buf, const SMARTBUFFER_CHAR * data, SMARTBUFFER_LEN_T size) {
    if (buf->is_readonly) { return false; }
    SMARTBUFFER_LEN_T oldCount = sBuffer_single_data_count(&(buf->data));
    sBuffer_single_data_add(&(buf->data), data, size);
    return sBuffer_single_data_count(&(buf->data)) - oldCount;
}
const SMARTBUFFER_CHAR * sBuffer_single_get(const sBuffer_single_ptr buf) { return sBuffer_single_data_get(&(buf->data)); }
SMARTBUFFER_LEN_T sBuffer_single_count(const sBuffer_single_ptr buf) { return sBuffer_single_data_count(&(buf->data)); }
SMARTBUFFER_LEN_T sBuffer_single_usageCount_decrease(sBuffer_single_ptr buf) {
    if (buf == NULL) { return 0; }
    if (buf->is_data_allocated || buf->is_this_allocated) {
        if (buf->usage_count > 0) { buf->usage_count--; }
    }
    return buf->usage_count;
}
SMARTBUFFER_LEN_T sBuffer_single_usageCount_increase(sBuffer_single_ptr buf) {
    if (buf == NULL) { return 0; }
    if (buf->is_data_allocated || buf->is_this_allocated) { buf->usage_count++; }
    return buf->usage_count;
}

SMARTBUFFER_LEN_T sBuffer_single_write(const sBuffer_single_ptr buf, SMARTBUFFER_LEN_T startIndex, const SMARTBUFFER_CHAR * data, SMARTBUFFER_LEN_T size) {
    if (buf->is_readonly) { return 0; }
    SIMPLE_ARRAY_WRITE(buf->data, startIndex, data, size);
    return size;
}

sBuffer_single_ptr sBuffer_single_copy(const sBuffer_single_ptr buf, SMARTBUFFER_LEN_T from, SMARTBUFFER_LEN_T to) {
    SMARTBUFFER_LEN_T len = to - from;
    sBuffer_single_ptr newBuf = sBuffer_single_create(len);
    sBuffer_single_add(newBuf, sBuffer_single_get(buf) + from, len);
    return newBuf;
}

SMARTBUFFER_BOOL_T sBuffer_single_freeData(sBuffer_single_ptr buf) {
    if (buf && buf->is_data_allocated && buf->usage_count == 0) {
        sBuffer_single_data_free(&(buf->data));
        return true;
    }
    return false;
}
void sBuffer_single_freePtr(sBuffer_single_ptr buf) {
    if (buf && buf->is_this_allocated && buf->usage_count == 0) {
        buf->is_this_allocated = false;
        SMARTBUFFER_H_FREE(buf);
    }
}

// sBuffer

sBuffer sBuffer_create(SMARTBUFFER_LEN_T size) { return (sBuffer) SIMPLE_ARRAY_CREATE_SIZE(sBuffer_single_ptr, size); }

void sBuffer_addStr(sBuffer * buf, const SMARTBUFFER_CHAR * str, SMARTBUFFER_LEN_T len) {
    sBuffer_single_ptr new_single = sBuffer_single_create(len);
    sBuffer_single_add(new_single, str, len);
    sBuffer_add(buf, new_single);
}
void sBuffer_add(sBuffer * buf, const sBuffer_single_ptr innerbuf) {
    SIMPLE_ARRAY_APPEND(*buf, innerbuf);
    sBuffer_single_usageCount_increase(innerbuf);
}
sBuffer_single_ptr sBuffer_get(const sBuffer * buf, SMARTBUFFER_LEN_T index) { return SIMPLE_ARRAY_GET(*buf, index); }
SMARTBUFFER_LEN_T sBuffer_count_single(const sBuffer * buf) { return (SMARTBUFFER_LEN_T) buf->written; }
SMARTBUFFER_LEN_T sBuffer_count(const sBuffer * buf) {
    SMARTBUFFER_LEN_T count = 0;
    // for (SMARTBUFFER_LEN_T i = 0; i < sBuffer_count_single(buf); i++) {
    SMARTBUFFER_FOREACH_P(i, current, buf,
        count += sBuffer_single_count(sBuffer_get(buf, i));
    )
    //}
    return count;
}

void sBuffer_usageCount_increaseAll(sBuffer * buf) {
    // for (SMARTBUFFER_LEN_T i = 0; i < sBuffer_count_single(buf); i++) {
        // sBuffer_single_usageCount_increase(sBuffer_get(buf, i));
    //}
    SMARTBUFFER_FOREACH_P(i, current, buf,
        sBuffer_single_usageCount_increase(current);
    )
}
void sBuffer_usageCount_decreaseAll(sBuffer * buf) {
    // for (SMARTBUFFER_LEN_T i = 0; i < sBuffer_count_single(buf); i++) {
        // sBuffer_single_usageCount_decrease(sBuffer_get(buf, i));
    // }
    SMARTBUFFER_FOREACH_P(i, current, buf,
        sBuffer_single_usageCount_decrease(sBuffer_get(buf, i));
    )
}

sBuffer sBuffer_copy(const sBuffer * buf) {
    SMARTBUFFER_LEN_T parts = sBuffer_count_single(buf);
    sBuffer ret = sBuffer_create(parts);

    /*for (SMARTBUFFER_LEN_T i = 0; i < parts; i++) {
        sBuffer_single_ptr current = sBuffer_get(buf, i);
        sBuffer_single_usageCount_increase(current);
        sBuffer_add(&ret, current);
    }*/
    SMARTBUFFER_FOREACH_P(i, current, buf,
        sBuffer_single_usageCount_increase(current);
        sBuffer_add(&ret, current);
    )

    return ret;
}
sBuffer sBuffer_copy_deep(const sBuffer * buf) {
    SMARTBUFFER_LEN_T parts = sBuffer_count_single(buf);
    sBuffer ret = sBuffer_create(parts);

    /*for (SMARTBUFFER_LEN_T i = 0; i < parts; i++) {
        const sBuffer_single_ptr current = sBuffer_get(buf, i);
        sBuffer_single_ptr newBuf = sBuffer_single_copy(current, 0, sBuffer_single_count(current));
        sBuffer_add(&ret, newBuf);
    }*/
    SMARTBUFFER_FOREACH_P(i, current, buf,
        sBuffer_single_ptr newBuf = sBuffer_single_copy(current, 0, sBuffer_single_count(current));
        sBuffer_add(&ret, newBuf);
    )

    return ret;
}

SMARTBUFFER_LEN_T sBuffer_read(const sBuffer * buf, sBuffer_readHandler reader, SMARTBUFFER_LEN_T maxlen, void * userData) {
    SMARTBUFFER_LEN_T curLen = 0;
    SMARTBUFFER_LEN_T i = 0;
    sBuffer_single_ptr curEl = NULL;
    while (curLen < maxlen) {
        curEl = sBuffer_get(buf, i);
        SMARTBUFFER_LEN_T wayToGo = maxlen - curLen;
        SMARTBUFFER_LEN_T curSize = sBuffer_single_count(curEl);
        curSize = (wayToGo > curSize) ? curSize : wayToGo;
        reader(userData, sBuffer_single_get(curEl), curSize);
        curLen += curSize;
        i++;
    }
    return curLen;
}

SMARTBUFFER_LEN_T sBuffer_write(sBuffer * buf, SMARTBUFFER_LEN_T startIndex, const SMARTBUFFER_CHAR * data, SMARTBUFFER_LEN_T size) {
    SMARTBUFFER_LEN_T index = 0;
    sBuffer_single_ptr temp = sBuffer_get(buf, index);
    for (; temp != NULL;) {
        SMARTBUFFER_LEN_T count = sBuffer_single_count(temp);
        startIndex -= count;
        if (startIndex <= 0) {
            break;
        }
        // continue / next element
        index++;
        temp = sBuffer_get(buf, index);
    }
    startIndex = startIndex * -1;
    if (temp == NULL) {
        return 0;
    } else if (startIndex == 0) {
        sBuffer_addStr(buf, data, size);
        return sBuffer_get(buf, index)->data.written;
    } else {
        sBuffer_single_write(temp, startIndex, data, size);
    }
    return 0;
}

sBuffer_single_ptr sBuffer_join(const sBuffer * toJoin) {
    sBuffer_single_ptr ret = sBuffer_single_create(sBuffer_count(toJoin));

    // for (SMARTBUFFER_LEN_T i = 0; i < sBuffer_single_count(ret); i++) {
        // sBuffer_single_ptr current = sBuffer_get(toJoin, i);
    SMARTBUFFER_FOREACH_P(i, current, toJoin,
        SMARTBUFFER_LEN_T toWrite = sBuffer_single_count(current);
        SMARTBUFFER_LEN_T written = sBuffer_single_add(ret, sBuffer_single_get(current), toWrite);
        if (toWrite != written) {
            return NULL;
        }
    )
    // }

    return ret;
}

void sBuffer_free(sBuffer * buf) {
    SMARTBUFFER_LEN_T len = sBuffer_count_single(buf);
    // for (SMARTBUFFER_LEN_T i = 0; i < len; i++) {
        // sBuffer_single_ptr ptr = sBuffer_get(buf, i);
    SMARTBUFFER_FOREACH_P(i, ptr, buf,
        sBuffer_single_usageCount_decrease(ptr);
        sBuffer_single_freeData(ptr);
        sBuffer_single_freePtr(ptr);
    // }
    )
    SIMPLE_ARRAY_FREE((*buf));
}