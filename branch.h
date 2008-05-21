#ifndef BRANCH_H
#define BRANCH_H

#include "heap.h"

#include <sys/types.h>

struct database;
struct heap;

typedef struct branch_tag {
    struct hashed_tag * tag;
    size_t weight;
} branch_tag_t;


typedef struct parent_branch {
    struct tag * branch;
    size_t weight;
} parent_branch_t;


void branch_analyse (struct database * db);

#endif
