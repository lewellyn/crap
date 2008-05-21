#include "changeset.h"
#include "database.h"
#include "file.h"
#include "utils.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


static int compare_changeset (const void * AA, const void * BB)
{
    const changeset_t * A = AA;
    const changeset_t * B = BB;

    // We emit implicit merges and branches as soon as they become ready.
    if (A->type != B->type)
        return A->type < B->type ? -1 : 1;

    if (A->time != B->time)
        return A->time > B->time;

    // That's all the ordering we really *need* to do, but we try and make
    // things as deterministic as possible.

    if (A->type != B->type)
        return A->type > B->type ? 1 : -1;

    if (A->type == ct_tag)
        return strcmp (as_tag (A)->tag, as_tag (B)->tag);

    if (A->versions->author != B->versions->author)
        return strcmp (A->versions->author, B->versions->author);

    if (A->versions->commitid != B->versions->commitid)
        return strcmp (A->versions->commitid, B->versions->commitid);

    if (A->versions->log != B->versions->log)
        return strcmp (A->versions->log, B->versions->log);

    if (A->versions->branch == NULL && B->versions->branch != NULL)
        return -1;

    if (A->versions->branch != NULL && B->versions->branch == NULL)
        return 1;

    if (A->versions->branch->tag != B->versions->branch->tag)
        return A->versions->branch->tag < B->versions->branch->tag ? -1 : 1;

    assert (A->versions->implicit_merge == B->versions->implicit_merge);

    if (A->versions->file != B->versions->file)
        return A->versions->file > B->versions->file;

    return A->versions > B->versions;
}


void database_init (database_t * db)
{
    db->files = NULL;
    db->files_end = NULL;
    db->tags = NULL;
    db->tags_end = NULL;
    db->changesets = NULL;
    db->changesets_end = NULL;
    db->tag_hash = NULL;
    db->tag_hash_num_entries = 0;
    db->tag_hash_num_buckets = 0;

    heap_init (&db->ready_changesets,
               offsetof (changeset_t, ready_index), compare_changeset);
}


void database_destroy (database_t * db)
{
    for (file_t * i = db->files; i != db->files_end; ++i) {
        free (i->versions);
        free (i->file_tags);
    }

    for (tag_t * i = db->tags; i != db->tags_end; ++i) {
        free (i->tag_files);
        free (i->branch_versions);
        free (i->tags);
        free (i->parents);
        free (i->changeset.children);
    }

    for (changeset_t ** i = db->changesets; i != db->changesets_end; ++i) {
        free ((*i)->children);
        free (*i);
    }

    free (db->files);
    free (db->tags);
    free (db->changesets);
    heap_destroy (&db->ready_changesets);
    free (db->tag_hash);
}


file_t * database_new_file (database_t * db)
{
    ARRAY_EXTEND (db->files);
    file_t * result = &db->files_end[-1];
    result->versions = NULL;
    result->versions_end = NULL;
    result->file_tags = NULL;
    result->file_tags_end = NULL;
    return result;
}


changeset_t * database_new_changeset (database_t * db)
{
    changeset_t * result = xmalloc (sizeof (changeset_t));
    changeset_init (result);

    ARRAY_APPEND (db->changesets, result);

    return result;
}


file_t * database_find_file (const database_t * db, const char * path)
{
    file_t * base = db->files;
    size_t count = db->files_end - db->files;

    while (count) {
        size_t mid = count >> 1;
        int c = strcmp (base[mid].path, path);
        if (c < 0) {
            base += mid + 1;
            count -= mid + 1;
        }
        else if (c > 0)
            count = mid;
        else
            return &base[mid];
    }

    return NULL;
}
