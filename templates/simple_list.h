#ifndef SIMPLE_LIST_H
#define SIMPLE_LIST_H

#ifndef NO_STD_LIB
    #include <stdlib.h>
    #define SIMPLE_LIST_MALLOC(size) malloc(size)
#endif

#define SIMPLE_LIST(type) \
struct \
{ \
    type data; \
    long next; \
} *

#define SIMPLE_LIST_CREATE_EL(elP, Data, Next) \
{ \
    elP = SIMPLE_LIST_MALLOC(sizeof(*elP)); \
    elP->data = Data; \
    elP->next = (long) Next; \
}

#define SIMPLE_LIST_CREATE_EL_WITHOUT_ANYTHING(elP, size) \
{ \
    elP = (long) SIMPLE_LIST_MALLOC(size); \
}

#define SIMPLE_LIST_SET_DATA_AND_NEXT(elP, Data, Next) \
{ \
    elP->data = Data; \
    elP->next = (long) Next; \
}

#define SIMPLE_LIST_ADDFIRST(liste, Data) \
{ \
    if (liste == NULL) { \
        SIMPLE_LIST_CREATE_EL(liste, Data, NULL); \
    } else { \
        void * temp = liste; \
        SIMPLE_LIST_CREATE_EL(liste, Data, temp); \
    } \
}

#define SIMPLE_LIST_ADDLAST(iterator, Data) \
{ \
    if (iterator == NULL) { \
        SIMPLE_LIST_CREATE_EL(iterator, Data, NULL); \
    } else { \
        for(; iterator->next != (long) NULL; iterator = (void *) iterator->next); \
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
    while(iterator->next != (long) NULL) { \
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

#endif