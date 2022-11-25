#ifndef WORDS_TEMPLATE_H
#define WORDS_TEMPLATE_H

#include "def.h"

#define FIND_TEMPLATE(in, what, len_func, len_type, get_index_w, get_index_s) \
    len_type size = len_func(in); \
    len_type searchSize = len_func(what); \
    int found = 0; \
    int trueN = 0; \
    for (len_type i = 0; i < size; i++) { \
        for (len_type n = trueN; n < searchSize; n++) { \
            if (get_index_w == get_index_s) { \
                if (n == searchSize - 1) { \
                    found = 2; \
                } else { \
                    found = 1; \
                    trueN = trueN + 1; \
                    break; \
                } \
            } else { \
                found = 0; \
                trueN = 0; \
                break; \
            } \
        } \
        if (found == 2) { break; } \
    } \
    if (found == 2) { return true; } \
    return false;

#endif