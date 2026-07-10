/*
 * scanner.c — Directory scanning implementation.
 */

#include "otter.h"

/* ── Internal helpers ──────────────────────────────────────── */

static int filelist_push(FileList *list, const char *name, const char *rel_path)
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
    entry->rel_path  = strdup(rel_path ? rel_path : name);
    entry->extension = extract_extension(name);

    if (!entry->name || !entry->rel_path || !entry->extension) {
        perror("otter: strdup/extract_extension");
        free(entry->name);
        free(entry->rel_path);
        free(entry->extension);
        return -1;
    }

    list->count++;
    return 0;
}

/* ── Public API ────────────────────────────────────────────── */

static int scan_directory_impl(const char *root_dir, const char *rel_dir, FileList *list, int recursive)
{
    char *current_dir = rel_dir ? path_join(root_dir, rel_dir) : strdup(root_dir);
    if (!current_dir) return OTTER_ERR_STAT;

    DIR *dir = opendir(current_dir);
    if (!dir) {
        fprintf(stderr, "otter: cannot open directory '%s': %s\n",
                current_dir, strerror(errno));
        free(current_dir);
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
        char *full_path = path_join(current_dir, entry->d_name);
        if (!full_path) {
            closedir(dir);
            free(current_dir);
            return OTTER_ERR_STAT;
        }

        struct stat st;
        if (stat(full_path, &st) == -1) {
            fprintf(stderr, "otter: cannot stat '%s': %s\n",
                    full_path, strerror(errno));
            free(full_path);
            continue;  /* skip but don't abort */
        }

        char *entry_rel_path = rel_dir ? path_join(rel_dir, entry->d_name) : strdup(entry->d_name);
        if (!entry_rel_path) {
            free(full_path);
            closedir(dir);
            free(current_dir);
            return OTTER_ERR_STAT;
        }

        if (S_ISREG(st.st_mode)) {
            if (filelist_push(list, entry->d_name, entry_rel_path) != 0) {
                free(entry_rel_path);
                free(full_path);
                closedir(dir);
                free(current_dir);
                return OTTER_ERR_STAT;
            }
        } else if (S_ISDIR(st.st_mode) && recursive) {
            /* Recursively scan subdirectory. We ignore failures in subdirs to keep going. */
            scan_directory_impl(root_dir, entry_rel_path, list, recursive);
        }

        free(entry_rel_path);
        free(full_path);
    }

    closedir(dir);
    free(current_dir);
    return OTTER_OK;
}

int scan_directory(const char *dir_path, FileList *list, int recursive)
{
    return scan_directory_impl(dir_path, NULL, list, recursive);
}

void filelist_free(FileList *list)
{
    if (!list) return;
    for (size_t i = 0; i < list->count; ++i) {
        free(list->items[i].name);
        free(list->items[i].rel_path);
        free(list->items[i].extension);
    }
    free(list->items);
    list->items    = NULL;
    list->count    = 0;
    list->capacity = 0;
}
