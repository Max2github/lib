#include "../word_pick_h.h"

#include "../templates/words_template.h"
#include "../words_h.h"

#ifndef NO_STD_LIB
    #include <stdarg.h>
    #define WORD_PICK_H_VA_LIST             va_list
    #define WORD_PICK_H_VA_ARG(ap, type)    va_arg(ap, type)
    #define WORD_PICK_H_VA_START(ap, param) va_start(ap, param)
    #define WORD_PICK_H_VA_END(ap)          va_end(ap)
#endif

#ifdef __cplusplus
extern "C" {
#endif

word_picker word_pick_until_index(const char * word, unsigned int index) {
    word_picker ret = WORD_PICK_INIT(word, word+index);
    return ret;
}
word_picker word_pick_until(const char * word, const char * search) {
    word_picker ret = WORD_PICK_INIT(word, NULL);
    ret.end = word_seek(word, search);
    return ret;
}
word_picker word_pick_until_any(const char * word, index32 number, ...) {
    WORD_PICK_H_VA_LIST args;
    WORD_PICK_H_VA_START(args, number);

    word_picker ret = WORD_PICK_INIT(word, NULL);
    ret.end = word_seek_first_v(word, (unsigned int) number, args);

    WORD_PICK_H_VA_END(args);

    return ret;
}
word_picker word_pick_from_to_index(const char * word, unsigned int start, unsigned int end) {
    word_picker ret = WORD_PICK_INIT(word+start, word+end);
    return ret;
}
word_picker word_pick_from_to(const char * word, const char * search1, const char * search2) {
    word_picker ret = WORD_PICK_INIT(NULL, NULL);
    ret.begin = word_seek(word, search1); // include - from the beginning of search1
    ret.end = word_seek(ret.begin, search2); // exclude - to the char before the beginning of search2
    return ret;
}
unsigned int word_picker_len(word_picker w) {
    int len = w.end - w.begin;
    return (len > 0) ? len : 0;
}
int word_picker_compare(word_picker eins, word_picker zwei) {
    unsigned int einsLen = word_picker_len(eins);
    unsigned int zweiLen = word_picker_len(zwei);
    if (einsLen > zweiLen) { return 1; }
    if (einsLen < zweiLen) { return -1; }
    unsigned int wahr = 0;
    while (eins.begin != eins.end) {
        if (*eins.begin++ == *zwei.begin++) {
            wahr++;
        }
    }
    if (wahr == einsLen) {
        return 0;
    }
    return 2;
}
int word_pick_compare(word_picker picked, const char * word) {
    unsigned int einsLen = word_picker_len(picked);
    unsigned int zweiLen = word_len(word);
    if (einsLen > zweiLen) { return 1; }
    if (einsLen < zweiLen) { return -1; }
    unsigned int wahr = 0;
    while (picked.begin != picked.end) {
        if (*picked.begin++ == *word++) {
            wahr++;
        }
    }
    if (wahr == einsLen) {
        return 0;
    }
    return 2;
}

bool word_picker_find(word_picker w, const char * s) {
    FIND_TEMPLATE(w, s, word_picker_len, word_len, unsigned int, w.begin[i], s[n])
}
bool word_picker_findP(word_picker w, word_picker s) {
    FIND_TEMPLATE(w, s, word_picker_len, word_picker_len, unsigned int, w.begin[i], s.begin[n])
}

index32 word_picker_findIndex_first(word_picker w) {
    return 0;
}

// splitting

// list

word_picker_list word_pick_splitList(const char * w, const char * search) {
    word_picker_list splitted = NULL;
    if (*search == '\0') {
        for (; *w != '\0'; w++) {
            word_picker pickedChar = WORD_PICK_INIT(w, w+1);
            word_picker_list iterator = splitted;
            SIMPLE_LIST_ADDLAST(iterator, pickedChar);
            if (splitted == NULL) { splitted = iterator; }
        }
        return splitted;
    }
    index32 searchLen = word_len(search);
    while (find(w, search)) {
        word_picker next = word_pick_until(w, search);
        index32 toSkip = word_picker_len(next) + searchLen;
        word_picker_list iterator = splitted;
        SIMPLE_LIST_ADDLAST(iterator, next);
        if (splitted == NULL) { splitted = iterator; }
        w += toSkip;
    }
    word_picker rest = WORD_PICK_INIT(w, w + word_len(w));
    word_picker_list iterator = splitted;
    SIMPLE_LIST_ADDLAST(iterator, rest);
    return splitted;
}

// array

word_picker_array word_pick_split(const char * w, const char * search) {
    word_picker_array splitted = SIMPLE_ARRAY_CREATE(word_picker);
    index32 searchLen = word_len(search);
    while (find(w, search)) {
        word_picker next = word_pick_until(w, search);
        unsigned int len = word_picker_len(next);
        index32 toSkip = len + searchLen;
        SIMPLE_ARRAY_APPEND(splitted, next);
        w += toSkip;
    }
    word_picker rest = WORD_PICK_INIT(w, w + word_len(w));
    SIMPLE_ARRAY_APPEND(splitted, rest);
    return splitted;
}

word_picker_array word_picker_split(word_picker w, const char * search) {
    word_picker_array splitted = SIMPLE_ARRAY_CREATE(word_picker);
    index32 searchLen = word_len(search);
    while (word_picker_find(w, search)) {
        word_picker next = word_pick_until(w.begin, search);
        unsigned int len = word_picker_len(next);
        index32 toSkip = len + searchLen;
        SIMPLE_ARRAY_APPEND(splitted, next);
        w.begin += toSkip;
    }
    // append the rest
    SIMPLE_ARRAY_APPEND(splitted, w);
    return splitted;
}

/*
word_picker_array word_picker_splitP(word_picker w, word_picker search) {
    word_picker_array splitted = SIMPLE_ARRAY_CREATE(word_picker);
    index32 searchLen = word_picker_len(search);
    while (word_picker_findP(w, search)) {
        word_picker next = word_pick_until(w.begin, search);
        unsigned int len = word_picker_len(next);
        index32 toSkip = len + searchLen;
        SIMPLE_ARRAY_APPEND(splitted, next);
        w.begin += toSkip;
    }
    // append the rest
    SIMPLE_ARRAY_APPEND(splitted, w);
    return splitted;
}*/

// to string
void word_picker_toString(word_picker w, char * saveto) {
    for (; w.begin != w.end; w.begin++) {
        *saveto = *(w.begin);
        saveto++;
    }
    *saveto = '\0';
}

#ifdef __cplusplus
}
#endif
