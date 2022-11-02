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

void memory_copy(void * dest, void * source, index64 nbytes) {
    index64 i = 0;
    for (; i < nbytes; i++) {
        ((index8 *)dest)[i] = ((index8 *)source)[i];
    }
}

void general_alloc_init(void * memory, index64 size) {
    HEAP_NODE_FIRST head;
    head.totalsize = size;
    head.node.size = size - GENERAL_ALLOC_FIRST_NODE_SIZE;
    head.node.used = false;
    memory_copy(memory, &head, sizeof(HEAP_NODE_FIRST));
}

HEAP_NODE * general_alloc_find_best_node(HEAP_LIST_WITH_FIRST head, index64 size) {
    HEAP_NODE * best = NULL;
    index64 totalsize = GENERAL_ALLOC_TOTALSIZE(head);
    HEAP_NODE * end = (HEAP_NODE *) ((index8 *) head + totalsize);

    head = GENERAL_ALLOC_START_OF_LIST(head);
    index64 best_size = totalsize - GENERAL_ALLOC_FIRST_NODE_SIZE;
    while (head != end) {
        /**
         * Find the best node which
         * -) is not used
         * -) has the smallest size possible
         * -) and has enough size
         */
        if (!head->used && head->size <= best_size && head->size >= size + GENERAL_ALLOC_NODE_SIZE) {
            best = head;
            best_size = head->size;
        }
        head = GENERAL_ALLOC_NODE_NEXT(head);
    }
    return best;
}

void * general_alloc(void * head, index64 size) {
    HEAP_NODE * best = general_alloc_find_best_node(head, size);
    if (best == NULL) { return NULL; }

    /** create a new Node that holds the spare memory of "best" / the memory of best that is not needed
     * => "best"s memory is splitted into:
     * -) used / memory needed = "best"s new size
     * -) memory of best not needed = spare = newNode
     * */
    index64 totalSizeBest = (GENERAL_ALLOC_NODE_SIZE + size); // total size of node "best" -> also size until next node
    HEAP_NODE * newNode = GENERAL_ALLOC_POINTER_PLUS(best, totalSizeBest);// (HEAP_NODE *) ((index8 *) best + totalSizeBest);
    newNode->used = false;
    newNode->size = best->size - totalSizeBest;

    // redefine "best"
    best->size = size;
    best->used = true;

    return (void *) ((int64) best + GENERAL_ALLOC_NODE_SIZE); // return the memory excluding the space for the node itself
}

HEAP_NODE * general_get_prev_node(HEAP_LIST head, HEAP_NODE * node) {
    for (; GENERAL_ALLOC_NODE_NEXT(head) != node; head = GENERAL_ALLOC_NODE_NEXT(head));
    return head;
}
bool general_find_node(HEAP_LIST head, index64 totalsize, HEAP_NODE * node) {
    HEAP_LIST end = head + totalsize;
    for (; head->used && head < end; head = GENERAL_ALLOC_NODE_NEXT(head)) {
        if (head == node) { return true; }
    }
    return false;
}

void general_free(void * head, void * toFree) {
    HEAP_NODE * node = (HEAP_NODE *) ( (int64) toFree - GENERAL_ALLOC_NODE_SIZE);
    index64 totalSize = GENERAL_ALLOC_TOTALSIZE(head);
    HEAP_NODE * end = GENERAL_ALLOC_POINTER_PLUS(head, totalSize);
    index64 totalsize = GENERAL_ALLOC_TOTALSIZE(head);

    head = GENERAL_ALLOC_START_OF_LIST(head);
    if (!general_find_node(head, totalsize, node)) { return; } // if it was not allocated
    if (head == node) {
        if (GENERAL_ALLOC_NODE_NEXT(node) != end && !GENERAL_ALLOC_NODE_NEXT(node)->used) {
            index64 totalSizeNextNode = (GENERAL_ALLOC_NODE_SIZE + GENERAL_ALLOC_NODE_NEXT(node)->size);
            node->size += totalSizeNextNode;
        }
        node->used = false;
        return;
    }
    // if possible merge with the next node
    if (GENERAL_ALLOC_NODE_NEXT(node) != end && !GENERAL_ALLOC_NODE_NEXT(node)->used) {
        index64 totalSizeNextNode = (GENERAL_ALLOC_NODE_SIZE + GENERAL_ALLOC_NODE_NEXT(node)->size);
        node->size += totalSizeNextNode;
    }
    HEAP_NODE * prev = general_get_prev_node(head, node);
    if (prev->used) { // if the previous node is already used, just set the node to "free"
        node->used = false;
        return;
    }

    // else merge the node into the previous
    index64 totalSizeNode = (GENERAL_ALLOC_NODE_SIZE + node->size);
    prev->size += totalSizeNode;
}