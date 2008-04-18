#include "file.h"
#include "xmalloc.h"

#include <string.h>
#include <sys/types.h>

version_t * file_new_version (file_t * f)
{
    f->versions = xrealloc (f->versions,
                            ++f->num_versions * sizeof (version_t));
    return f->versions + f->num_versions - 1;
}


file_tag_t * file_new_file_tag (file_t * f)
{
    f->file_tags = xrealloc (f->file_tags,
                             ++f->num_file_tags * sizeof (file_tag_t));
    return f->file_tags + f->num_file_tags - 1;
}


version_t * file_find_version (const file_t * f, const char * s)
{
    version_t * base = f->versions;
    ssize_t count = f->num_versions;

    while (count > 0) {
        size_t mid = count >> 1;

        int c = strcmp (base[mid].version, s);
        if (c == 0)
            return base + mid;

        if (c < 0) {
            base += mid + 1;
            count = count - mid - 1;
        }
        else {
            count = mid - 1;
        }
    }

    return NULL;
}