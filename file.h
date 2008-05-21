#ifndef FILE_H
#define FILE_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>

typedef struct file file_t;
typedef struct version version_t;
typedef struct file_tag file_tag_t;

struct file {
    const char * path;
    const char * rcs_path;

    version_t * versions;
    version_t * versions_end;

    file_tag_t * file_tags;
    file_tag_t * file_tags_end;
};

version_t * file_new_version (file_t * f);

/// Find a file version object by the version string.  The version string @c s
/// need not be cached.
version_t * file_find_version (const file_t * f, const char * s);

/// Find a branch on which version @c s of file @c lies.
file_tag_t * file_find_branch (const file_t * f,
                               file_tag_t * const * branches,
                               file_tag_t * const * branches_end,
                               const char * s);

struct version {
    file_t * file;                      ///< File this is a version of.
    const char * version;               ///< Version string.
    bool dead;                          ///< A dead revision marking a delete.

    /// Indicate that this revision is the implicit merge of a vendor branch
    /// import to the trunk.
    bool implicit_merge;

    /// An implicit merge might not actually get used; this flag is set to
    /// indicate if the revision was actually used.
    bool used;

    /// Should this version be mode 755 instead of 644?
    bool exec;

    version_t * parent;                 ///< Previous version.
    version_t * children;               ///< A child, or NULL.
    version_t * sibling;                ///< A sibling, or NULL.

    const char * author;
    const char * commitid;
    time_t time;
    time_t offset;
    const char * log;
    file_tag_t * branch;

    struct changeset * commit;
    version_t * cs_sibling;             ///< Sibling in changeset.

    union {
        size_t ready_index;             ///< Heap index for emitting versions.
        size_t mark;                    ///< Mark during emission.
    };
};


static inline version_t * version_normalise (version_t * v)
{
    return v ? v - v->implicit_merge : v;
}


static inline version_t * version_live (version_t * v)
{
    return v && !v->dead ? v - v->implicit_merge : NULL;
}


struct file_tag {
    file_t * file;
    struct tag * tag;
    /// vers is the version information stored in cvs.  For a branch, version is
    /// the version to use as the branch point.  Version may be null.
    const char * vers;
    version_t * version;
};


/// Find a @c file_tag for the given @c file and @c tag.
file_tag_t * find_file_tag (file_t * file, struct tag * tag);


#endif
