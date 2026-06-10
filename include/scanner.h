/*
 * scanner.h — Directory scanning interface.
 *
 * Scans a target directory and collects regular-file entries,
 * ignoring subdirectories, ".", and "..".
 */

#ifndef SCANNER_H
#define SCANNER_H

#include <stddef.h>

/* ── Types ─────────────────────────────────────────────────── */

/* Represents a single file entry discovered by the scanner. */
typedef struct {
    char *name;       /* file basename (heap-allocated)           */
    char *extension;  /* extension in UPPER CASE, or "NO_EXT"     */
} FileEntry;

/* Dynamic array of FileEntry. */
typedef struct {
    FileEntry *items;
    size_t     count;
    size_t     capacity;
} FileList;

/* ── Public API ────────────────────────────────────────────── */

/*
 * scan_directory — Populate `list` with every regular file found
 *                  directly inside `dir_path`.
 *
 * Returns OTTER_OK on success, or an OTTER_ERR_* code on failure.
 * The caller must eventually call filelist_free() on `list`.
 */
int  scan_directory(const char *dir_path, FileList *list);

/* Free all memory owned by a FileList. */
void filelist_free(FileList *list);

#endif /* SCANNER_H */
