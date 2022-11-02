#ifndef LIST_H
#define LIST_H

// verkettete liste mit void * - pointern machen und diese in einer anderen verketteten liste speichern!
#include<stdarg.h>
// #include<stdio.h>
// #include<stdlib.h>
// #include<string.h>
#include "words.h"

#include "../list_h.h"

#ifndef NO_STD_LIB
    #define LIST_MALLOC(size) malloc(size)
    #define LIST_H_FREE(p) free(p)
#endif


// Code
int list_node_len(list head) {
    int i = 0;
    while (head != NULL) {
        head = head->next;
        i++;
    }
    return i;
}
list list_copy(list value) {
    if (value == NULL) { return NULL; }
    list new_list = NULL;
    /* puts("START:");
    list_print(value, 0);
    puts("val-end"); */

    while(value != NULL) {
        list_element_pointer copy_el = new_list_element_type(value->type, value->el);
        if (new_list == NULL) {
            new_list = list_addFirst_element(new_list, copy_el);
        } else {
            list_addLast_element(new_list, copy_el);
        }
        value = value->next;
    }
    /* puts("Ergebnis:");
    list_print(new_list, 0);
    puts("Fertig!"); */
    return new_list;
}

// list_el_el * & list_el_els
// create "list_el_el"
// primitive
list_element_pointer new_list_element_char(char c) {
    list_element_pointer eins = (list_element_pointer) LIST_MALLOC(sizeof(list_element));
    eins->el = (void *) LIST_MALLOC(sizeof(char));
    char * zP = (char *) eins->el;
    *zP = c;
    eins->type = Char;
    eins->next = NULL;
    return eins;
}
list_element_pointer new_list_element_int(int zahl) {
    list_element_pointer eins = (list_element_pointer) LIST_MALLOC(sizeof(list_element));
    eins->el = (void *) LIST_MALLOC(sizeof(int));
    int * zP = (int *) eins->el;
    *zP = zahl;
    eins->type = Integer;
    eins->next = NULL;
    return eins;
}
list_element_pointer new_list_element_float(float zahl) {
    list_element_pointer eins = (list_element_pointer) LIST_MALLOC(sizeof(list_element));
    eins->el = (void *) LIST_MALLOC(sizeof(float));
    float * zP = (float *) eins->el;
    *zP = zahl;
    eins->type = Float;
    eins->next = NULL;
    return eins;
}
list_element_pointer new_list_element_double(double zahl) {
    list_element_pointer eins = (list_element_pointer) LIST_MALLOC(sizeof(list_element));
    eins->el = (void *) LIST_MALLOC(sizeof(double));
    double * zP = (double *) eins->el;
    *zP = zahl;
    eins->type = Double;
    eins->next = NULL;
    return eins;
}
// Array
list_element_pointer new_list_element_string(char * str) {
    list_element_pointer eins = (list_element_pointer) LIST_MALLOC(sizeof(list_element));
    eins->el = (void *) LIST_MALLOC(sizeof(char *) * word_len(str));
    word_copy((char *) eins->el, str);
    eins->type = String;
    eins->next = NULL;
    return eins;
}
// pointer
list_element_pointer new_list_element_char_pointer(char * zP) {
    list_element_pointer eins = (list_element_pointer) LIST_MALLOC(sizeof(list_element));
    // eins->el = (void *) LIST_MALLOC(sizeof(char *));
    eins->el = (void *) zP;
    eins->type = Char_pointer;
    eins->next = NULL;
    return eins;
}
list_element_pointer new_list_element_int_pointer(int * zP) {
    list_element_pointer eins = (list_element_pointer) LIST_MALLOC(sizeof(list_element));
    // eins->el = (void *) LIST_MALLOC(sizeof(int *));
    eins->el = (void *) zP;
    eins->type = Integer_pointer;
    eins->next = NULL;
    return eins;
}
list_element_pointer new_list_element_float_pointer(float * zP) {
    list_element_pointer eins = (list_element_pointer) LIST_MALLOC(sizeof(list_element));
    // eins->el = (void *) LIST_MALLOC(sizeof(float *));
    eins->el = (void *) zP;
    eins->type = Float_pointer;
    eins->next = NULL;
    return eins;
}
list_element_pointer new_list_element_double_pointer(double * zP) {
    list_element_pointer eins = (list_element_pointer) LIST_MALLOC(sizeof(list_element));
    // eins->el = (void *) LIST_MALLOC(sizeof(double *));
    eins->el = (void *) zP;
    eins->type = Double_pointer;
    eins->next = NULL;
    return eins;
}
// list & other
list_element_pointer new_list_element_list(list under) {
    list_element_pointer eins = (list_element_pointer) LIST_MALLOC(sizeof(list_element));
    eins->el = (void *) under;
    eins->type = List;
    eins->next = NULL;
    return eins;
}
list_element_pointer new_list_element_type(list_type type, void * pointer) {
    list_element_pointer eins = NULL;
    switch (type) {
        // primitive
        case Char : {
            char * p = (char *) pointer;
            eins = new_list_element_char(*p);
            break;
        }
        case Integer : {
            int * p = (int *) pointer;
            eins = new_list_element_int(*p);
            break;
        }
        case Float: {
            float * p = (float *) pointer;
            eins = new_list_element_float(*p);
            break;
        }
        case Double : {
            double * p = (double *) pointer;
            eins = new_list_element_double(*p);
            break;
        }
        // pointer
        case String :
            eins = new_list_element_string((char *) pointer);
            break;
        case Char_pointer :
            eins = new_list_element_char_pointer((char *) pointer);
            break;
        case Integer_pointer :
            eins = new_list_element_int_pointer((int *) pointer);
            break;
        case Float_pointer :
            eins = new_list_element_float_pointer((float *) pointer);
            break;
        case Double_pointer :
            eins = new_list_element_double_pointer((double *) pointer);
            break;
        case Void_pointer :
            /* code */
            break;
        case List : 
            eins = new_list_element_list(list_copy( (list) pointer ));
            break;
        case List_pointer : 
            eins = new_list_element_list( (list) pointer );
            eins->type = List_pointer;
            break;
        case End : break;
    };
    return eins;
}

