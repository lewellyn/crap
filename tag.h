#ifndef TAG_H
#define TAG_H

#include "changeset.h"

typedef struct tag tag_t;
typedef struct hashed_tag hashed_tag_t;

struct database;

struct tag {
    const char * tag;                   ///< The tag name.

    hashed_tag_t * hash;                ///< The tag hash entry for this tag.
    tag_t * sibling;                    ///< Sibling with same hash.

    struct file_tag ** tag_files;
    struct file_tag ** tag_files_end;

    /// This is non-NULL for branches, where a tag is considered a branch if the
    /// tag is a branch tag on any file.  It points to an array of versions, the
    /// same size as the database file array.  Each item in the slot is current
    /// version, in the emission of the branch, of the corresponding file.
    struct version ** branch_versions;

    /// Tags on this branch (if it's a branch).
    struct branch_tag * tags;
    struct branch_tag * tags_end;

    /// The last changeset committed on this branch.
    changeset_t * last;
};


struct hashed_tag {
    changeset_t changeset;              ///< Tag emission changeset.

    /// The list of tags with this hash.
    tag_t * tag;

    tag_t * branch;                     ///< Branch we leach off.
    changeset_t * parent;               ///< Changeset we leach off.

    /// The array of parent branches to this tag.  The emission process will
    /// choose one of these as the branch to put the tag on.
    struct parent_branch * parents;
    struct parent_branch * parents_end;

    /// Have we been released for emission?  A tag may be released for one
    /// of two reasons; either all it's parents have been released, or we had
    /// an exact match in the tag hash.
    bool is_released;
    
    /// Have we had an exact match from the tag hash?
    bool exact_match;

    hashed_tag_t * hash_next;           ///< Next in tag hash table.

    /// A sha-1 hash of the version information; this is used to identify
    /// identical tags, and when
    /// a set of versions exactly matching this tag has been emitted.
    uint32_t hash[5];
};


/// Initialise a @c tag with @c name.
void tag_init (tag_t * tag, const char * name);


/// Initialise a @c hashed_tag with @c name.
void hashed_tag_init (hashed_tag_t * tag, const uint32_t hash[5]);


/// Check that a changeset is a tag and return it as such.
static inline hashed_tag_t * as_tag (const changeset_t * cs)
{
    assert (cs->type == ct_tag);
    return (hashed_tag_t *) cs;
}


/// Insert a tag into the tag hash.
void database_tag_hash_insert (struct database * db,
                               tag_t * tag, const uint32_t hash[5]);

/// Find the hashed_tag matching a hash.
hashed_tag_t * database_tag_hash_find (const struct database * db,
                                       const uint32_t hash[5]);


hashed_tag_t * database_tag_hash_begin (const struct database * db);
hashed_tag_t * database_tag_hash_next (const struct database * db,
                                       hashed_tag_t * tag);


#endif
