/*
 * main.c — Entry point for the Otter CLI.
 *
 * Usage:  otter [OPTIONS] <directory>
 *
 * Options:
 *   -n, --dry-run   Simulate without moving files.
 *   -u, --undo      Undo the last organize operation.
 *   -h, --help      Show usage information.
 *   -v, --version   Show version.
 */

#include "otter.h"

int main(int argc, char *argv[])
{
    const char *dir_path = NULL;
    int         dry_run  = 0;
    int         undo     = 0;

    /* ── Argument parsing ──────────────────────────────────── */
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return OTTER_OK;
        }
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            print_version();
            return OTTER_OK;
        }
        if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--dry-run") == 0) {
            dry_run = 1;
            continue;
        }
        if (strcmp(argv[i], "-u") == 0 || strcmp(argv[i], "--undo") == 0) {
            undo = 1;
            continue;
        }
        if (argv[i][0] == '-') {
            fprintf(stderr, "otter: unknown option '%s'\n", argv[i]);
            print_usage(argv[0]);
            return OTTER_ERR_ARGS;
        }
        /* First non-option argument is the target directory. */
        if (dir_path == NULL) {
            dir_path = argv[i];
        } else {
            fprintf(stderr, "otter: unexpected argument '%s'\n", argv[i]);
            print_usage(argv[0]);
            return OTTER_ERR_ARGS;
        }
    }

    if (dir_path == NULL) {
        fprintf(stderr, "otter: no directory specified\n");
        print_usage(argv[0]);
        return OTTER_ERR_ARGS;
    }

    /* ── Undo mode ─────────────────────────────────────────── */
    if (undo) {
        return history_undo(dir_path);
    }

    /* ── Banner ────────────────────────────────────────────── */
    printf("\n");
    printf("  🦦  Otter v%s\n", OTTER_VERSION);
    printf("  Organizing: %s\n", dir_path);
    if (dry_run) {
        printf("  Mode:       dry-run (no files will be moved)\n");
    }
    printf("\n");

    /* ── Scan ──────────────────────────────────────────────── */
    FileList list = {0};
    int rc = scan_directory(dir_path, &list);
    if (rc != OTTER_OK) {
        return rc;
    }

    if (list.count == 0) {
        printf("  No files found in '%s'. Nothing to do.\n\n", dir_path);
        filelist_free(&list);
        return OTTER_OK;
    }

    /* ── Organize ──────────────────────────────────────────── */
    OrganizerStats stats = {0};
    rc = organize_files(dir_path, &list, dry_run, &stats);

    /* ── Summary ───────────────────────────────────────────── */
    print_stats(&stats, dry_run);

    /* ── Cleanup ───────────────────────────────────────────── */
    filelist_free(&list);
    return rc;
}