// add & remove
list list_addFirst_element(list head, list_element_pointer new_list_el) {
    new_list_el->next = head;
    return new_list_el;
}
void list_addLast_element(list head, list_element_pointer new_list_el) {
    if (head == NULL) {
        return;
    }
    while (head->next != NULL) {
        head = head->next;
    }
    head->next = new_list_el;
}
list list_addFirst_list(list head, list newHead) {
    list list = newHead;
    while (newHead->next != NULL) {
        newHead = newHead->next;
    }
    newHead->next = head;
    return list;
}
void list_addIndex_list(list head, list newEl, unsigned int index) {
    list temp = head;
    list beforeTemp = NULL;
    int i = 0;
    while(temp != NULL) {
        if (i == index) {
            beforeTemp->next = newEl;
            while (newEl->next != NULL) {
                newEl = newEl->next;
            }
            newEl->next = temp;
            return;
        }
        beforeTemp = temp;
        temp = temp->next;
        i++;
    }
    if (i == index) {
        beforeTemp->next = newEl;
        while (newEl->next != NULL) {
            newEl = newEl->next;
        }
        newEl->next = temp;
        return;
    }
}
// remove
list list_removeFirst(list head) {
    list next = head->next;
    list_element_free(head);
    return next;
}
void list_removeLast(list head) {
    list before = NULL;
    while (head->next != NULL) {
        before = head;
        head = head->next;
    }
    list_element_free(head);
    before->next = NULL;
}
void list_removeIndex(list head, unsigned int index) {
    list before = NULL;
    int i = 0;
    while (head != NULL) {
        if (i == index) {
            before->next = head->next;
            list_element_free(head);
            head = before;
        }
        before = head;
        head = head->next;
        i++;
    }
    if (i == index) {
        before->next = head->next;
        list_element_free(head);
    }
}

