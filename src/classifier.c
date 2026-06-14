/*
 * classifier.c — Keyword scoring engine implementation.
 *
 * Classification pipeline:
 *   1. Extract text from file content (or filename fallback)
 *   2. Normalise text (lowercase, strip punctuation)
 *   3. Score against every category using weighted keyword matching
 *   4. Select highest-scoring category above confidence threshold
 *   5. Fall back to "Miscellaneous" if no confident match
 */

#include "otter.h"

/* ── Internal helpers ──────────────────────────────────────── */

/*
 * normalize_text — Normalise text in-place.
 *
 * - Convert to lowercase
 * - Replace punctuation with spaces
 * - Collapse multiple spaces
 * - Trim leading/trailing whitespace
 */
static void normalize_text(char *text)
{
    if (!text) return;

    /* Pass 1: lowercase + replace punctuation with space. */
    for (char *p = text; *p; p++) {
        unsigned char c = (unsigned char)*p;
        if (isupper(c)) {
            *p = (char)tolower(c);
        } else if (ispunct(c)) {
            *p = ' ';
        }
    }

    /* Pass 2: collapse multiple spaces + trim. */
    char *dst = text;
    char *src = text;
    int   prev_space = 1;  /* treat start as "after space" to trim leading */

    while (*src) {
        if (*src == ' ' || *src == '\t' || *src == '\n' || *src == '\r') {
            if (!prev_space) {
                *dst++ = ' ';
                prev_space = 1;
            }
        } else {
            *dst++ = *src;
            prev_space = 0;
        }
        src++;
    }

    /* Trim trailing space. */
    if (dst > text && *(dst - 1) == ' ') dst--;
    *dst = '\0';
}

/*
 * count_keyword_occurrences — Count how many times a keyword appears
 * in the text, as a substring match.
 *
 * Returns the number of occurrences.
 */
static int count_occurrences(const char *text, const char *keyword)
{
    if (!text || !keyword || !*keyword) return 0;

    int count = 0;
    size_t kw_len = strlen(keyword);
    const char *p = text;

    while ((p = strstr(p, keyword)) != NULL) {
        count++;
        p += kw_len;
    }

    return count;
}

/*
 * score_category — Compute the weighted score of a category
 * against the normalised content and filename texts.
 */
static float score_category(const Category *cat,
                            const char *content_text,
                            const char *filename_text,
                            const char *extension,
                            float w_content,
                            float w_filename,
                            float w_extension)
{
    float content_score  = 0.0f;
    float filename_score = 0.0f;
    float ext_score      = 0.0f;

    /* ── Keyword matching against content ─────────────────── */
    if (content_text) {
        for (size_t i = 0; i < cat->keyword_count; i++) {
            int occ = count_occurrences(content_text, cat->keywords[i].word);
            content_score += (float)(occ * cat->keywords[i].weight);
        }
    }

    /* ── Keyword matching against filename ────────────────── */
    if (filename_text) {
        for (size_t i = 0; i < cat->keyword_count; i++) {
            int occ = count_occurrences(filename_text, cat->keywords[i].word);
            filename_score += (float)(occ * cat->keywords[i].weight);
        }
    }

    /* ── Extension matching ───────────────────────────────── */
    if (extension && cat->extension_count > 0) {
        for (size_t i = 0; i < cat->extension_count; i++) {
            if (strcasecmp(extension, cat->extensions[i]) == 0) {
                /*
                 * Extension-only categories (no keywords) get a high
                 * score that always exceeds the confidence threshold.
                 * Categories with both keywords and extensions use
                 * the normal weighted score.
                 */
                if (cat->keyword_count == 0) {
                    return 100.0f;  /* definitive match */
                }
                ext_score = 10.0f;
                break;
            }
        }
    }

    return (content_score  * w_content) +
           (filename_score * w_filename) +
           (ext_score      * w_extension);
}

/*
 * get_extension_lower — Extract lowercase extension from uppercase
 * extension string.  Returns heap-allocated string.
 */
static char *ext_to_lower(const char *extension)
{
    if (!extension) return strdup("");

    char *lower = strdup(extension);
    if (!lower) return NULL;

    for (char *p = lower; *p; p++) {
        *p = (char)tolower((unsigned char)*p);
    }
    return lower;
}

/* ── Public API ────────────────────────────────────────────── */

void classifier_init(Classifier *cls, const ContextConfig *config)
{
    cls->config = config;
}

int classify_file(const Classifier *cls,
                  const char *filepath,
                  const char *filename,
                  const char *extension,
                  ClassifyResult *result)
{
    if (!cls || !cls->config || !result) return -1;
    memset(result, 0, sizeof(ClassifyResult));

    const ContextConfig *cfg = cls->config;

    /* ── Step 1: Extract text ─────────────────────────────── */
    ExtractedText extracted = {0};
    if (extract_text(filepath, filename, extension, &extracted) != 0) {
        result->category = strdup("Miscellaneous");
        result->score    = 0;
        return result->category ? 0 : -1;
    }

    /* ── Step 2: Prepare normalised texts ─────────────────── */
    char *content_norm = NULL;
    if (extracted.from_content && extracted.text) {
        content_norm = strdup(extracted.text);
        if (content_norm) normalize_text(content_norm);
    }

    /* Always normalise the filename for filename-based scoring. */
    char *fname_copy = strdup(filename);
    if (!fname_copy) {
        free(content_norm);
        extracted_text_free(&extracted);
        return -1;
    }

    /* Remove extension from filename for scoring. */
    char *dot = strrchr(fname_copy, '.');
    if (dot && dot != fname_copy) *dot = '\0';
    /* Replace separators. */
    for (char *p = fname_copy; *p; p++) {
        if (*p == '_' || *p == '-' || *p == '.') *p = ' ';
    }
    normalize_text(fname_copy);

    /* Get lowercase extension for extension matching. */
    char *ext_lower = ext_to_lower(extension);

    /* ── Step 3: Score each category ──────────────────────── */
    float best_score = -1.0f;
    int   best_idx   = -1;

    for (size_t i = 0; i < cfg->category_count; i++) {
        float score = score_category(&cfg->categories[i],
                                     content_norm,
                                     fname_copy,
                                     ext_lower,
                                     cfg->weight_content,
                                     cfg->weight_filename,
                                     cfg->weight_extension);

        if (score > best_score) {
            best_score = score;
            best_idx   = (int)i;
        }
    }

    /* ── Step 4: Apply confidence threshold ───────────────── */
    if (best_idx >= 0 && best_score >= (float)cfg->confidence_threshold) {
        result->category = strdup(cfg->categories[best_idx].name);
        result->score    = (int)(best_score + 0.5f);  /* round */
    } else {
        result->category = strdup("Miscellaneous");
        result->score    = (int)(best_score + 0.5f);
    }

    /* ── Cleanup ──────────────────────────────────────────── */
    free(content_norm);
    free(fname_copy);
    free(ext_lower);
    extracted_text_free(&extracted);

    return result->category ? 0 : -1;
}

void classify_result_free(ClassifyResult *result)
{
    if (!result) return;
    free(result->category);
    result->category = NULL;
    result->score    = 0;
}
