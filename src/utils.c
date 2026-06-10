/*
 * utils.c — Shared utility helpers.
 */

#include "otter.h"

char *extract_extension(const char *filename)
{
    const char *dot = strrchr(filename, '.');

    /*
     * No extension if:
     *   – there is no '.'
     *   – the '.' is the first character (hidden file like ".bashrc")
     *   – the '.' is the last character ("Makefile.")
     */
    if (!dot || dot == filename || *(dot + 1) == '\0') {
        return strdup("NO_EXT");
    }

    /* Skip the dot and duplicate the rest. */
    char *ext = strdup(dot + 1);
    if (!ext) return NULL;

    return str_to_upper(ext);
}

char *str_to_upper(char *s)
{
    if (!s) return NULL;
    for (char *p = s; *p; ++p) {
        *p = (char)toupper((unsigned char)*p);
    }
    return s;
}

char *path_join(const char *dir, const char *name)
{
    size_t dir_len  = strlen(dir);
    size_t name_len = strlen(name);

    /* +2: one for '/', one for '\0' */
    char *path = malloc(dir_len + name_len + 2);
    if (!path) {
        perror("otter: malloc");
        return NULL;
    }

    memcpy(path, dir, dir_len);

    /* Avoid double slash. */
    if (dir_len > 0 && dir[dir_len - 1] != '/') {
        path[dir_len] = '/';
        memcpy(path + dir_len + 1, name, name_len + 1);
    } else {
        memcpy(path + dir_len, name, name_len + 1);
    }

    return path;
}

void print_usage(const char *prog)
{
    printf("\n");
    printf("        ╭──────────────────────────────────────╮\n");
    printf("        │          🦦  O T T E R  v%s        │\n", OTTER_VERSION);
    printf("        │    File Organizer for the Terminal   │\n");
    printf("        ╰──────────────────────────────────────╯\n");
    printf("\n");
    printf("  DESCRIPTION\n");
    printf("      Otter scans a directory, detects file extensions,\n");
    printf("      creates matching folders, and moves each file into\n");
    printf("      its extension folder — all in one command.\n");
    printf("\n");
    printf("      Messy Downloads/ folder? Let this little otter\n");
    printf("      clean it up for you. 🫧\n");
    printf("\n");
    printf("  USAGE\n");
    printf("      %s [OPTIONS] <directory>\n", prog);
    printf("\n");
    printf("  OPTIONS\n");
    printf("      -n, --dry-run    Simulate the organization without\n");
    printf("                       actually moving any files. Great for\n");
    printf("                       previewing what otter would do.\n");
    printf("\n");
    printf("      -h, --help       Show this help message and exit.\n");
    printf("\n");
    printf("      -v, --version    Show otter version and exit.\n");
    printf("\n");
    printf("  FEATURES\n");
    printf("      ✦  Scans all regular files in the target directory\n");
    printf("      ✦  Skips subdirectories (non-recursive)\n");
    printf("      ✦  Case-insensitive extension detection\n");
    printf("         (.pdf, .PDF, .Pdf → all go to PDF/)\n");
    printf("      ✦  Files without extensions → NO_EXT/\n");
    printf("      ✦  Hidden files (.bashrc, .gitignore) → NO_EXT/\n");
    printf("      ✦  Handles duplicate filenames automatically\n");
    printf("         (photo.jpg → photo_1.jpg, photo_2.jpg, ...)\n");
    printf("      ✦  Summary report after each run\n");
    printf("\n");
    printf("  EXAMPLES\n");
    printf("      Organize your Downloads folder:\n");
    printf("          %s ~/Downloads\n", prog);
    printf("\n");
    printf("      Preview what would happen (no files moved):\n");
    printf("          %s --dry-run ~/Downloads\n", prog);
    printf("\n");
    printf("      Organize the current directory:\n");
    printf("          %s .\n", prog);
    printf("\n");
    printf("  WHAT HAPPENS\n");
    printf("      Before:                    After:\n");
    printf("      Downloads/                 Downloads/\n");
    printf("      ├── tugas.pdf              ├── PDF/\n");
    printf("      ├── foto.jpg               │   └── tugas.pdf\n");
    printf("      ├── video.mp4              ├── JPG/\n");
    printf("      └── arsip.zip              │   └── foto.jpg\n");
    printf("                                 ├── MP4/\n");
    printf("                                 │   └── video.mp4\n");
    printf("                                 └── ZIP/\n");
    printf("                                     └── arsip.zip\n");
    printf("\n");
    printf("  Made with 💙 — one job, done well. 🦦\n");
    printf("\n");
}

void print_version(void)
{
    printf("🦦 otter %s\n", OTTER_VERSION);
}