// print & free
// print
void list_print_format(unsigned int format) {
    for (int i = 0; i < format; i++) {
        putchar(' ');
    }
}
void list_element_sprint(list_element_pointer head, char * saveto, list separatorList) {
    switch(head->type) {
        // primitive
        case Char : {
            char * el = (char *) head->el;
            // sprintf(saveto, "%c", *el);
            saveto[0] = *el;
            saveto[1] = '\0';
            break;
        }
        case Integer : {
            int * el = (int *) head->el;
            // sprintf(saveto, "%d", *el);
            intToString(*el, saveto);
            break;
        }
        case Float : {
            float * el = (float *) head->el;
            // sprintf(saveto, "%f", *el);
            doubleToString(*el, 6, saveto);
            break;
        }
        case Double : {
            double * el = (double *) head->el;
            // sprintf(saveto, "%f", *el);
            doubleToString(*el, 6, saveto);
            break;
        }
        // array
        case String : 
            // sprintf(saveto, "%s", (char *) head->el);
            word_copy(saveto, (char *) head->el);
            break;
        // pointer
        case Char_pointer : case Integer_pointer : 
        case Float_pointer : case Double_pointer : 
        case Void_pointer : break;
        // list
        case List : case List_pointer : list_toStr((list) head->el, saveto, separatorList->next); break;
        case End : break;
    }
}
void list_toStr(list head, char * saveto, list separatorList) {
    while (head != NULL) {
        list_element_sprint(head, saveto, separatorList);
        if (head->next != NULL && separatorList != NULL) { word_add(saveto, (const char *) separatorList->el); }
        while (*saveto != '\0') {
            saveto++;
        }
        head = head->next;
    }
}

void list_element_print(list_element_pointer head,  unsigned int format) {
    switch(head->type) {
        // primitive
        case Char : {
            char * el = (char *) head->el;
            printf("%c", *el);
            break;
        }
        case Integer : {
            int * el = (int *) head->el;
            printf("%d", *el);
            break;
        }
        case Float : {
            float * el = (float *) head->el;
            //printf("%f", *el);
            char str[intLen((int)*el)+8]; // + '.' + 6 afterpoint + 1 '\0'
            doubleToString(*el, 6, str);
            puts(str);
            break;
        }
        case Double : {
            double * el = (double *) head->el;
            // printf("%f", *el);
            char str[intLen((int)*el)+8]; // + '.' + 6 afterpoint + 1 '\0'
            doubleToString(*el, 6, str);
            printf("%s", str);
            break;
        }
        // array
        case String : printf("%s", (char *) head->el); break;
        // pointer
        case Char_pointer : {
            char * eP = (char *) head->el;
            printf("-> %c", *eP);
            break;
        }
        case Integer_pointer : {
            int * eP = (int *) head->el;
            printf("-> %d", *eP);
            break;
        }
        case Float_pointer : {
            float * eP = (float *) head->el;
            // printf("-> %f", *eP);
            char str[intLen((int)*eP)+8]; // + '.' + 6 afterpoint + 1 '\0'
            doubleToString(*eP, 6, str);
            printf("-> %s", str);
            break;
        }
        case Double_pointer : {
            double * eP = (double *) head->el;
            // printf("-> %f", *eP);
            char str[intLen((int)*eP)+8]; // + '.' + 6 afterpoint + 1 '\0'
            doubleToString(*eP, 6, str);
            printf("-> %s", str);
            break;
        }
        case Void_pointer : printf("Function"); break;
        // list
        case List : list_print((list) head->el, format); break;
        case List_pointer : list_print((list) head->el, format); break;
        case End : break;
    }
}
void list_print(list head, unsigned int format) {
    if (head == NULL) {
        list_print_format(format); puts("NULL");
        return;
    }
    putchar('['); putchar('\n');
    format += 4;
    while (head != NULL) {
        // if (head->type != List && head->type != List_pointer) {
            list_print_format(format);
        // };
        switch(head->type) {
            // primitive
            case Char : printf("%s", "Char: "); break;
            case Integer : printf("%s", "Integer: "); break;
            case Float : printf("%s", "Float: "); break;
            case Double : printf("%s", "Double: "); break;
            // array
            case String : printf("%s", "String: "); break;
            // pointer
            case Char_pointer : printf("%s", "Char_pointer : "); break;
            case Integer_pointer : printf("%s", "Int_pointer : "); break;
            case Float_pointer : printf("%s", "Float_pointer : "); break;
            case Double_pointer : printf("%s", "Double_pointer : "); break;
            case Void_pointer : printf("%s", "Void_pointer : "); break;
            case List : break;
            case List_pointer : break;
            case End : break;
        }
        list_element_print(head, format);
        if (head->type != List) {
            putchar('\n');
        }
        head = head->next;
    }
    format -= 4;
    list_print_format(format); putchar(']'); putchar('\n');
}
// free
void list_element_free(list_element_pointer head) {
    if (head->type == List) {
        list_free((list) head->el);
    } else if (head->type != Char_pointer && head->type != Integer_pointer && head->type != Float_pointer && head->type != Double_pointer && head->type != Void_pointer && head->type != List_pointer) {
        LIST_H_FREE(head->el);
    }
    LIST_H_FREE(head);
}
void list_free(list head) {
    list temp = head;
    while (temp != NULL) {
        list next = temp->next;
        list_element_free(temp);
        temp = next;
    }
}

