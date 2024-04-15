#include "../../smartbuffer.h"
#include "smartbuffer_intern.h"
#include <assert.h>


// sBuffer

// intern

sBuffer_single_ptr sBuffer_intern_create_single(const SMARTBUFFER_CHAR * data, SMARTBUFFER_LEN_T len, SMARTBUFFER_LEN_T * outWritten) {
    sBuffer_single_ptr newBuf = sBuffer_single_create_once(len);
    const SMARTBUFFER_LEN_T written = sBuffer_single_add(newBuf, data, len);
    if (outWritten) { *outWritten = written; }
    return newBuf;
}

SMARTBUFFER_LEN_T sBuffer_intern_add_new_single(sBuffer * buf, const SMARTBUFFER_CHAR * data, SMARTBUFFER_LEN_T len) {
    SMARTBUFFER_LEN_T written = 0;
    sBuffer_add(buf, sBuffer_intern_create_single(data, len, &written));
    return written;
}

SMARTBUFFER_LEN_T sBuffer_intern_insert_new_single(sBuffer * buf, SMARTBUFFER_LEN_T index, const SMARTBUFFER_CHAR * data, SMARTBUFFER_LEN_T len) {
    SMARTBUFFER_LEN_T written = 0;
    sBuffer_insert_single(buf, index, sBuffer_intern_create_single(data, len, &written));
    return written;
}

void sBuffer_intern_replace_single(sBuffer_single_ptr * dest, sBuffer_single_ptr src) {
    sBuffer_single_free(*dest);
    sBuffer_single_usageCount_increase(src);
    *dest = src;
}

SMARTBUFFER_LEN_T sBuffer_intern_replace_new_single(sBuffer_single_ptr * dest, const SMARTBUFFER_CHAR * data, SMARTBUFFER_LEN_T len) {
    SMARTBUFFER_LEN_T written = 0;
    sBuffer_single_ptr newBuf = sBuffer_intern_create_single(data, len, &written);
    sBuffer_intern_replace_single(dest, newBuf);
    return written;
}

// replace a sBuffer_single, described by current by another buffer and two other sBuffer_single (one before, the other after)
void sBuffer_intern_split_replace(sBuffer * buf, sBuffer_index_descr current, sBuffer_single_ptr toInsert, sBuffer_single_ptr before, sBuffer_single_ptr after) {
    SMARTBUFFER_LEN_T toShift = 0;
    if (before) {
        toShift++;
    }
    if(after) {
        toShift++;
    }

    if (toShift == 0) {
        return sBuffer_intern_replace_single(current.single.bufP, toInsert);
    }

    sBuffer_single_free(*current.single.bufP); // not needed anymore

    // shift the buffer & insert the buf children + the new one in the correct order
    sBuffer_shift_right(buf, current.index, toShift);
    if (before) { sBuffer_insert_single_noShift(buf, current.index, before); }
    sBuffer_insert_single_noShift(buf, current.index+1, toInsert);
    if (after) { sBuffer_insert_single_noShift(buf, current.index+2, after); }
}

// extern

sBuffer sBuffer_create(SMARTBUFFER_LEN_T size) { return (sBuffer) SIMPLE_ARRAY_CREATE_SIZE(sBuffer_single_ptr, size); }

