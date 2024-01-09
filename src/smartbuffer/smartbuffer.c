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
void sBuffer_single_data_shift_right(sBuffer_single_data * buf, SMARTBUFFER_LEN_T index, SMARTBUFFER_LEN_T amount) { SIMPLE_ARRAY_SHIFT_RIGHT(*buf, index, amount); }
SMARTBUFFER_CHAR * sBuffer_single_data_get(const sBuffer_single_data * buf) { return (SMARTBUFFER_CHAR *) buf->data; }
SMARTBUFFER_LEN_T sBuffer_single_data_count(const sBuffer_single_data * buf) { return buf->written; }
SMARTBUFFER_LEN_T sBuffer_single_data_count_remaining(const sBuffer_single_data * buf) {
    if (buf->count <= buf->written) { return 0; }
    return buf->count - buf->written;
}
void sBuffer_single_data_free(sBuffer_single_data * buf) { SIMPLE_ARRAY_FREE(*buf); }

// sBuffer_single

sBuffer_single sBuffer_single_create_intern(SMARTBUFFER_LEN_T size, SMARTBUFFER_BOOL_T readonly, SMARTBUFFER_BOOL_T allocated) {
    sBuffer_single neu;
    neu.data = sBuffer_single_data_create(size);
    neu.flags.is_child = SMARTBUFFER_FALSE;
    neu.flags.is_readonly = readonly;
    neu.flags.is_data_allocated = allocated;
    neu.usage_count = 0;
    return neu;
}

sBuffer_single sBuffer_single_create_static_intern(const SMARTBUFFER_CHAR * buf, SMARTBUFFER_LEN_T size) {
    sBuffer_single neu;
    neu.data = sBuffer_single_data_create_static((SMARTBUFFER_CHAR *) buf, size);
    neu.flags.is_child = SMARTBUFFER_FALSE;
    neu.flags.is_readonly = SMARTBUFFER_TRUE;
    neu.flags.is_data_allocated = SMARTBUFFER_FALSE;
    neu.usage_count = 0;
    return neu;
}

sBuffer_single_ptr sBuffer_single_create_outer(sBuffer_single data) {
    sBuffer_single_ptr new_ptr = (sBuffer_single_ptr) SMARTBUFFER_H_MALLOC(sizeof(sBuffer_single));
    *new_ptr = data;
    new_ptr->flags.is_this_allocated = true;
    return new_ptr;
}

// exported

sBuffer_single_ptr sBuffer_single_create_static(const SMARTBUFFER_CHAR * buf, SMARTBUFFER_LEN_T len) { return sBuffer_single_create_outer(sBuffer_single_create_static_intern(buf, len)); }
sBuffer_single_ptr sBuffer_single_create(SMARTBUFFER_LEN_T size) { return sBuffer_single_create_outer(sBuffer_single_create_intern(size, SMARTBUFFER_FALSE, SMARTBUFFER_TRUE)); }

sBuffer_single_ptr sBuffer_single_create_child(const sBuffer_single_ptr buf, SMARTBUFFER_LEN_T begin, SMARTBUFFER_LEN_T size) {
    sBuffer_single neu;
    neu.child.parent = buf;
    neu.child.begin = begin;
    neu.child.size = size;
    neu.flags.is_child = SMARTBUFFER_TRUE;
    neu.flags.is_readonly = SMARTBUFFER_TRUE;
    neu.flags.is_data_allocated = SMARTBUFFER_FALSE;
    sBuffer_single_usageCount_increase((sBuffer_single_ptr) buf);
    return sBuffer_single_create_outer(neu);
}

SMARTBUFFER_BOOL_T sBuffer_single_add_char(sBuffer_single * buf, const SMARTBUFFER_CHAR c) {
    if (buf->flags.is_readonly) { return false; }
    SMARTBUFFER_LEN_T oldCount = sBuffer_single_data_count(&(buf->data));
    sBuffer_single_data_add_char(&(buf->data), c);
    return (sBuffer_single_data_count(&(buf->data)) == oldCount + 1);
}
SMARTBUFFER_LEN_T sBuffer_single_add(sBuffer_single_ptr buf, const SMARTBUFFER_CHAR * data, SMARTBUFFER_LEN_T size) {
    if (buf->flags.is_readonly) { return 0; }
    SMARTBUFFER_LEN_T oldCount = sBuffer_single_data_count(&(buf->data));
    sBuffer_single_data_add(&(buf->data), data, size);
    return sBuffer_single_data_count(&(buf->data)) - oldCount;
}

