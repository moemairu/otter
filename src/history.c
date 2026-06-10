/*
 * history.c — Move history implementation.
 *
 * Format of .otter_history:
 *   Each line is a tab-separated pair:  <original_path>\t<moved_path>
 *   Lines starting with '#' are comments.
 */

#include "otter.h"

/* ── Public API ────────────────────────────────────────────── */

int history_open(HistoryLog *log, const char *dir_path)
{
    log->path = path_join(dir_path, OTTER_HISTORY_FILE);
    if (!log->path) return -1;

    log->fp = fopen(log->path, "w");
    if (!log->fp) {
        fprintf(stderr, "otter: cannot create history '%s': %s\n",
                log->path, strerror(errno));
        free(log->path);
        log->path = NULL;
        return -1;
    }

    fprintf(log->fp, "# Otter history — do not edit\n");
    log->entries = 0;
    return 0;
}

int history_record(HistoryLog *log, const char *src, const char *dst)
{
    if (!log || !log->fp) return -1;

    if (fprintf(log->fp, "%s\t%s\n", src, dst) < 0) {
        perror("otter: history write");
        return -1;
    }

    log->entries++;
    return 0;
}

void history_close(HistoryLog *log)
{
    if (!log) return;
    if (log->fp) {
        fclose(log->fp);
        log->fp = NULL;
    }
    free(log->path);
    log->path = NULL;
}

/* ── Undo ──────────────────────────────────────────────────── */

/*
 * try_rmdir_if_empty — Remove a directory only if it is empty.
 */
static void try_rmdir_if_empty(const char *path)
{
    DIR *d = opendir(path);
    if (!d) return;

    int count = 0;
    struct dirent *entry;
    while ((entry = readdir(d)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 &&
            strcmp(entry->d_name, "..") != 0) {
            count++;
            break;  /* not empty */
        }
    }
    closedir(d);

    if (count == 0) {
        rmdir(path);
    }
}

/*
 * dirname_of — Return the parent directory of `path`.
 * Returns a heap-allocated string. Caller must free().
 */
static char *dirname_of(const char *path)
{
    const char *last_slash = strrchr(path, '/');
    if (!last_slash || last_slash == path) {
        return strdup(".");
    }
    size_t len = (size_t)(last_slash - path);
    char *dir = malloc(len + 1);
    if (!dir) return NULL;
    memcpy(dir, path, len);
    dir[len] = '\0';
    return dir;
}

int history_undo(const char *dir_path)
{
    char *hist_path = path_join(dir_path, OTTER_HISTORY_FILE);
    if (!hist_path) return OTTER_ERR_STAT;

    FILE *fp = fopen(hist_path, "r");
    if (!fp) {
        fprintf(stderr, "otter: no history found in '%s'\n", dir_path);
        fprintf(stderr, "  (nothing to undo)\n");
        free(hist_path);
        return OTTER_ERR_OPENDIR;
    }

    printf("\n");
    printf("  🦦  Otter — Undo\n");
    printf("  Restoring: %s\n", dir_path);
    printf("\n");

    char line[PATH_MAX * 2 + 4];
    size_t restored = 0;
    size_t failed   = 0;

    while (fgets(line, sizeof(line), fp)) {
        /* Strip newline. */
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';

        /* Skip comments and empty lines. */
        if (line[0] == '#' || line[0] == '\0') continue;

        /* Split on tab: src\tdst */
        char *tab = strchr(line, '\t');
        if (!tab) continue;

        *tab = '\0';
        const char *original = line;
        const char *current  = tab + 1;

        /* Move file back: current → original */
        if (rename(current, original) == 0) {
            /* Extract just the filename from paths for display. */
            const char *fname = strrchr(current, '/');
            fname = fname ? fname + 1 : current;

            const char *ext_dir = strrchr(original, '/');
            (void)ext_dir; /* we show the reverse arrow */

            printf("  ↩  %s  →  restored\n", fname);
            restored++;

            /* Try to remove the now-empty extension directory. */
            char *parent = dirname_of(current);
            if (parent) {
                try_rmdir_if_empty(parent);
                free(parent);
            }
        } else {
            fprintf(stderr, "  ✗  failed: %s → %s (%s)\n",
                    current, original, strerror(errno));
            failed++;
        }
    }

    fclose(fp);

    /* Remove the history file after successful undo. */
    if (failed == 0) {
        unlink(hist_path);
    }

    /* Summary */
    printf("\n");
    printf("  ──────────────────────────────────\n");
    printf("  📊  Undo Summary\n");
    printf("  ──────────────────────────────────\n");
    printf("  Files restored : %zu\n", restored);
    if (failed > 0) {
        printf("  Files failed   : %zu\n", failed);
    }
    printf("  ──────────────────────────────────\n");
    printf("\n");

    free(hist_path);
    return (failed > 0) ? OTTER_ERR_MOVE : OTTER_OK;
}
