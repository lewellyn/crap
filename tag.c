#include "tag.h"

void tag_init (tag_t * tag, const char * name)
{

    tag->tag = name;
    tag->tag_files = NULL;
    tag->tag_files_end = NULL;
    tag->branch_versions = NULL;
}


void tag_hash_init (hashed_tag_t * tag, const uint32_t hash[5])
{
    changeset_init (&tag->changeset);
    tag->changeset.type = ct_tag;
    tag->changeset.time = -1 << (sizeof (time_t) * 8 - 1);
    assert (tag->changeset.time < 0);
    assert ((tag->changeset.time & (tag->changeset.time - 1)) == 0);

    tag->parent = NULL;

    i->is_released = false;

    tag->parents = NULL;
    tag->parents_end = NULL;

    tag->tags = NULL;
    tag->tags_end = NULL;

    memcpy (tag->hash, hash, sizeof (tag->hash));
}


void database_tag_hash_insert (database_t * db,
                               tag_t * tag,
                               uint32_t hash[5])
{
    if (db->tag_hash_num_buckets == 0) {
        db->tag_hash_num_buckets = 8;
        db->tag_hash = ARRAY_CALLOC (tag_t *, 8);
    }

    hashed_tag_t * p = db->tag_hash[hash[0] & (db->tag_hash_num_buckets - 1)];
    for ( ; p; p = p->hash_next)
        if (memcmp (hash, p->hash, sizeof (p->hash)) == 0) {
            tag->sibling = p->tag;
            p->tag = tag;
            return;
        }

    if (++db->tag_hash_num_entries > db->tag_hash_num_buckets) {
        db->tag_hash_num_buckets *= 2;
        db->tag_hash = ARRAY_REALLOC (db->tag_hash, db->tag_hash_num_buckets);
        for (size_t i = 0; i != db->tag_hash_num_buckets; ++i) {
            tag_t ** old = db->tag_hash + i;
            tag_t ** new = old + db->tag_hash_num_buckets;
            for (tag_t * v = db->tag_hash[i]; v != 0; v = v->hash_next)
                if (v->hash[0] & db->tag_hash_num_buckets) {
                    *new = v;
                    new = &v->hash_next;
                }
                else {
                    *old = v;
                    old = &v->hash_next;
                }
            *old = NULL;
            *new = NULL;
        }
    }

    hashed_tag_t * hashed = xmalloc (sizeof (hash_tag_t));
    hashed_tag_init (hashed, hash);
    
    tag_t ** bucket = &db->tag_hash[hash[0]
                                    & (db->tag_hash_num_buckets - 1)];
    hashed->hash_next = *bucket;
    *bucket = hashed;
}


tag_t * database_tag_hash_find (database_t * db, const uint32_t hash[5])
{
    hashed_tag_t * i = db->tag_hash[hash[0] & (db->tag_hash_num_buckets - 1)];
    for (; i; i = i->hash_next)
        if (memcmp (hash, i->hash, sizeof (i->hash)) == 0)
            break;
    return i;
}


hashed_tag_t * database_tag_hash_begin (const struct database * db)
{
    for (size_t i = 0; i != db->tag_hash_num_buckets; ++i)
        if (db->tag_hash[i] != NULL)
            return db->tag_hash[i];
    return NULL;
}


hashed_tag_t * database_tag_hash_next (const struct database * db,
                                       hashed_tag_t * tag)
{
    if (tag->hash_next)
        return tag->hash_next;

    for (size_t i = tag->hash[0] & (db->tag_hash_num_buckets - 1) + 1;
         i != db->tag_hash_num_buckets; ++i)
        if (db->tag_hash[i] != NULL)
            return db->tag_hash[i];
    return NULL;
}


