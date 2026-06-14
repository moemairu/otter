/*
 * main.c — Entry point for the Otter CLI.
 *
 * Usage:  otter [OPTIONS] <directory>
 *
 * Options:
 *   -n, --dry-run           Simulate without moving files.
 *   -u, --undo              Undo the last organize operation.
 *   -V, --verbose           Show detailed per-file output.
 *   -s, --strategy <type>   Organization strategy: extension (default), context.
 *   -r, --rules <path>      Path to custom rules.toml for context strategy.
 *   -h, --help              Show usage information.
 *   -v, --version           Show version.
 */

#include "otter.h"

int main(int argc, char *argv[])
{
    const char *dir_path = NULL;
    const char *strategy = "extension";
    const char *rules    = NULL;
    int         dry_run  = 0;
    int         undo     = 0;
    int         verbose  = 0;

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
        if (strcmp(argv[i], "-V") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = 1;
            continue;
        }
        if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--strategy") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "otter: --strategy requires an argument\n");
                return OTTER_ERR_ARGS;
            }
            strategy = argv[++i];
            if (strcmp(strategy, "extension") != 0 &&
                strcmp(strategy, "context")   != 0) {
                fprintf(stderr,
                        "otter: unknown strategy '%s' "
                        "(use 'extension' or 'context')\n", strategy);
                return OTTER_ERR_ARGS;
            }
            continue;
        }
        if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--rules") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "otter: --rules requires a path argument\n");
                return OTTER_ERR_ARGS;
            }
            rules = argv[++i];
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
        return history_undo(dir_path, verbose);
    }

    /* ── Banner (verbose or dry_run) ───────────────────────── */
    if (verbose || dry_run) {
        printf("\n");
        printf("  🦦  Otter v%s\n", OTTER_VERSION);
        printf("  Organizing: %s\n", dir_path);
        printf("  Strategy:   %s\n", strategy);
        if (dry_run) {
            printf("  Mode:       dry-run (no files will be moved)\n");
        }
        printf("\n");
    }

    /* ── Scan ──────────────────────────────────────────────── */
    FileList list = {0};
    int rc = scan_directory(dir_path, &list);
    if (rc != OTTER_OK) {
        return rc;
    }

    if (list.count == 0) {
        printf("🦦 No files found. Nothing to do.\n");
        filelist_free(&list);
        return OTTER_OK;
    }

    /* ── Organize ──────────────────────────────────────────── */
    OrganizerStats stats = {0};

    if (strcmp(strategy, "context") == 0) {
        /* ── Context-based organization ────────────────────── */
        ContextConfig cfg = {0};
        if (config_load(&cfg, rules) != 0) {
            fprintf(stderr, "otter: failed to load configuration\n");
            filelist_free(&list);
            return OTTER_ERR_CONFIG;
        }

        Classifier cls;
        classifier_init(&cls, &cfg);

        rc = organize_files_context(dir_path, &list, &cls, dry_run,
                                    verbose, &stats);

        config_free(&cfg);
    } else {
        /* ── Extension-based organization (default) ────────── */
        rc = organize_files(dir_path, &list, dry_run, verbose, &stats);
    }

    /* ── Summary ───────────────────────────────────────────── */
    print_stats(&stats, dry_run, verbose);

    /* ── Cleanup ───────────────────────────────────────────── */
    filelist_free(&list);
    return rc;
}