SMARTBUFFER_LEN_T sBuffer_single_insert(sBuffer_single_ptr buf, SMARTBUFFER_LEN_T index, const SMARTBUFFER_CHAR * data, SMARTBUFFER_LEN_T size) {
    if (buf->flags.is_readonly) { return 0; }
    if (index < sBuffer_single_count(buf)) {
        sBuffer_single_shift_right(buf, index, size);
    }
    return sBuffer_single_write(buf, index, data, size);
}

const SMARTBUFFER_CHAR * sBuffer_single_get(const sBuffer_single_ptr buf) {
    if (buf->flags.is_child) {
        return sBuffer_single_get(buf->child.parent) + buf->child.begin;
    }
    return sBuffer_single_data_get(&(buf->data));
}

SMARTBUFFER_LEN_T sBuffer_single_count(const sBuffer_single_ptr buf) {
    if (buf->flags.is_child) {
        return buf->child.size;
    }
    return sBuffer_single_data_count(&(buf->data));
}

SMARTBUFFER_BOOL_T sBuffer_single_is_empty(const sBuffer_single_ptr buf) { return (sBuffer_single_count(buf) == 0); }
SMARTBUFFER_LEN_T sBuffer_single_count_remaining(const sBuffer_single_ptr buf) {
    if (buf->flags.is_child) { return 0; }
    return sBuffer_single_data_count_remaining(&(buf->data));
}

SMARTBUFFER_LEN_T sBuffer_single_usageCount_decrease(sBuffer_single_ptr buf) {
    if (buf == NULL) { return 0; }
    if (buf->flags.is_child) {
        return sBuffer_single_usageCount_decrease(buf->child.parent);
    }
    if (buf->flags.is_data_allocated || buf->flags.is_this_allocated) {
        if (buf->usage_count > 0) { buf->usage_count--; }
    }
    return buf->usage_count;
}
SMARTBUFFER_LEN_T sBuffer_single_usageCount_increase(sBuffer_single_ptr buf) {
    if (buf == NULL) { return 0; }
    if (buf->flags.is_child) {
        return sBuffer_single_usageCount_increase(buf->child.parent);
    }
    if (buf->flags.is_data_allocated || buf->flags.is_this_allocated) { buf->usage_count++; }
    return buf->usage_count;
}

SMARTBUFFER_LEN_T sBuffer_single_shift_right(sBuffer_single_ptr buf, SMARTBUFFER_LEN_T index, SMARTBUFFER_LEN_T amount) {
    if (buf->flags.is_readonly) { return 0; }
    sBuffer_single_data_shift_right(&(buf->data), index, amount);
    if (index + amount > sBuffer_single_count(buf)) {
        return (index + amount) - sBuffer_single_count(buf);
    }
    return amount;
}

SMARTBUFFER_LEN_T sBuffer_single_write(sBuffer_single_ptr buf, SMARTBUFFER_LEN_T startIndex, const SMARTBUFFER_CHAR * data, SMARTBUFFER_LEN_T size) {
    if (buf->flags.is_readonly) { return 0; }
    SIMPLE_ARRAY_WRITE(buf->data, startIndex, data, size);
    return size;
}

sBuffer_single_ptr sBuffer_single_copy(const sBuffer_single_ptr buf, SMARTBUFFER_LEN_T from, SMARTBUFFER_LEN_T to) {
    SMARTBUFFER_LEN_T len = to - from;
    if (sBuffer_single_count(buf) < len) { return NULL; }
    sBuffer_single_ptr newBuf = sBuffer_single_create(len);
    sBuffer_single_add(newBuf, sBuffer_single_get(buf) + from, len);
    return newBuf;
}

