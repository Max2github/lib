#ifndef LIST_H_H
#define LIST_H_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

enum list_types {
    Char, Integer, Float, Double, /*MiniBuffer, MiniString,*/ // primitive Variables - no alloction
    String, /*Buffer,*/ // allocated
    Char_pointer, Integer_pointer, Float_pointer, Double_pointer, // pointer - no allocation
    Void_pointer, // for e.g. functions - no alloction
    List, List_pointer, // for e.g. appending a List to another : List -> creates a copy, List_pointer -> only a pointer is given => Memory!
    End // symbolyses the end of the argument list (va_list)
};
typedef enum list_types list_type;

struct list_element {
    union {
        // this is used normally
        // it either stores the value directly in in or points it (value allocated on the HEAP)
        unsigned long long el;
        // this is used for copying bytes to el
        unsigned char elbuff[sizeof(unsigned long long)];
    };
    list_type type : 8;
    // may use instead (in the future):
    // unsigned char type;

    // we should use all of our leftover space
    // -> e.g. store strings with less than 7 (32 bit) / 15 (64 bit) directly in the list_element
    // on 64-bit systems: 7 bytes left
    // on 32-bit systems: 3 bytes left
    // unsigned char buf[sizeof(void *) - 1];

    // the bitfield does not work as I thought, so we only have 4 byte left on 64-bit systems
    // and 0 byte left on 32-bit systems
    unsigned char buf[sizeof(void *) - 4];


    struct list_element * next;
};
typedef struct list_element list_element;
typedef struct list_element * list;
typedef struct list_element * list_element_pointer;

typedef enum select_keys {
    select_All, select_Distinct, select_Whole_record, select_First
} select_key;

/**
 * only sort_size works for now
 * sort_value was not implemented yet
 */
typedef enum sort_keys {
    sort_size_low, sort_size_high, sort_value_low, sort_value_high
} sort_key;

// little functions
int list_node_len(list head);
list list_copy(list value);
// this is basically just a memcpy (copies nbytes of memory)
void list_element_memcopy(void * dest, void * src, unsigned int nbytes);

// functions and macros to get and set the value in list_element correctly
// getter
char get_list_element_value_char(unsigned long long value);
int get_list_element_value_int(unsigned long long value);
float get_list_element_value_float(unsigned long long value);
double get_list_element_value_double(unsigned long long value);
#define GET_LIST_ELEMENT_CHAR(list_el_pointer)   get_list_element_value_char(list_el_pointer->el)
#define GET_LIST_ELEMENT_INT(list_el_pointer)    get_list_element_value_int(list_el_pointer->el)
#define GET_LIST_ELEMENT_FLOAT(list_el_pointer)  get_list_element_value_float(list_el_pointer->el)
#define GET_LIST_ELEMENT_DOUBLE(list_el_pointer) get_list_element_value_double(list_el_pointer->el)
// setter
void set_list_element_char(list_element_pointer, char);
void set_list_element_int(list_element_pointer, int);
void set_list_element_float(list_element_pointer, float);
void set_list_element_double(list_element_pointer, double);

// functions to create new list_elements
// primitive
list_element_pointer new_list_element_char(char c);
list_element_pointer new_list_element_int(int zahl);
list_element_pointer new_list_element_float(float zahl);
list_element_pointer new_list_element_double(double zahl);
// array
list_element_pointer new_list_element_string(char * str);
// pointer
list_element_pointer new_list_element_char_pointer(char * zP);
list_element_pointer new_list_element_int_pointer(int * zP);
list_element_pointer new_list_element_float_pointer(float * zP);
list_element_pointer new_list_element_double_pointer(double * zP);
// list & other
list_element_pointer new_list_element_list(list under); // creates a list_element of ->type = List and ->el = list under
list_element_pointer new_list_element_type(list_type type, unsigned long long pointer); // creates a copy of a list_element and gives a pointer to it

// add, remove, print & free
// add
list list_addFirst_element(list head, list_element_pointer new_list_el);
void list_addLast_element(list head, list_element_pointer new_list_el_el);
list list_addFirst_list(list head, list newHead);
void list_addIndex_list(list head, list newEl, unsigned int index);
// remove
list list_removeFirst(list head);
void list_removeLast(list head);
void list_removeIndex(list head, unsigned int index);
// print
void list_print_format(unsigned int format);
void list_element_sprint(list_element_pointer head, char * saveto, list separatorList);
void list_toStr(list head, char * saveto, list separatorList);
void list_element_print(list_element_pointer head, unsigned int format);
void list_print(list head, unsigned int format);
// free
void list_element_free(list_element_pointer head);
void list_free(list head);

// whole lists
list new_list_element_as_list(va_list elems);
list list_addFirst(list head, ...);
void list_addLast(list head, ...);
void list_addIndex(list head, unsigned int index, ...);

list_element_pointer list_seek(list head, unsigned int index);
int list_findFirstIndex(list head, int (*cond)(unsigned long long , list_type, int));
int list_element_compare(unsigned long long value, list_type type, unsigned long long element, list_type eltype);
int list_find(list head, unsigned long long value, list_type type);

// selects
list list_select_one(list head, int (*cond)(unsigned long long , list_type, int), select_key keyword, list greatSelect);
list list_select_va(select_key keyword, va_list args);
list list_select(select_key keyword, ...);
void list_select_remove(select_key keyword, ...);
list list_select_index_one(list head, int (*cond)(unsigned long long , list_type, int), select_key keyword, list greatSelect);

// sorting
int list_element_size(list_element_pointer head);
int list_size(list head);
void list_swap_element(list_element_pointer a, list_element_pointer b);
int list_sort_compare(sort_key keyword, int eins, int zwei);
void list_sort_size_to_first(list head, sort_key keyword);
void list_sort(list head, sort_key keyword);

#ifdef __cplusplus
}
#endif

#endif