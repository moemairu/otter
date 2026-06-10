/*
 * organizer.c — File-organization implementation.
 *
 * Creates extension-named subdirectories and moves files into them
 * using rename(2).
 */

#include "otter.h"

/* ── Internal helpers ──────────────────────────────────────── */

/*
 * ensure_dir — Create `path` if it does not already exist.
 * Returns 0 on success, -1 on failure.
 */
static int ensure_dir(const char *path, int *was_created)
{
    struct stat st;
    if (stat(path, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            *was_created = 0;
            return 0;  /* already exists */
        }
        fprintf(stderr, "otter: '%s' exists but is not a directory\n", path);
        return -1;
    }

    if (mkdir(path, 0755) == -1) {
        fprintf(stderr, "otter: cannot create directory '%s': %s\n",
                path, strerror(errno));
        return -1;
    }

    *was_created = 1;
    return 0;
}

/*
 * generate_unique_dest — If `dest` already exists, append a numeric
 * suffix to avoid overwriting.  E.g. "photo.jpg" → "photo_1.jpg".
 *
 * Returns a heap-allocated path the caller must free().
 */
static char *generate_unique_dest(const char *dest_dir,
                                  const char *filename)
{
    char *dest = path_join(dest_dir, filename);
    if (!dest) return NULL;

    struct stat st;
    if (stat(dest, &st) != 0) {
        /* Doesn't exist yet — we're good. */
        return dest;
    }

    /* Find the last dot so we can insert _N before the extension. */
    const char *dot = strrchr(filename, '.');
    char  base[PATH_MAX];
    char  ext[OTTER_MAX_EXT_LEN + 2];  /* includes the dot */

    if (dot && dot != filename) {
        size_t base_len = (size_t)(dot - filename);
        snprintf(base, sizeof(base), "%.*s", (int)base_len, filename);
        snprintf(ext,  sizeof(ext),  "%s",   dot);
    } else {
        snprintf(base, sizeof(base), "%s", filename);
        ext[0] = '\0';
    }

    for (int n = 1; n < 10000; ++n) {
        free(dest);
        char new_name[PATH_MAX + 16];
        snprintf(new_name, sizeof(new_name), "%s_%d%s", base, n, ext);
        dest = path_join(dest_dir, new_name);
        if (!dest) return NULL;
        if (stat(dest, &st) != 0) {
            return dest;
        }
    }

    /* Extremely unlikely — give up. */
    free(dest);
    return NULL;
}

/* ── Public API ────────────────────────────────────────────── */

int organize_files(const char *dir_path, const FileList *list,
                   int dry_run, int verbose, OrganizerStats *stats)
{
    OrganizerStats local = {0};
    local.files_scanned = list->count;

    /* Open history log (only when actually moving files). */
    HistoryLog history = {0};
    int has_history = 0;
    if (!dry_run) {
        has_history = (history_open(&history, dir_path) == 0);
    }

    for (size_t i = 0; i < list->count; ++i) {
        const FileEntry *fe = &list->items[i];

        /* Build destination directory path, e.g. "/home/user/Downloads/PDF" */
        char *dest_dir = path_join(dir_path, fe->extension);
        if (!dest_dir) {
            local.files_skipped++;
            continue;
        }

        if (dry_run) {
            printf("  [dry-run]  %s  →  %s/%s\n",
                   fe->name, fe->extension, fe->name);
            local.files_moved++;
            free(dest_dir);
            continue;
        }

        /* Create the extension subdirectory if needed. */
        int was_created = 0;
        if (ensure_dir(dest_dir, &was_created) != 0) {
            local.files_skipped++;
            free(dest_dir);
            continue;
        }
        if (was_created) {
            local.dirs_created++;
        }

        /* Build source and destination full paths. */
        char *src = path_join(dir_path, fe->name);
        char *dst = generate_unique_dest(dest_dir, fe->name);

        if (!src || !dst) {
            local.files_skipped++;
            free(dest_dir);
            free(src);
            free(dst);
            continue;
        }

        /* Move the file. */
        if (rename(src, dst) == 0) {
            if (verbose) {
                printf("  ✓  %s  →  %s/\n", fe->name, fe->extension);
            }
            local.files_moved++;

            /* Record in history for undo. */
            if (has_history) {
                history_record(&history, src, dst);
            }
        } else {
            fprintf(stderr, "  ✗  %s  →  %s/ (failed: %s)\n",
                    fe->name, fe->extension, strerror(errno));
            local.files_skipped++;
        }

        free(dest_dir);
        free(src);
        free(dst);
    }

    if (has_history) {
        history_close(&history);
    }

    if (stats) *stats = local;

    return (local.files_skipped > 0) ? OTTER_ERR_MOVE : OTTER_OK;
}

void print_stats(const OrganizerStats *stats, int dry_run, int verbose)
{
    if (verbose) {
        /* Detailed summary box. */
        printf("\n");
        printf("  ──────────────────────────────────\n");
        printf("  📊  Summary%s\n", dry_run ? " (dry-run)" : "");
        printf("  ──────────────────────────────────\n");
        printf("  Files scanned : %zu\n", stats->files_scanned);
        printf("  Files moved   : %zu\n", stats->files_moved);
        if (stats->files_skipped > 0) {
            printf("  Files skipped : %zu\n", stats->files_skipped);
        }
        if (!dry_run) {
            printf("  Dirs created  : %zu\n", stats->dirs_created);
        }
        printf("  ──────────────────────────────────\n");
        printf("\n");
    } else {
        /* Clean one-liner. */
        printf("🦦 Scanned %zu file%s, created %zu folder%s, moved %zu file%s.\n",
               stats->files_scanned, stats->files_scanned == 1 ? "" : "s",
               stats->dirs_created,  stats->dirs_created  == 1 ? "" : "s",
               stats->files_moved,   stats->files_moved   == 1 ? "" : "s");
        if (stats->files_skipped > 0) {
            printf("   ⚠  %zu file%s skipped.\n",
                   stats->files_skipped, stats->files_skipped == 1 ? "" : "s");
        }
    }
}