// list
list new_list_element_as_list(va_list elems) {
    list new_list = NULL;
    list_type type = va_arg(elems, list_type);
    while (type != End) {
        list_element_pointer new_list_el = NULL;
        switch (type) {
            // primitive
            case Char : // char was promoted to int by being passed as an argument
                new_list_el = new_list_element_char(va_arg(elems, int));
                break;
            case Integer :
                new_list_el = new_list_element_int(va_arg(elems, int));
                break;
            case Float: // float was promoted to double by being passed as an argument
                new_list_el = new_list_element_float(va_arg(elems, double));
                break;
            case Double :
                new_list_el = new_list_element_double(va_arg(elems, double));
                break;
            
            // pointer
            case String :
                new_list_el = new_list_element_string(va_arg(elems, char *));
                break;
            case Char_pointer :
                new_list_el = new_list_element_char_pointer(va_arg(elems, char *));
                break;
            case Integer_pointer :
                new_list_el = new_list_element_int_pointer(va_arg(elems, int *));
                break;
            case Float_pointer :
                new_list_el = new_list_element_float_pointer(va_arg(elems, float *));
                break;
            case Double_pointer :
                new_list_el = new_list_element_double_pointer(va_arg(elems, double *));
                break;
            case Void_pointer :
                /* code */
                break;
            case List : 
                new_list_el = new_list_element_list( list_copy( va_arg(elems, list) ) );
                break;
            case List_pointer : 
                new_list_el = new_list_element_list( va_arg(elems, list) );
                new_list_el->type = List_pointer;
                break;
            case End : return new_list;
        };
        if (new_list == NULL) {
            // printf("%d\n", list_node_len(new_list_el));
            if (type == List || type == List_pointer) {
                new_list = new_list_el;
                new_list->next = NULL;
            } else {
                new_list = list_addFirst_element(new_list, new_list_el);
            }
            // list_print(new_list, 8);
        } else {
            list_addLast_element(new_list, new_list_el);
        }
        type = va_arg(elems, list_type); // get the type of the next argument
    }
    //list_print(new_list, 8);
    return new_list;
}
list list_addFirst(list head, ...) {
    list_element_pointer newHead = NULL;
    list newHead_list = NULL;

    va_list node_els;
    va_start(node_els, head);
    newHead_list = new_list_element_as_list(node_els);
    va_end(node_els);

    if (list_node_len(newHead_list) == 1) {
        newHead_list->next = head;
        return newHead_list;
    }
    newHead = (list_element_pointer) LIST_MALLOC(sizeof(list_element));
    newHead->el = (void *) newHead_list;
    newHead->type = List;
    newHead->next = head;

    return newHead;
}
void list_addLast(list head, ...) {
    list_element_pointer newLast = NULL;
    list newLast_list = NULL;

    va_list node_els;
    va_start(node_els, head);
    newLast_list = new_list_element_as_list(node_els);
    va_end(node_els);

    if (list_node_len(newLast_list) == 1) {
        while (head->next != NULL) {
            head = head->next;
        }
        head->next = newLast_list;
        return;
    }

    newLast = (list_element_pointer) LIST_MALLOC(sizeof(list_element));
    newLast->el = (void *) newLast_list;
    newLast->type = List;
    newLast->next = NULL;

    while (head->next != NULL) {
        head = head->next;
    }
    head->next = newLast;

    return;
}
void list_addIndex(list head, unsigned int index, ...) {
    list_element_pointer newEl = NULL;
    list newEl_list = NULL;

    va_list node_els;
    va_start(node_els, index);
    newEl_list = new_list_element_as_list(node_els);
    va_end(node_els);

    if (list_node_len(newEl_list) == 1) {
        list beforeHead = NULL;
        int i = 0;
        while (head != NULL) {
            if (i == index) {
                beforeHead->next = newEl_list;
                newEl_list->next = head;
                return;
            }
            beforeHead = head;
            head = head->next;
            i++;
        }
        if (i == index) {
            beforeHead->next = newEl_list;
            newEl_list->next = head;
        }
        return;
    }

    list beforeHead = NULL;
    int i = 0;
    while (head != NULL) {
        if (i == index) {
            newEl = (list_element_pointer) LIST_MALLOC(sizeof(list_element));
            newEl->el = (void *) newEl_list;
            newEl->type = List;

            beforeHead->next = newEl;
            newEl->next = head;
            return;
        }
        beforeHead = head;
        head = head->next;
        i++;
    }
    if (i == index) {
        newEl = (list_element_pointer) LIST_MALLOC(sizeof(list_element));
        newEl->el = (void *) newEl_list;
        newEl->type = List;

        beforeHead->next = newEl;
        newEl->next = head;
        return;
    }
}

