/*
 * history.h — Move history for undo support.
 *
 * Records file moves to a `.otter_history` file inside the
 * target directory so they can be reversed later.
 */

#ifndef HISTORY_H
#define HISTORY_H

#include <stdio.h>

/* ── Constants ─────────────────────────────────────────────── */
#define OTTER_HISTORY_FILE ".otter_history"

/* ── Types ─────────────────────────────────────────────────── */

/* Opaque handle to the history log. */
typedef struct {
    FILE  *fp;        /* open file handle          */
    char  *path;      /* full path to history file  */
    size_t entries;   /* number of entries written   */
} HistoryLog;

/* One recorded move (for undo). */
typedef struct {
    char *src;   /* original location  */
    char *dst;   /* where it was moved */
} HistoryEntry;

/* ── Public API ────────────────────────────────────────────── */

/*
 * history_open — Create/overwrite the history log in `dir_path`.
 * Returns 0 on success, -1 on failure.
 */
int  history_open(HistoryLog *log, const char *dir_path);

/* Record a single move: src → dst. */
int  history_record(HistoryLog *log, const char *src, const char *dst);

/* Close the history log. */
void history_close(HistoryLog *log);

/*
 * history_undo — Read `.otter_history` in `dir_path` and reverse
 *                all recorded moves.
 *
 * Returns OTTER_OK on success.
 */
int  history_undo(const char *dir_path, int verbose);

#endif /* HISTORY_H */