SMARTBUFFER_LEN_T sBuffer_single_clear(sBuffer_single_ptr buf) {
    if (buf->flags.is_readonly) { return 0; }
    const SMARTBUFFER_LEN_T size = sBuffer_single_count(buf);
    buf->data.written = 0;
    return size;
}

SMARTBUFFER_BOOL_T sBuffer_single_freeData(sBuffer_single_ptr buf) {
    if (buf && buf->flags.is_child && buf->child.parent->usage_count == 0) {
        sBuffer_single_free(buf->child.parent);
        return true;
    }
    if (buf && buf->flags.is_data_allocated && buf->usage_count == 0) {
        sBuffer_single_data_free(&(buf->data));
        return true;
    }
    return false;
}
void sBuffer_single_freePtr(sBuffer_single_ptr buf) {
    if (buf && buf->flags.is_this_allocated && buf->usage_count == 0) {
        buf->flags.is_this_allocated = false;
        SMARTBUFFER_H_FREE(buf);
    }
}

void sBuffer_single_free(sBuffer_single_ptr buf) {
    sBuffer_single_usageCount_decrease(buf);
    sBuffer_single_freeData(buf);
    sBuffer_single_freePtr(buf);
}

// sBuffer

sBuffer sBuffer_create(SMARTBUFFER_LEN_T size) { return (sBuffer) SIMPLE_ARRAY_CREATE_SIZE(sBuffer_single_ptr, size); }

SMARTBUFFER_LEN_T sBuffer_append(sBuffer * buf, const SMARTBUFFER_CHAR * data, SMARTBUFFER_LEN_T len) {
    const SMARTBUFFER_LEN_T countInnerBufs = sBuffer_count_single(buf);
    if (countInnerBufs == 0) {
        sBuffer_single_ptr new_single = sBuffer_single_create(len);
        SMARTBUFFER_LEN_T written = sBuffer_single_add(new_single, data, len);
        sBuffer_add(buf, new_single);
        return written;
    }
    sBuffer_single_ptr last = sBuffer_get(buf, countInnerBufs - 1);
    if (last->flags.is_readonly || sBuffer_single_count_remaining(last) == 0) {
        sBuffer_single_ptr new_single = sBuffer_single_create(len);
        SMARTBUFFER_LEN_T written = sBuffer_single_add(new_single, data, len);
        sBuffer_add(buf, new_single);
        return written;
    }
    SMARTBUFFER_LEN_T remaining = sBuffer_single_count_remaining(last);
    if (remaining >= len) {
        return sBuffer_single_add(last, data, len);
    } else {
        SMARTBUFFER_LEN_T written = sBuffer_single_add(last, data, remaining); // fill last buffer

        const SMARTBUFFER_CHAR * restData = data + remaining;
        SMARTBUFFER_LEN_T restLen = len - remaining;
        sBuffer_single_ptr new_single = sBuffer_single_create(restLen);
        written += sBuffer_single_add(new_single, restData, restLen);
        sBuffer_add(buf, new_single);
        return written;
    }
}
void sBuffer_add(sBuffer * buf, const sBuffer_single_ptr innerbuf) {
    SIMPLE_ARRAY_APPEND(*buf, innerbuf);
    sBuffer_single_usageCount_increase(innerbuf);
}

void sBuffer_shift_right(sBuffer * buf, SMARTBUFFER_LEN_T index, SMARTBUFFER_LEN_T amount) {
    SIMPLE_ARRAY_SHIFT_RIGHT(*buf, index, amount);
}

void sBuffer_insert_single_noShift(sBuffer * buf, SMARTBUFFER_LEN_T index, sBuffer_single_ptr innerbuf) {
    SIMPLE_ARRAY_GET(*buf, index) = innerbuf;
}

void sBuffer_insert_single(sBuffer * buf, SMARTBUFFER_LEN_T index, sBuffer_single_ptr innerbuf) {
    sBuffer_shift_right(buf, index, 1);
    sBuffer_insert_single_noShift(buf, index, innerbuf);
}