// getting data from lists
list_element_pointer list_seek(list head, unsigned int index) {
    int i = 0;
    while (head != NULL) {
        if (i == index) {
            break;
        }
        head = head->next;
        i++;
    }
    return head;
}
int list_findFirstIndex(list head, int (*cond)(void * , list_type, int)) {
    int index = 0;
    if (head == NULL) {
        return -1;
    }
    while (head != NULL) {
        int tf = (*cond)(head->el, head->type, index);
        if (tf == 1) {
            if (tf == 1) {
                return index;
            }
        } else if (head->type == List || head->type == List_pointer) {
            int sel = list_findFirstIndex((list) head->el, cond); 
            if (sel != -1) {
                return index;
            }
        }
        head = head->next;
        index++;
    }
    return -1;
}
int list_element_compare(void * value, list_type type, void * element, list_type eltype) {
    if (eltype == List_pointer) {
        eltype = List;
    }
    if (type == List_pointer) {
        type = List;
    }
    if (eltype != type) {
        return 0;
    }
    switch (type) {
        // primitive
        case Char : {
            if (*((char *) value) == *((char *) element)) {
                return 1;
            }
            break;
        }
        case Integer : {
            if (*((int *) value) == *((int *) element)) {
                return 1;
            }
            break;
        }
        case Float: {
            if (*((float *) value) == *((float *) element)) {
                return 1;
            }
            break;
        }
        case Double : {
            if (*((double *) value) == *((double *) element)) {
                return 1;
            }
            break;
        }
        // pointer
        case String : {
            if (word_compare((char *) value, (char *) element) == 0) {
                return 1;
            }
            break;
        }
        case Char_pointer : {
            if ((char *) value == (char *) element) {
                return 1;
            }
            break;
        }
        case Integer_pointer : {
            if ((char *) value == (char *) element) {
                return 1;
            }
            break;
        }
        case Float_pointer : {
            if ((char *) value == (char *) element) {
                return 1;
            }
            break;
        }
        case Double_pointer : {
            if ((char *) value == (char *) element) {
                return 1;
            }
            break;
        }
        case Void_pointer : {
            if ((char *) value == (char *) element) {
                return 1;
            }
            break;
        }
        case List : {
            // need a list_compare function!!!
            list l1 = (list) value;
            list l2 = (list) element;
            while (1) {
                if (l1 == NULL && l2 == NULL) {
                    return 1;
                }
                if (list_element_compare(l1->el, l1->type, l2->el, l2->type) == 0) {
                    return 0;
                }
                l1 = l1->next;
                l2 = l2->next;
            }
            break;
        }
        case List_pointer : {
            if ((list) value == (list) element) {
                return 1;
            }
            break;
        }
        case End : break;
    };
    return 0;
}
int list_find(list head, void * value, list_type type) {
    int found = 0;
    while (head != NULL) {
        if (list_element_compare(head->el, head->type, value, type) == 1) {
            found++;
        }
        head = head->next;
    }
    return found;
}

