/*
 * organizer.h — File-organization interface.
 *
 * Given a FileList and a base directory, creates extension-named
 * subdirectories and moves files into them.
 */

#ifndef ORGANIZER_H
#define ORGANIZER_H

#include "scanner.h"

/* ── Types ─────────────────────────────────────────────────── */

/* Summary statistics returned after organizing. */
typedef struct {
    size_t files_scanned;     /* total regular files found           */
    size_t files_moved;       /* files successfully moved            */
    size_t files_skipped;     /* files that could not be moved       */
    size_t dirs_created;      /* new extension directories created   */
} OrganizerStats;

/* ── Public API ────────────────────────────────────────────── */

/*
 * organize_files — Create subdirectories and move files.
 *
 * `dir_path` : the target directory (must be writable).
 * `list`     : file list previously obtained from scan_directory().
 * `dry_run`  : if non-zero, only simulate (no fs changes).
 * `stats`    : output statistics (may be NULL).
 *
 * Returns OTTER_OK on success.
 */
int organize_files(const char *dir_path, const FileList *list,
                   int dry_run, OrganizerStats *stats);

/* Print a human-readable summary to stdout. */
void print_stats(const OrganizerStats *stats, int dry_run);

#endif /* ORGANIZER_H */
