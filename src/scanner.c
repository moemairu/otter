/*
 * scanner.c — Directory scanning implementation.
 */

#include "otter.h"

/* ── Internal helpers ──────────────────────────────────────── */

static int filelist_push(FileList *list, const char *name)
{
    /* Grow if necessary (doubling strategy). */
    if (list->count == list->capacity) {
        size_t new_cap = list->capacity ? list->capacity * 2 : 16;
        FileEntry *tmp = realloc(list->items, new_cap * sizeof(FileEntry));
        if (!tmp) {
            perror("otter: realloc");
            return -1;
        }
        list->items    = tmp;
        list->capacity = new_cap;
    }

    FileEntry *entry = &list->items[list->count];
    entry->name      = strdup(name);
    entry->extension = extract_extension(name);

    if (!entry->name || !entry->extension) {
        perror("otter: strdup/extract_extension");
        free(entry->name);
        free(entry->extension);
        return -1;
    }

    list->count++;
    return 0;
}

/* ── Public API ────────────────────────────────────────────── */

int scan_directory(const char *dir_path, FileList *list)
{
    DIR *dir = opendir(dir_path);
    if (!dir) {
        fprintf(stderr, "otter: cannot open directory '%s': %s\n",
                dir_path, strerror(errno));
        return OTTER_ERR_OPENDIR;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        /* Skip "." and ".." */
        if (strcmp(entry->d_name, ".")  == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        /* Build full path and stat() to confirm it is a regular file. */
        char *full_path = path_join(dir_path, entry->d_name);
        if (!full_path) {
            closedir(dir);
            return OTTER_ERR_STAT;
        }

        struct stat st;
        if (stat(full_path, &st) == -1) {
            fprintf(stderr, "otter: cannot stat '%s': %s\n",
                    full_path, strerror(errno));
            free(full_path);
            continue;  /* skip but don't abort */
        }

        if (S_ISREG(st.st_mode)) {
            if (filelist_push(list, entry->d_name) != 0) {
                free(full_path);
                closedir(dir);
                return OTTER_ERR_STAT;
            }
        }

        free(full_path);
    }

    closedir(dir);
    return OTTER_OK;
}

void filelist_free(FileList *list)
{
    if (!list) return;
    for (size_t i = 0; i < list->count; ++i) {
        free(list->items[i].name);
        free(list->items[i].extension);
    }
    free(list->items);
    list->items    = NULL;
    list->count    = 0;
    list->capacity = 0;
}
