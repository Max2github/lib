#ifndef GENERAL_ALLOC_H_H
#define GENERAL_ALLOC_H_H

#include "../templates/def.h"

#define GENERAL_ALLOC_NODE_SIZE ((int32) sizeof(HEAP_NODE))
#define GENERAL_ALLOC_FIRST_NODE_SIZE ((int32) sizeof(HEAP_NODE_FIRST))

#define GENERAL_ALLOC_NODE_NEXT(node) ((HEAP_NODE *) ((index8 *) node + GENERAL_ALLOC_NODE_SIZE + node->size))

#define GENERAL_ALLOC_START_OF_LIST(head) &(((HEAP_NODE_FIRST *) head)->node)
#define GENERAL_ALLOC_TOTALSIZE(head) ((HEAP_NODE_FIRST *) head)->totalsize

#define GENERAL_ALLOC_POINTER_PLUS(start, nbytes) (void *) ((index8 *) start + nbytes)


typedef struct HEAP_NODE HEAP_NODE;
typedef struct HEAP_NODE_FIRST HEAP_NODE_FIRST;
typedef HEAP_NODE * HEAP_LIST;
typedef HEAP_LIST HEAP_LIST_WITH_FIRST;

struct HEAP_NODE {
    index64 size;
    bool used;
} ATTRIBUTE_PACKED;

struct HEAP_NODE_FIRST {
    index64 totalsize;
    HEAP_NODE node;
} ATTRIBUTE_PACKED;

#endif