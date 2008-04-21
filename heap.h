#ifndef HEAP_H
#define HEAP_H

#include <stddef.h>

/** Type used to store a heap.  */
typedef struct heap {
    void ** entries;
    size_t num_entries;
    size_t max_entries;
    size_t index_offset;
    /**
     * @c compare should return >0 if first arg is greater than second, and <=0
     * otherwise.  Thus either a strcmp or a '>' like predicate can be used.  */
    int (*compare) (const void *, const void *);
} heap_t;


/** Initialise a new heap.  */
void heap_init (heap_t * heap, size_t offset,
                int (*compare) (const void *, const void *));

/** Destroy a heap.  */
void heap_destroy (heap_t * heap);

/** Insert an item.  */
void heap_insert (heap_t * heap, void * item);

/** Remove an item.  */
void heap_remove (heap_t * heap, void * item);

/** Remove one item and insert another.  */
void heap_replace (heap_t * heap, void * old, void * new);

/** Return least item from a heap.  */
void * heap_front (heap_t * heap);

/** Return least item from a heap, after removing it.  */
void * heap_pop (heap_t * heap);

#endif