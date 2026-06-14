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
    printf("        │          🦦  O T T E R  v%s       │\n", OTTER_VERSION);
    printf("        │    File Organizer for the Terminal   │\n");
    printf("        ╰──────────────────────────────────────╯\n");
    printf("\n");
    printf("  USAGE\n");
    printf("      %s [OPTIONS] <directory>\n", prog);
    printf("\n");
    printf("  OPTIONS\n");
    printf("      -n, --dry-run              Preview changes without moving files\n");
    printf("      -u, --undo                 Undo the last organize operation\n");
    printf("      -V, --verbose              Show detailed per-file output\n");
    printf("      -s, --strategy <type>      Strategy: extension (default), context\n");
    printf("      -r, --rules <path>         Path to rules.toml for context strategy\n");
    printf("      -h, --help                 Show this help message\n");
    printf("      -v, --version              Show version\n");
    printf("\n");
    printf("  STRATEGIES\n");
    printf("      extension    Organize by file extension (PDF/, JPG/, ...)\n");
    printf("      context      Organize by content analysis (Academic/, Research/, ...)\n");
    printf("\n");
    printf("  EXAMPLES\n");
    printf("      %s ~/Downloads                          Organize by extension\n", prog);
    printf("      %s -s context ~/Downloads               Organize by content\n", prog);
    printf("      %s -s context -r rules.toml ~/Downloads Use custom rules\n", prog);
    printf("      %s -s context -n ~/Downloads            Context dry-run\n", prog);
    printf("      %s -u ~/Downloads                       Undo last organize\n", prog);
    printf("\n");
}

void print_version(void)
{
    printf("🦦 otter %s\n", OTTER_VERSION);
}