list list_select_one(list head, int (*cond)(void * , list_type, int), select_key keyword, list greatSelect) {
    if (head == NULL) {
        return NULL;
    }
    list selected = NULL;
    int index = 0;
    list trueHead = head;
    while(head != NULL) {
        int tf = (*cond)(head->el, head->type, index);
        if (tf == 1) {
            switch (keyword) {
                case select_Distinct : {
                    int isAlready = list_find(greatSelect, head->el, head->type);
                    if (isAlready != 0) {
                        tf = 2;
                    }
                    isAlready = list_find(selected, head->el, head->type);
                    if (isAlready != 0) {
                        tf = 2;
                    }
                    break;
                }
                case select_Whole_record : {
                    list_element_pointer sel = new_list_element_type(List, trueHead);
                    int isAlready = list_find(greatSelect, trueHead, List);
                    if (isAlready != 0) {
                        tf = 2;
                    }
                    isAlready = list_find(selected, trueHead, List);
                    if (isAlready != 0) {
                        tf = 2;
                    }

                    if (tf == 1) {
                        if (selected == NULL) {
                            selected = list_addFirst_element(selected, sel);
                        } else {
                            list_addLast_element(selected, sel);
                        }
                    } else {
                        list_element_free(sel);
                    }
                    tf = 2;
                    break;
                }
                case select_First : break;
                case select_All : break;
            }
            if (tf == 1) {
                list_element_pointer sel = new_list_element_type(head->type, head->el);
                if (selected == NULL) {
                    selected = list_addFirst_element(selected, sel);
                } else {
                    list_addLast_element(selected, sel);
                }
                if (keyword == select_First) {
                    break;
                }
            }
        } else if (head->type == List || head->type == List_pointer) {
            list sel = list_select_one((list) head->el, cond, keyword, greatSelect);
            if (sel != NULL) {
                if (selected == NULL) {
                    selected = list_addFirst_list(selected, sel);
                } else {
                    list_addLast_element(selected, list_select_one((list) head->el, cond, keyword, greatSelect));
                }
            }
        }
        head = head->next;
        index++;
    }
    return selected;
}
list list_select_va(select_key keyword, va_list args) {
    list selected = NULL;
    list l = va_arg(args, list);
    while (l != NULL) {
        int (*cond)(void * , list_type, int) = va_arg(args, int (*)(void * , list_type, int));
        
        list sel = list_select_one(l, cond, keyword, selected);
        if (sel != NULL) {
            if (selected == NULL) {
                selected = list_addFirst_list(selected, sel);
            } else {
                list_addLast_element(selected, sel);
            }
        }

        l = va_arg(args, list);
    }
    return selected;
}
// structure: list_select(char * saveto, list, condition, list, condition, ..., NULL)
list list_select(select_key keyword, ...) {
    va_list args;
    va_start(args, keyword);
    list selected = list_select_va(keyword, args);
    va_end(args);
    return selected;
}
void list_select_remove(select_key keyword, ...) {
    va_list args;
    va_start(args, keyword);

    list * l = va_arg(args, list * );
    while (*l != NULL) {
        int (*cond)(void * , list_type, int) = va_arg(args, int (*)(void * , list_type, int));
        
        switch(keyword) {
            case select_All : break;
            case select_Distinct : break;
            case select_Whole_record : {
                list temp = *l;
                unsigned int index = 0;
                while (temp != NULL) {
                    int tf = (*cond)(temp->el, temp->type, index);
                    if (tf == 1) {
                        list_free(*l);
                        *l = NULL;
                        return;
                    }
                    temp = temp->next;
                    index++;
                }
                return;
            }
            case select_First : {
                return;
            }
        }
        
        list temp = *l;
        list beforeTemp = NULL;
        int index = 0;
        while (temp != NULL) {
            int tf = (*cond)(temp->el, temp->type, index);
            if (tf == 1) {
                if (index == 0) {
                    // printf("Error: Unable to remove first element!");
                    beforeTemp = temp;
                    temp = temp->next;
                    *l = list_removeFirst(*l);
                } else {
                    list next = temp->next;

                    list_element_free(temp);
                    beforeTemp->next = next;

                    temp = next;
                }
            } else if (temp->type == List || temp->type == List_pointer) {
                beforeTemp = temp;
                temp = temp->next;
            } else {
                beforeTemp = temp;
                temp = temp->next;
            }
            index++;
        }

        l = va_arg(args, list * );
    }

    va_end(args);
}

