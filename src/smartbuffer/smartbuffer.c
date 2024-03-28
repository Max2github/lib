#include "../../smartbuffer.h"
#include "smartbuffer_intern.h"
#include <assert.h>


// sBuffer

sBuffer sBuffer_create(SMARTBUFFER_LEN_T size) { return (sBuffer) SIMPLE_ARRAY_CREATE_SIZE(sBuffer_single_ptr, size); }

SMARTBUFFER_LEN_T sBuffer_append(sBuffer * buf, const SMARTBUFFER_CHAR * data, SMARTBUFFER_LEN_T len) {
    const SMARTBUFFER_LEN_T countInnerBufs = sBuffer_count_single(buf);
    if (countInnerBufs == 0) {
        sBuffer_single_ptr new_single = sBuffer_single_create(len);
        new_single = sBuffer_single_add(new_single, data, len);
        sBuffer_add(buf, new_single);
        return sBuffer_single_count(new_single);
    }
    sBuffer_single_ptr last = sBuffer_get(buf, countInnerBufs - 1);
    if (last->flags.is_readonly || sBuffer_single_count_remaining(last) == 0) {
        sBuffer_single_ptr new_single = sBuffer_single_create(len);
        new_single = sBuffer_single_add(new_single, data, len);
        sBuffer_add(buf, new_single);
        return sBuffer_single_count(new_single);
    }
    const SMARTBUFFER_LEN_T remaining = sBuffer_single_count_remaining(last);
    const SMARTBUFFER_LEN_T last_lenBefore = sBuffer_single_count(last);
    if (remaining >= len) {
        last = sBuffer_single_add(last, data, len);
        // TODO: assert that the address of last stays the same
        return sBuffer_single_count(last) - last_lenBefore;
    } else {
        last = sBuffer_single_add(last, data, remaining); // fill last buffer
        // TODO: assert that the address of last stays the same
        SMARTBUFFER_LEN_T written = sBuffer_single_count(last) - last_lenBefore;

        const SMARTBUFFER_CHAR * restData = data + remaining;
        SMARTBUFFER_LEN_T restLen = len - remaining;
        sBuffer_single_ptr new_single = sBuffer_single_create(restLen);
        new_single = sBuffer_single_add(new_single, restData, restLen);
        written += sBuffer_single_count(new_single);
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

SMARTBUFFER_LEN_T sBuffer_write_helper(sBuffer * buf, sBuffer_index_descr found, const SMARTBUFFER_CHAR * data, SMARTBUFFER_LEN_T size) {
    // check if buffer is big enough, else write into the next
    const SMARTBUFFER_LEN_T space = (*found.single.bufP)->own.allocated - found.single.index;
    const SMARTBUFFER_LEN_T lenBefore = sBuffer_single_count(*found.single.bufP);
    if (size > space) {
        *found.single.bufP = sBuffer_single_write(*found.single.bufP, found.single.index, data, space); // fill the current buffer
        const SMARTBUFFER_LEN_T written = sBuffer_single_count(*found.single.bufP) - lenBefore;

        // write the rest into the next buffer (overwrite)
        const SMARTBUFFER_CHAR * restData = data + space;
        const SMARTBUFFER_LEN_T restLen = size - space;

        const SMARTBUFFER_LEN_T newIndex = found.index + 1;
        if (newIndex < sBuffer_count_single(buf)) {
            const sBuffer_single_with_index next = (sBuffer_single_with_index) { sBuffer_getP(buf, newIndex), 0 };
            const sBuffer_index_descr descr = { next, newIndex };
            return written + sBuffer_write_helper(buf, descr, restData, restLen);
        } else {
            return written + sBuffer_append(buf, restData, restLen);
        }
    } else {
        *found.single.bufP = sBuffer_single_write(*found.single.bufP, found.single.index, data, size);
        return sBuffer_single_count(*found.single.bufP) - lenBefore;
    }
}

SMARTBUFFER_LEN_T sBuffer_insert_helper(sBuffer * buf, sBuffer_index_descr found, const SMARTBUFFER_CHAR * data, SMARTBUFFER_LEN_T size) {
    // if we insert between two sBuffer_single, we just insert a new sBuffer_single
    if (found.single.index == 0 /* || found.single.index == sBuffer_single_count(found.single.buf)*/) {
        sBuffer_single_ptr newbuf = sBuffer_single_create(size);
        newbuf = sBuffer_single_add(newbuf, data, size);
        sBuffer_insert_single(buf, found.index, newbuf);
        return sBuffer_single_count(newbuf);
    }
    // else insert into the sBuffer_single
    // TODO: In the future the buffer may be split (splitting must be implemented) and a new sBuffer_single can be inserted
    //return sBuffer_single_insert(found.single.buf, found.single.index, data, size);
    sBuffer_single_ptr part1 = sBuffer_single_create_child(*found.single.bufP, 0, found.single.index);
    sBuffer_single_ptr part2 = sBuffer_single_create_child(*found.single.bufP, found.single.index, sBuffer_single_count(*found.single.bufP) - found.single.index);
    sBuffer_remove_single(buf, *found.single.bufP);

    sBuffer_single_ptr newbuf = sBuffer_single_create(size);
    newbuf = sBuffer_single_add(newbuf, data, size);

    sBuffer_insert_single(buf, found.index, part1);
    sBuffer_insert_single(buf, found.index+1, newbuf);
    sBuffer_insert_single(buf, found.index+2, part2);

    //sBuffer_single_free(part1);
    //sBuffer_single_free(part2);
    return sBuffer_single_count(newbuf);
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
        ret = sBuffer_single_add(ret, sBuffer_single_get(current), toWrite);
    )

    return ret;
}

void sBuffer_clear(sBuffer * buf) {
    SMARTBUFFER_FOREACH_P(i, ptr, buf,
        if (ptr->flags.is_readonly || ptr->own.usage_count > 1) {
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