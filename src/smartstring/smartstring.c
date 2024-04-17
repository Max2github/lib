#include "../../smartstring.h"

#include "../../freestanding/byte.h"
//#include "../../words_h.h"
#include "../../word_pick_h.h"

sString sString_create() {
    sString s;
    s.buf = sBuffer_create(1);
    s.flags = SMARTSTRING_FLAG_U_ZERO;
    return s;
}
sString sString_init(sString_char_t * str, sString_len_t len, sString_flags_u flags) {
    sString s = sString_create();

    sBuffer_single_ptr newInner = NULL;

    if (flags.obj.is_static) {
        newInner = sBuffer_single_create_static(str, len);
    } else {
        newInner = sBuffer_single_create(len);
        sBuffer_single_add(newInner, str, len);
        newInner->flags.is_readonly = flags.obj.is_const;
    }

    sBuffer_add(&(s.buf), newInner);
    // sBuffer_addStr(&(s.buf), str, len);
    s.flags = flags;
    return s;
}

void sString_free(sString * str) { return sBuffer_free(SSTRING_GETBUF_ADDR_P(str)); }

sString_len_t sString_count(const sString * s) { return sBuffer_count_single(&(s->buf)); }
sString_len_t sString_len(const sString * s) { return sBuffer_count(&(s->buf)); }

void sString_addSingleBuf(sString * s, sBuffer_single_ptr buf) {
    sBuffer_add(&(s->buf), buf);
}
void sString_add(sString * s, sString other) {
    if (s->flags.obj.is_const) { return; }
    if (s->flags.obj.on_add_join) {
        sBuffer_single_ptr last = sBuffer_get(&(s->buf), sBuffer_count_single(&(s->buf)) - 1);

        SMARTBUFFER_FOREACH_V(i, curBuf, other.buf,
            sString_len_t len = sBuffer_single_count(curBuf);
            if (s->flags.obj.on_add_copy) { curBuf = sBuffer_single_copy(curBuf, 0, len); }
            sBuffer_single_add(last, sBuffer_single_get(curBuf), len);
        )
    } else {
        SMARTBUFFER_FOREACH_V(i, curBuf, other.buf,
            if (s->flags.obj.on_add_copy) { curBuf = sBuffer_single_copy(curBuf, 0, sBuffer_single_count(curBuf)); }
            sBuffer_add(&(s->buf), curBuf);
        )
    }
}
sString_len_t sString_append(sString * s, const sString_char_t * str, sString_len_t len) {
    if (s->flags.obj.is_const) { return 0; }
    if (s->flags.obj.on_add_join) {
        sBuffer_single_ptr last = sBuffer_get(&(s->buf), sBuffer_count_single(&(s->buf)) - 1);
        return sBuffer_single_add(last, str, len);
    } else {
        return sBuffer_append(&(s->buf), str, len);
    }
}

sString sString_get(const sString * s, sString_len_t index) {
    sBuffer_single_ptr inner = sBuffer_get(SSTRING_GETBUF_ADDR_P(s), index);

    sString_flags_u flags = s->flags; // copy flags of parent
    flags.obj.is_static = true; // this is static

    return sString_init((sString_char_t *) sBuffer_single_get(inner), sBuffer_single_count(inner), flags);;
}
sString sString_subStr(const sString * s, sString_len_t from, sString_len_t len) {
    sBuffer_single_ptr start = NULL;
    sString_len_t startIndex = 0;
    SMARTBUFFER_FOREACH_V(i, curBuf, s->buf,
        sString_len_t curLen = sBuffer_single_count(curBuf);

        start = curBuf;
        startIndex = i;

        if (from > curLen) { from -= curLen; }
        else { break; }
    )
    const sString_char_t * startCstr = sBuffer_single_get(start) + from;
    sString_len_t remainingLen = sBuffer_single_count(start) - from;
    if (len > remainingLen) {
        sString ret = sString_create();
        sString_add(&ret, sString_init((sString_char_t *) startCstr, remainingLen, SMARTSTRING_FLAG_U_STATIC_CONST));
        for (sString_len_t i = startIndex; i < sBuffer_count_single(SSTRING_GETBUF_ADDR_P(s)); i++) {
            sBuffer_single_ptr curBuf = sBuffer_get(SSTRING_GETBUF_ADDR_P(s), i);
            sString_len_t curLen = sBuffer_single_count(curBuf);

            if (curLen > len) {
                // add and continue to the next loop
                sString_addSingleBuf(&ret, curBuf);
                len -= curLen;
            } else {
                // we are finished
                //sBuffer_single_ptr part = sBuffer_single_create_static(sBuffer_single_get(curBuf), len);
                sString_addSingleBuf(&ret, curBuf);
                break;
            }
        }
        return ret;
    } else {
        sString ret = sString_init((sString_char_t *) startCstr, len, SMARTSTRING_FLAG_U_STATIC);
        ret.flags = s->flags; // copy flags of parent
        ret.flags.obj.is_static = true; // this is static
        return ret;
    }
}

void sString_read(const sString * str, sString_readHandler reader, sString_len_t len, void * userData) { sBuffer_read(SSTRING_GETBUF_ADDR_P(str), NULL, len, reader, userData); }

void sString_toCstr_reader(void * userData, const sString_char_t * str, sString_len_t len) {
    char * dest = (char *) userData;
    while (*dest != '\0') { dest++; } // skip until the end
    word_picker w = word_pick_from_to_index((const char *) str, 0, len);
    word_picker_toString(w, dest);
}
char * sString_toCstr(sString * s) {
    sString_len_t len = sBuffer_count(&(s->buf));
    char * ret = (char *) SMARTSTRING_H_MALLOC(len + 1);
    *ret = '\0';

    /*sString_len_t read = */sBuffer_read(&(s->buf), NULL, len, sString_toCstr_reader, ret);

    return ret;
}