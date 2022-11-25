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
bool word_picker_find(word_picker w, word_picker s);
word_picker_list word_pick_splitList(const char * w, const char * search);
word_picker_array word_pick_split(const char * w, const char * search);

#endif