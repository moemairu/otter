/*
 * otter.h — Top-level header for the Otter file organizer.
 *
 * Pulls in every module header so translation units only need
 * #include "otter.h".
 */

#ifndef OTTER_H
#define OTTER_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>

/* ── Version ───────────────────────────────────────────────── */
#define OTTER_VERSION "1.0.0"

/* ── Limits ────────────────────────────────────────────────── */
#define OTTER_MAX_EXT_LEN  32   /* max length of an extension string */

/* ── Return codes ──────────────────────────────────────────── */
#define OTTER_OK            0
#define OTTER_ERR_ARGS      1
#define OTTER_ERR_OPENDIR   2
#define OTTER_ERR_STAT      3
#define OTTER_ERR_MKDIR     4
#define OTTER_ERR_MOVE      5
#define OTTER_ERR_UNDO      6
#define OTTER_ERR_CONFIG    7

/* ── Module headers ────────────────────────────────────────── */
#include "scanner.h"
#include "config.h"
#include "extractor.h"
#include "classifier.h"
#include "organizer.h"
#include "history.h"
#include "utils.h"

#endif /* OTTER_H */
