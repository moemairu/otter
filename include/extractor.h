/*
 * extractor.h — Text extraction interface.
 *
 * Extracts readable text from supported file formats (TXT, MD,
 * PDF, DOCX).  Falls back to the filename for unsupported types.
 */

#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include <stddef.h>

/* ── Limits ────────────────────────────────────────────────── */
#define EXTRACTOR_MAX_BYTES  (64 * 1024)   /* cap at 64 KB */

/* ── Types ─────────────────────────────────────────────────── */

/* Result of a text-extraction attempt. */
typedef struct {
    char  *text;          /* heap-allocated extracted text        */
    size_t length;        /* strlen(text)                        */
    int    from_content;  /* 1 = from file content, 0 = filename */
} ExtractedText;

/* ── Public API ────────────────────────────────────────────── */

/*
 * extract_text — Extract readable text from a file.
 *
 * Supported formats: TXT, MD, PDF (via pdftotext), DOCX (via pandoc).
 * For unsupported formats or on failure, populates `result` with a
 * sanitised version of the filename.
 *
 * Returns 0 on success, -1 on allocation failure.
 */
int  extract_text(const char *filepath, const char *filename,
                  const char *extension, ExtractedText *result);

/* Free all memory owned by an ExtractedText. */
void extracted_text_free(ExtractedText *result);

#endif /* EXTRACTOR_H */