SMARTBUFFER_LEN_T sBuffer_append(sBuffer * buf, const SMARTBUFFER_CHAR * data, SMARTBUFFER_LEN_T len) {
    const SMARTBUFFER_LEN_T countInnerBufs = sBuffer_count_single(buf);
    if (countInnerBufs == 0) {
        return sBuffer_intern_add_new_single(buf, data, len);
    }
    sBuffer_single_ptr last = sBuffer_get(buf, countInnerBufs - 1);
    // if last is readonly, or if it does not only belong this, or if it has no space left
    if (last->flags.is_readonly || sBuffer_single_usageCount_get(last) > 1 || sBuffer_single_count_remaining(last) == 0) {
        return sBuffer_intern_add_new_single(buf, data, len);
    }
    // else write as much as possible into last
    const SMARTBUFFER_LEN_T remaining = sBuffer_single_count_remaining(last);
    if (remaining >= len) {
        // write everything into last
        return sBuffer_single_add(last, data, len);
    } else {
        // write as much as possible into last and the rest in an own buffer
        SMARTBUFFER_LEN_T written = sBuffer_single_add(last, data, remaining); // fill last buffer

        const SMARTBUFFER_CHAR * restData = data + remaining;
        SMARTBUFFER_LEN_T restLen = len - remaining;
        written += sBuffer_intern_add_new_single(buf, restData, restLen);

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
    sBuffer_single_usageCount_increase(innerbuf);
}

void sBuffer_insert_single(sBuffer * buf, SMARTBUFFER_LEN_T index, sBuffer_single_ptr innerbuf) {
    sBuffer_shift_right(buf, index, 1);
    sBuffer_insert_single_noShift(buf, index, innerbuf);
}

sBuffer_single_ptr sBuffer_get(const sBuffer * buf, SMARTBUFFER_LEN_T index) { return SIMPLE_ARRAY_GET(*buf, index); }
sBuffer_single_ptr * sBuffer_getP(const sBuffer * buf, SMARTBUFFER_LEN_T index) { return &SIMPLE_ARRAY_GET(*buf, index); }

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
    const SMARTBUFFER_LEN_T maxbufs = sBuffer_count_single(buf);
    while (curLen < maxlen && i < maxbufs) {
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

SMARTBUFFER_LEN_T sBuffer_read_to(SMARTBUFFER_CHAR * dest, const sBuffer * buf, SMARTBUFFER_LEN_T maxlen) {
    SMARTBUFFER_LEN_T written = 0;
    SMARTBUFFER_LEN_T i = 0;
    const SMARTBUFFER_LEN_T maxbufs = sBuffer_count_single(buf);
    while (written < maxlen && i < maxbufs) {
        const sBuffer_single_ptr curEl = sBuffer_get(buf, i);
        SMARTBUFFER_LEN_T curLen = sBuffer_single_count(curEl);
        curLen = curLen < maxlen ? curLen : maxlen;
        SMARTBUFFER_H_MEMCOPY(dest, sBuffer_single_get(curEl), curLen);
        written += curLen;
        i++;
    }
    return written;
}

sBuffer_index_descr sBuffer_find_index(const sBuffer * buf, SMARTBUFFER_LEN_T searchIndex) {
    SMARTBUFFER_LEN_T index = 0;
    sBuffer_single_ptr * temp = sBuffer_getP(buf, index);
    for (; temp != NULL;) {
        SMARTBUFFER_LEN_T count = sBuffer_single_count(*temp);
        if (searchIndex < count) {
            break;
        }
        searchIndex -= count;

        // continue / next element
        index++;
        temp = sBuffer_getP(buf, index);
    }
    const sBuffer_single_with_index found = { temp, searchIndex };
    return (sBuffer_index_descr) { found, index };
}

SMARTBUFFER_LEN_T sBuffer_write_helper(sBuffer * buf, sBuffer_index_descr found, const SMARTBUFFER_CHAR * data, SMARTBUFFER_LEN_T len) {
    sBuffer_single_ptr curBuf = *found.single.bufP;

    // if curBuf is readonly or if it does not only belong this -> we do not have write permission
    if (curBuf->flags.is_readonly || sBuffer_single_usageCount_get(curBuf) > 1) {
        sBuffer_single_ptr partBefore = NULL;
        sBuffer_single_ptr partAfter = NULL;
        if (found.single.index > 0) {
            partBefore = sBuffer_single_create_child(curBuf, 0, found.single.index);
        }
        const SMARTBUFFER_LEN_T endIndex = found.single.index + len;
        if(endIndex < sBuffer_single_size(curBuf)) {
            partAfter = sBuffer_single_create_child(curBuf, endIndex, sBuffer_single_count(curBuf));
        }
        SMARTBUFFER_LEN_T written = 0;
        sBuffer_single_ptr newBuf = sBuffer_intern_create_single(data, len, &written);
        sBuffer_intern_split_replace(buf, found, newBuf, partBefore, partAfter);
        return written;

    // else we have write permission
    } else {
        // check if buffer is big enough, else write into the next
        const SMARTBUFFER_LEN_T space = sBuffer_single_size(curBuf) - found.single.index;
        if (len > space) {
            // write as much as possible into curBuf
            const SMARTBUFFER_LEN_T written = sBuffer_single_write(curBuf, found.single.index, data, space); // fill the current buffer

            // write the rest into the next buffer (overwrite)
            const SMARTBUFFER_CHAR * restData = data + space;
            const SMARTBUFFER_LEN_T restLen = len - space;

            const SMARTBUFFER_LEN_T newIndex = found.index + 1;
            if (newIndex < sBuffer_count_single(buf)) {
                const sBuffer_single_with_index next = (sBuffer_single_with_index) { sBuffer_getP(buf, newIndex), 0 };
                const sBuffer_index_descr descr = { next, newIndex };
                return written + sBuffer_write_helper(buf, descr, restData, restLen);
            } else {
                return written + sBuffer_append(buf, restData, restLen);
            }
        } else {
            return sBuffer_single_write(curBuf, found.single.index, data, len);
        }
    }
}

SMARTBUFFER_LEN_T sBuffer_insert_helper(sBuffer * buf, sBuffer_index_descr found, const SMARTBUFFER_CHAR * data, SMARTBUFFER_LEN_T len) {
    // if we insert between two sBuffer_single, we just insert a new sBuffer_single
    if (found.single.index == 0 /*|| found.single.index == sBuffer_single_count(*found.single.bufP)*/) {
        return sBuffer_intern_insert_new_single(buf, found.index, data, len);
    }
    // else split the sBuffer_single into children and insert between them
    sBuffer_single_ptr curBuf = *found.single.bufP;
    sBuffer_single_ptr part1 = sBuffer_single_create_child(curBuf, 0, found.single.index);
    sBuffer_single_ptr part2 = sBuffer_single_create_child(curBuf, found.single.index, sBuffer_single_count(curBuf) - found.single.index);

    SMARTBUFFER_LEN_T written = 0;
    sBuffer_single_ptr newbuf = sBuffer_intern_create_single(data, len, &written);
    sBuffer_intern_split_replace(buf, found, newbuf, part1, part2);

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
        sBuffer_single_add(ret, sBuffer_single_get(current), toWrite);
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