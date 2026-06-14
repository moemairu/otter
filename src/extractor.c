/*
 * extractor.c — Text extraction implementation.
 *
 * Reads content from TXT, MD, PDF (via pdftotext), and
 * DOCX (via pandoc).  Falls back to sanitised filenames
 * for unsupported or unreadable formats.
 */

#include "otter.h"

/* ── Internal helpers ──────────────────────────────────────── */

/*
 * read_entire_file — Read up to EXTRACTOR_MAX_BYTES of a text file.
 * Returns a heap-allocated NUL-terminated string, or NULL on failure.
 */
static char *read_entire_file(const char *filepath, size_t *out_len)
{
    FILE *fp = fopen(filepath, "r");
    if (!fp) return NULL;

    char *buf  = malloc(EXTRACTOR_MAX_BYTES + 1);
    if (!buf) { fclose(fp); return NULL; }

    size_t n = fread(buf, 1, EXTRACTOR_MAX_BYTES, fp);
    fclose(fp);

    buf[n] = '\0';
    if (out_len) *out_len = n;
    return buf;
}

/*
 * read_from_command — Run a shell command and capture its stdout.
 * Returns a heap-allocated NUL-terminated string, or NULL on failure.
 */
static char *read_from_command(const char *cmd, size_t *out_len)
{
    FILE *pp = popen(cmd, "r");
    if (!pp) return NULL;

    size_t cap = 4096;
    size_t len = 0;
    char  *buf = malloc(cap);
    if (!buf) { pclose(pp); return NULL; }

    while (!feof(pp)) {
        if (len + 1024 > cap) {
            cap *= 2;
            if (cap > EXTRACTOR_MAX_BYTES + 1) cap = EXTRACTOR_MAX_BYTES + 1;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); pclose(pp); return NULL; }
            buf = tmp;
        }
        size_t n = fread(buf + len, 1, 1024, pp);
        len += n;
        if (len >= EXTRACTOR_MAX_BYTES) { len = EXTRACTOR_MAX_BYTES; break; }
    }

    int status = pclose(pp);
    if (status != 0 || len == 0) {
        free(buf);
        if (out_len) *out_len = 0;
        return NULL;
    }

    buf[len] = '\0';
    if (out_len) *out_len = len;
    return buf;
}

/*
 * extract_from_pdf — Use pdftotext to extract text from a PDF.
 */
static char *extract_from_pdf(const char *filepath, size_t *out_len)
{
    /* Build command: pdftotext <file> - */
    size_t cmd_len = strlen(filepath) + 64;
    char  *cmd     = malloc(cmd_len);
    if (!cmd) return NULL;

    /* Shell-escape the filepath with single quotes. */
    snprintf(cmd, cmd_len, "pdftotext '%s' - 2>/dev/null", filepath);

    char *result = read_from_command(cmd, out_len);
    free(cmd);
    return result;
}

/*
 * extract_from_docx — Use pandoc to extract text from a DOCX.
 */
static char *extract_from_docx(const char *filepath, size_t *out_len)
{
    size_t cmd_len = strlen(filepath) + 64;
    char  *cmd     = malloc(cmd_len);
    if (!cmd) return NULL;

    snprintf(cmd, cmd_len, "pandoc -t plain '%s' 2>/dev/null", filepath);

    char *result = read_from_command(cmd, out_len);
    free(cmd);
    return result;
}

/*
 * sanitize_filename_text — Convert a filename into searchable text.
 *
 * Removes the extension, replaces underscores/hyphens/dots with
 * spaces.  Returns a heap-allocated string.
 */
static char *sanitize_filename_text(const char *filename)
{
    /* Work on a copy. */
    char *text = strdup(filename);
    if (!text) return NULL;

    /* Remove extension. */
    char *dot = strrchr(text, '.');
    if (dot && dot != text) *dot = '\0';

    /* Replace separators with spaces. */
    for (char *p = text; *p; p++) {
        if (*p == '_' || *p == '-' || *p == '.') *p = ' ';
    }

    return text;
}

/*
 * str_casecmp_ext — Case-insensitive compare for extension matching.
 */
static int str_casecmp_ext(const char *a, const char *b)
{
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b))
            return 1;
        a++; b++;
    }
    return (*a != *b) ? 1 : 0;
}

/* ── Public API ────────────────────────────────────────────── */

int extract_text(const char *filepath, const char *filename,
                 const char *extension, ExtractedText *result)
{
    if (!result) return -1;
    memset(result, 0, sizeof(ExtractedText));

    char *text = NULL;
    size_t len = 0;
    int from_content = 0;

    /* Try content extraction based on extension. */
    if (str_casecmp_ext(extension, "TXT") == 0 ||
        str_casecmp_ext(extension, "MD")  == 0) {
        text = read_entire_file(filepath, &len);
        if (text) from_content = 1;

    } else if (str_casecmp_ext(extension, "PDF") == 0) {
        text = extract_from_pdf(filepath, &len);
        if (text) from_content = 1;

    } else if (str_casecmp_ext(extension, "DOCX") == 0) {
        text = extract_from_docx(filepath, &len);
        if (text) from_content = 1;
    }

    /* Fallback: sanitise the filename. */
    if (!text) {
        text = sanitize_filename_text(filename);
        if (!text) return -1;
        len = strlen(text);
        from_content = 0;
    }

    result->text         = text;
    result->length       = len;
    result->from_content = from_content;
    return 0;
}

void extracted_text_free(ExtractedText *result)
{
    if (!result) return;
    free(result->text);
    result->text   = NULL;
    result->length = 0;
}