sBuffer_single_ptr sBuffer_get(const sBuffer * buf, SMARTBUFFER_LEN_T index) { return SIMPLE_ARRAY_GET(*buf, index); }

SMARTBUFFER_LEN_T sBuffer_remove_single(sBuffer * buf, const sBuffer_single_ptr toRemove) {
    SMARTBUFFER_LEN_T len = sBuffer_count_single(buf);
    SMARTBUFFER_LEN_T removed = 0;
    SMARTBUFFER_FOREACH_P(i, current, buf,
        if (current == toRemove) {
            sBuffer_single_free(current);
            if (i+1 < len) {
                // advance the bufers afterwards
                for (SMARTBUFFER_LEN_T n = i+1; n < len; n++) {
                    SIMPLE_ARRAY_GET(*buf, n-1) = SIMPLE_ARRAY_GET(*buf, n);
                }
            }
            removed++;
        }
    )
    buf->written -= removed;
    return removed;
}

SMARTBUFFER_LEN_T sBuffer_count_single(const sBuffer * buf) { return (SMARTBUFFER_LEN_T) buf->written; }
SMARTBUFFER_LEN_T sBuffer_count(const sBuffer * buf) {
    SMARTBUFFER_LEN_T count = 0;
    SMARTBUFFER_FOR_P(i, buf,
        count += sBuffer_single_count(sBuffer_get(buf, i));
    )
    return count;
}

void sBuffer_usageCount_increaseAll(sBuffer * buf) {
    SMARTBUFFER_FOREACH_P(i, current, buf,
        sBuffer_single_usageCount_increase(current);
    )
}
void sBuffer_usageCount_decreaseAll(sBuffer * buf) {
    SMARTBUFFER_FOREACH_P(i, current, buf,
        sBuffer_single_usageCount_decrease(current);
    )
}

