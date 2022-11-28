#ifndef WORD_PICK_H_H
#define WORD_PICK_H_H

#include "templates/def.h"
#include "templates/simple_list.h"
#include "templates/simple_array.h"

struct word_picker {
    const char * begin;
    const char * end;
};
typedef struct word_picker word_picker;
typedef SIMPLE_LIST(word_picker)  word_picker_list;
typedef SIMPLE_ARRAY(word_picker) word_picker_array;
#define WORD_PICKER_ARRAY_GET(arr, index) SIMPLE_ARRAY_GET(arr, word_picker, index)
#define WORD_PICK_INIT(begin, end) { begin, end }

word_picker word_pick_until_index(const char * word, unsigned int index);
word_picker word_pick_until(const char * word, const char * search);
word_picker word_pick_from_to_index(const char * word, unsigned int start, unsigned int end);
word_picker word_pick_from_to(const char * word, const char * search1, const char * search2);

unsigned int word_picker_len(word_picker w);
int word_picker_compare(word_picker eins, word_picker zwei);
int word_pick_compare(word_picker picked, const char * word);

// find
bool word_picker_find(word_picker w, const char * s);
bool word_picker_findP(word_picker w, word_picker s);

// splitting

// list
word_picker_list word_pick_splitList(const char * w, const char * search);
// not yet!
// word_picker_list word_picker_splitList(const word_picker * w, const char * search);
// word_picker_list word_picker_splitListP(const word_picker * w, const word_picker * search);

// array
word_picker_array word_pick_split(const char * w, const char * search);
word_picker_array word_picker_split(word_picker w, const char * search);
// not yet!
// word_picker_array word_picker_splitP(word_picker w, word_picker search);

// to string
void word_picker_toString(word_picker w, char * saveto);
// void word_picker_join_toString()

#endif