list list_select_index_one(list head, int (*cond)(void * , list_type, int), select_key keyword, list greatSelect) {
    list selected = NULL;
    int index = 0;
    if (head == NULL) {
        return NULL;
    }
    while (head != NULL) {
        int tf = (*cond)(head->el, head->type, index);
        if (tf == 1) {
            switch(keyword) {
                case select_All : break;
                case select_Distinct : break;
                case select_Whole_record : break;
                case select_First : break;
            }
            if (tf == 1) {
                selected = list_addFirst(NULL, Integer, index, End);
                return selected;
            }
        } else if (head->type == List || head->type == List_pointer) {
            list underSel = list_select_index_one((list) head->el, cond, keyword, greatSelect);
            selected = list_addFirst(NULL, Integer, index, End);
            if (underSel != NULL) {
                list_addLast_element(selected, underSel);
                return selected;
            }
        }
        head = head->next;
        index++;
    }
    list_free(selected);
    return NULL;
}
list list_select_index(select_key keyword, ...) {
    list selected = NULL;
    va_list args;
    va_start(args, keyword);

    list l = va_arg(args, list);
    while (l != NULL) {
        int (*cond)(void * , list_type, int) = va_arg(args, int (*)(void * , list_type, int));

        list isHere = list_select_index_one(l, cond, keyword, selected);
        if (isHere != NULL) {
            if (selected == NULL) {
                selected = list_addFirst_list(NULL, isHere);
            } else {
                list_addLast_element(selected, isHere);
            }
            list_addLast(selected, Integer, -1, End);
        }

        l = va_arg(args, list);
    }
    return selected;
}

int list_element_size(list_element_pointer head) {
    if (head == NULL) {
        return 0;
    }
    int size = sizeof(list_type) + sizeof(list_element_pointer);
    switch (head->type) {
        // primitive
        case Char : { size += sizeof(char); break; }
        case Integer : { size += sizeof(int); break; }
        case Float : { size += sizeof(float); break; }
        case Double : { size += sizeof(double); break; }
        // array
        case String : { size += word_len((char *) head->el) * sizeof(char); break;}
        // pointer
        case Char_pointer : case Integer_pointer : case Float_pointer : 
        case Double_pointer : case Void_pointer : case List_pointer : {
            size += 8; break;
        }
        case List : { size += list_size((list) head->el); break; }
        case End : return -1;
    }
    return size;
}
int list_size(list head) {
    int size = 0;
    while (head != NULL) {
        size += list_element_size(head);
        head = head->next;
    }
    return size;
}

void list_swap_element(list_element_pointer a, list_element_pointer b) {
    void * valueA = a->el; list_type typeA = a->type;
    a->el = b->el; a->type = b->type;
    b->el = valueA; b->type = typeA;
}
int list_sort_compare(sort_key keyword, int eins, int zwei) {
    switch (keyword) {
        case sort_value_low :
        case sort_size_low :{
            if (eins < zwei) { return 1; }
            break;
        }
        case sort_value_high :
        case sort_size_high : {
            if (eins > zwei) { return 1; }
            break;
        }
    }
    if (eins == zwei) { return 0; }
    return -1;
}
void list_sort_size_to_first(list head, sort_key keyword) {
    list temp = head;
    int smallest = -1;
    list_element_pointer sm = NULL;
    while (temp != NULL) {
        int currentSize = list_element_size(temp);
        int comp = list_sort_compare(keyword, currentSize, smallest);

        if (smallest == -1 || comp == 1) {
            smallest = currentSize;
            sm = temp;
        } else if (comp == 0) {
            
        }
        temp = temp->next;
    }
    list_swap_element(head, sm);
}
void list_sort(list head, sort_key keyword) {
    switch (keyword) {
        case sort_size_high :
        case sort_size_low : {
            while (head->next != NULL) {
                list_sort_size_to_first(head, keyword);
                head = head->next;
            }
            break;
        }
        case sort_value_high : 
        case sort_value_low : {
            break;
        }
    }
}

#endif