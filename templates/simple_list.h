#ifndef SIMPLE_LIST_H
#define SIMPLE_LIST_H

#include "def.h"

#ifndef NO_STD_LIB
    #include <stdlib.h>
    #ifndef SIMPLE_LIST_H_MALLOC
    #define SIMPLE_LIST_H_MALLOC(size) malloc(size)
    #endif
    #ifndef SIMPLE_LIST_H_FREE
    #define SIMPLE_LIST_H_FREE(p) free(p)
    #endif
#endif

#define SIMPLE_LIST(type) \
struct { \
    type data; \
    indexP next; \
} *

#define SIMPLE_LIST_CREATE_EL(elP, Data, Next) \
{ \
    elP = SIMPLE_LIST_H_MALLOC(sizeof(*elP)); \
    elP->data = Data; \
    elP->next = (indexP) Next; \
}

#define SIMPLE_LIST_CREATE_EL_WITHOUT_ANYTHING(elP, size) \
{ \
    elP = (indexP) SIMPLE_LIST_H_MALLOC(size); \
}

#define SIMPLE_LIST_SET_DATA_AND_NEXT(elP, Data, Next) \
{ \
    elP->data = Data; \
    elP->next = (indexP) Next; \
}

#define SIMPLE_LIST_ADDFIRST(liste, Data) \
{ \
    if (liste == NULL) { \
        SIMPLE_LIST_CREATE_EL(liste, Data, NULL); \
    } else { \
        void * temp = (void *) liste; \
        SIMPLE_LIST_CREATE_EL(liste, Data, temp); \
    } \
}

#define SIMPLE_LIST_ADDLAST(iterator, Data) \
{ \
    if (iterator == NULL) { \
        SIMPLE_LIST_CREATE_EL(iterator, Data, NULL); \
    } else { \
        for(; iterator->next != (indexP) NULL; iterator = (void *) iterator->next) {  } \
        SIMPLE_LIST_CREATE_EL_WITHOUT_ANYTHING(iterator->next, sizeof(*iterator)); \
        iterator = (void *) iterator->next; \
        SIMPLE_LIST_SET_DATA_AND_NEXT(iterator, Data, NULL); \
    } \
}

#define SIMPLE_LIST_ADDINDEX(iterator, Data, index) \
{ \
    if (index == 0) { \
        SIMPLE_LIST_ADDFIRST(iterator, Data); \
    } \
    int i = 0; \
    unsigned char wasSet = 0; \
    while(iterator->next != (indexP) NULL) { \
        if (i == index) { \
            void * temp = (void *) iterator->next; \
            SIMPLE_LIST_CREATE_EL(iterator, Data, temp); \
            wasSet = 1; \
            break; \
        } \
        i++; iterator = (void *) iterator->next; \
    } \
    if (!wasSet) { \
        if (i == index) { \
            void * temp = (void *) iterator->next; \
            SIMPLE_LIST_CREATE_EL(iterator, Data, temp); \
        } else { \
            i++; \
            if (i == index) { \
                SIMPLE_LIST_CREATE_EL_WITHOUT_ANYTHING(iterator->next, sizeof(*iterator)); \
                iterator = (void *) iterator->next; \
                SIMPLE_LIST_SET_DATA_AND_NEXT(iterator, Data, NULL); \
            } \
        } \
    } \
}

#define SIMPLE_LIST_FOREACH(iterator, code) \
{ \
    for (; iterator != NULL; iterator = (void *) iterator->next) { \
        code \
    } \
}

#define SIMPLE_LIST_COPY(from_iterator, to_iterator, to) \
{ \
    SIMPLE_LIST_FOREACH(from_iterator, \
        SIMPLE_LIST_ADDLAST(to_iterator, from_iterator->data); \
        if (to == NULL) { to = to_iterator; } \
    ) \
}

// #define SIMPLE_LIST_FREE(liste) SIMPLE_LIST_FOREACH(liste, SIMPLE_ARRAY_H_FREE(liste->data))

#define SIMPLE_LIST_FREE(iterator) \
{ \
    void * before = NULL; \
    SIMPLE_LIST_FOREACH(iterator, \
        SIMPLE_LIST_H_FREE(before); \
        before = iterator; \
    ); \
    SIMPLE_LIST_H_FREE(before); \
}

#endif