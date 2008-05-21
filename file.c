#include "file.h"
#include "utils.h"

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

version_t * file_new_version (file_t * f)
{
    ARRAY_EXTEND (f->versions);
    f->versions_end[-1].implicit_merge = false;
    f->versions_end[-1].used = true;
    f->versions_end[-1].ready_index = SIZE_MAX;
    return &f->versions_end[-1];
}


version_t * file_find_version (const file_t * f, const char * s)
{
    version_t * base = f->versions;
    size_t count = f->versions_end - f->versions;

    while (count > 0) {
        size_t mid = count >> 1;

        int c = strcmp (base[mid].version, s);
        if (c < 0) {
            base += mid + 1;
            count -= mid + 1;
        }
        else if (c > 0)
            count = mid;
        else
            return version_normalise (base + mid);
    }

    return NULL;
}


file_tag_t * file_find_branch (const file_t * f,
                               file_tag_t * const * branches,
                               file_tag_t * const * branches_end,
                               const char * s)
{
    char vers[strlen (s) + 1];
    strcpy (vers, s);
    char * dot = strrchr (vers, '.');
    assert (dot != NULL);
    if (memchr (vers, '.', dot - vers) == NULL)
        dot = vers;                     // On trunk.

    *dot = 0;                           // Truncate the last component.

    // Now bsearch for the branch.
    file_tag_t * const * base = branches;
    ssize_t count = branches_end - branches;

    while (count > 0) {
        size_t mid = count >> 1;

        int c = strcmp (base[mid]->vers, vers);
        if (c < 0) {
            base += mid + 1;
            count -= mid + 1;
        }
        else if (c > 0)
            count = mid;
        else
            return base[mid];
    }

    fprintf (stderr, "File %s version %s (%s) has no branch\n",
             f->path, s, vers);

    return NULL;
}


file_tag_t * find_file_tag (file_t * file, tag_t * tag)
{
    file_tag_t ** base = tag->tag_files;
    size_t count = tag->tag_files_end - tag->tag_files;

    while (count > 0) {
        size_t mid = count >> 1;
        file_tag_t ** midp = base + mid;
        if (file < (*midp)->file)
            count = mid;
        else if (file > (*midp)->file) {
            base += mid + 1;
            count -= mid + 1;
        }
        else
            return *midp;
    }

    return NULL;
}