sBuffer sBuffer_copy(const sBuffer * buf) {
    SMARTBUFFER_LEN_T parts = sBuffer_count_single(buf);
    sBuffer ret = sBuffer_create(parts);

    SMARTBUFFER_FOREACH_P(i, current, buf,
        sBuffer_add(&ret, current);
    )

    return ret;
}
sBuffer sBuffer_copy_deep(const sBuffer * buf) {
    SMARTBUFFER_LEN_T parts = sBuffer_count_single(buf);
    sBuffer ret = sBuffer_create(parts);

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

sBuffer_index_descr sBuffer_find_index(const sBuffer * buf, SMARTBUFFER_LEN_T searchIndex) {
    SMARTBUFFER_LEN_T index = 0;
    sBuffer_single_ptr temp = sBuffer_get(buf, index);
    for (; temp != NULL;) {
        SMARTBUFFER_LEN_T count = sBuffer_single_count(temp);
        if (searchIndex < count) {
            break;
        }
        searchIndex -= count;

        // continue / next element
        index++;
        temp = sBuffer_get(buf, index);
    }
    const sBuffer_single_with_index found = { temp, searchIndex };
    return (sBuffer_index_descr) { found, index };
}

SMARTBUFFER_LEN_T sBuffer_write_helper(sBuffer * buf, sBuffer_index_descr found, const SMARTBUFFER_CHAR * data, SMARTBUFFER_LEN_T size) {
    // check if buffer is big enough, else write into the next
    const SMARTBUFFER_LEN_T space = found.single.buf->data.count - found.single.index;
    if (size > space) {
        const SMARTBUFFER_LEN_T written = sBuffer_single_write(found.single.buf, found.single.index, data, space); // fill the current buffer

        // write the rest into the next buffer (overwrite)
        const SMARTBUFFER_CHAR * restData = data + space;
        const SMARTBUFFER_LEN_T restLen = size - space;

        const SMARTBUFFER_LEN_T newIndex = found.index + 1;
        if (newIndex < sBuffer_count_single(buf)) {
            const sBuffer_single_with_index next = (sBuffer_single_with_index) { sBuffer_get(buf, newIndex), 0 };
            const sBuffer_index_descr descr = { next, newIndex };
            return written + sBuffer_write_helper(buf, descr, restData, restLen);
        } else {
            return written + sBuffer_append(buf, restData, restLen);
        }
    } else {
        return sBuffer_single_write(found.single.buf, found.single.index, data, size);
    }
}

SMARTBUFFER_LEN_T sBuffer_insert_helper(sBuffer * buf, sBuffer_index_descr found, const SMARTBUFFER_CHAR * data, SMARTBUFFER_LEN_T size) {
    // if we insert between two sBuffer_single, we just insert a new sBuffer_single
    if (found.single.index == 0 /* || found.single.index == sBuffer_single_count(found.single.buf)*/) {
        sBuffer_single_ptr newbuf = sBuffer_single_create(size);
        SMARTBUFFER_LEN_T written = sBuffer_single_add(newbuf, data, size);
        sBuffer_insert_single(buf, found.index, newbuf);
        return written;
    }
    // else insert into the sBuffer_single
    // TODO: In the future the buffer may be split (splitting must be implemented) and a new sBuffer_single can be inserted
    //return sBuffer_single_insert(found.single.buf, found.single.index, data, size);
    sBuffer_single_ptr part1 = sBuffer_single_create_child(found.single.buf, 0, found.single.index);
    sBuffer_single_ptr part2 = sBuffer_single_create_child(found.single.buf, found.single.index, sBuffer_single_count(found.single.buf) - found.single.index);
    sBuffer_remove_single(buf, found.single.buf);

    sBuffer_single_ptr newbuf = sBuffer_single_create(size);
    SMARTBUFFER_LEN_T written = sBuffer_single_add(newbuf, data, size);

    sBuffer_insert_single(buf, found.index, part1);
    sBuffer_insert_single(buf, found.index+1, newbuf);
    sBuffer_insert_single(buf, found.index+2, part2);

    //sBuffer_single_free(part1);
    //sBuffer_single_free(part2);
    return written;
}

SMARTBUFFER_LEN_T sBuffer_write(sBuffer * buf, SMARTBUFFER_LEN_T startIndex, const SMARTBUFFER_CHAR * data, SMARTBUFFER_LEN_T size) {
    // if startIndex is at the end -> append
    if (startIndex >= sBuffer_count(buf)) {
        return sBuffer_append(buf, data, size);
    }
    sBuffer_index_descr found = sBuffer_find_index(buf, startIndex);
    return sBuffer_write_helper(buf, found, data, size);
}

SMARTBUFFER_LEN_T sBuffer_insert(sBuffer * buf, SMARTBUFFER_LEN_T startIndex, const SMARTBUFFER_CHAR * data, SMARTBUFFER_LEN_T size) {
    // if startIndex is at the end -> append
    if (startIndex >= sBuffer_count(buf)) {
        return sBuffer_append(buf, data, size);
    }
    sBuffer_index_descr found = sBuffer_find_index(buf, startIndex);
    return sBuffer_insert_helper(buf, found, data, size);
}

sBuffer_single_ptr sBuffer_join(const sBuffer * toJoin) {
    sBuffer_single_ptr ret = sBuffer_single_create(sBuffer_count(toJoin));

    SMARTBUFFER_FOREACH_P(i, current, toJoin,
        SMARTBUFFER_LEN_T toWrite = sBuffer_single_count(current);
        SMARTBUFFER_LEN_T written = sBuffer_single_add(ret, sBuffer_single_get(current), toWrite);
        if (toWrite != written) {
            return NULL;
        }
    )

    return ret;
}

void sBuffer_clear(sBuffer * buf) {
    SMARTBUFFER_FOREACH_P(i, ptr, buf,
        if (ptr->flags.is_readonly || ptr->usage_count > 1) {
            SMARTBUFFER_LEN_T removed = sBuffer_remove_single(buf, ptr);
            i -= removed;
        } else {
            sBuffer_single_clear(ptr);
        }
    )
}

void sBuffer_free(sBuffer * buf) {
    SMARTBUFFER_FOREACH_P(i, ptr, buf,
        sBuffer_single_free(ptr);
    )
    SIMPLE_ARRAY_FREE((*buf));
}