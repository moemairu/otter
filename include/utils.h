/*
 * utils.h — Shared utility helpers.
 */

#ifndef UTILS_H
#define UTILS_H

/*
 * extract_extension — Return the extension of `filename` in upper case.
 *
 * If the file has no extension (no '.' or ends with '.'),
 * returns a heap-allocated copy of "NO_EXT".
 * Otherwise returns a heap-allocated upper-case copy (e.g. "PDF").
 */
char *extract_extension(const char *filename);

/*
 * str_to_upper — Convert a string to upper case in-place.
 * Returns `s` for convenience.
 */
char *str_to_upper(char *s);

/*
 * path_join — Concatenate dir and name with '/' separator.
 * Returns a heap-allocated string. Caller must free().
 */
char *path_join(const char *dir, const char *name);

/* Print the Otter banner / usage information. */
void print_usage(const char *prog);

/* Print the Otter version. */
void print_version(void);

#endif /* UTILS_H */
