#ifndef GENERAL_ALLOC_H_H
#define GENERAL_ALLOC_H_H

#ifndef NO_STD_LIB
    /**
     * NO_STD_LIB is not defined
     * and we want it to stay that way,
     * but we don not want def.h to include unnecessary stuff.
     * */
    #define NO_STD_LIB
    #include "../templates/def.h"
    #undef NO_STD_LIB
#else
    #include "../templates/def.h"
#endif

#define GENERAL_ALLOC_NODE_SIZE ((int32) sizeof(GENERAL_ALLOC_NODE))
#define GENERAL_ALLOC_FIRST_NODE_SIZE ((int32) sizeof(GENERAL_ALLOC_NODE_FIRST))

#define GENERAL_ALLOC_NODE_NEXT(node) ((GENERAL_ALLOC_NODE *) ((index8 *) node + GENERAL_ALLOC_NODE_SIZE + node->size))

#define GENERAL_ALLOC_START_OF_LIST(head) &(((GENERAL_ALLOC_NODE_FIRST *) head)->node)
#define GENERAL_ALLOC_TOTALSIZE(head) ((GENERAL_ALLOC_NODE_FIRST *) head)->totalsize

#define GENERAL_ALLOC_POINTER_PLUS(start, nbytes) (void *) ((index8 *) start + nbytes)


typedef struct GENERAL_ALLOC_NODE GENERAL_ALLOC_NODE;
typedef struct GENERAL_ALLOC_NODE_FIRST GENERAL_ALLOC_NODE_FIRST;
typedef GENERAL_ALLOC_NODE * GENERAL_ALLOC_LIST;
typedef GENERAL_ALLOC_LIST GENERAL_ALLOC_LIST_WITH_FIRST;

struct GENERAL_ALLOC_NODE {
    index64 size;
    bool used;
} ATTRIBUTE_PACKED;

struct GENERAL_ALLOC_NODE_FIRST {
    index64 totalsize;
    GENERAL_ALLOC_NODE node;
} ATTRIBUTE_PACKED;

void memory_copy(void * dest, void * source, index64 nbytes);

// alloc functions

bool general_alloc_init(void * memory, index64 size);

void * general_alloc(void * head, index64 size);

bool general_free(void * head, void * toFree);

// helper functions

GENERAL_ALLOC_NODE * general_alloc_find_best_node(GENERAL_ALLOC_LIST_WITH_FIRST head, index64 size);

GENERAL_ALLOC_NODE * general_get_prev_node(GENERAL_ALLOC_LIST head, GENERAL_ALLOC_NODE * node);

bool general_find_node(GENERAL_ALLOC_LIST head, index64 totalsize, GENERAL_ALLOC_NODE * node);

#endif