/**
 * @file words_template.h
 * @author Max2
 * @brief functions originating from words.h as macros
 * The following macros are based on well-working functions
 * of words.h in order to use their concept more generally.
 * It should be sort of like a template in C.
 * These macros should not be used directly,
 * but should be called in an own function.
 * 
 * - FIND_TEMPLATE - general find function
 *   Originaly for arrays, but can also be used with other formats / types / structures.
 *   + e.g. FIND_TEMPLATE(
 *       list1, list2, // the lists - seach (entire) list2 in list1
 *       list_len, list_len, // a function (or clever (!) macro) for getting the number of elements in each list
 *       int, // the type which is returned from the lenth functions
 *       list_getIndex(i), list_getIndex(n) // get the individual elements
 *     )
 *   + Warning: This macro may not always be efficient, especially not for linked lists.
 * 
 */

#ifndef WORDS_TEMPLATE_H
#define WORDS_TEMPLATE_H

#include "def.h"

#define FIND_TEMPLATE(in, what, len_func_w, len_func_s, len_type, get_index_w, get_index_s) \
    len_type size = len_func_w(in); \
    len_type searchSize = len_func_s(what); \
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