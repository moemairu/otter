/*
 * classifier.h — Keyword-based file classification engine.
 *
 * Scores files against configured categories using content,
 * filename, and extension matching with configurable weights.
 */

#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include "config.h"

/* ── Types ─────────────────────────────────────────────────── */

/* Classification result for a single file. */
typedef struct {
    char *category;       /* chosen category name (heap-allocated)  */
    int   score;          /* raw highest score                      */
} ClassifyResult;

/* Classifier instance — wraps a loaded configuration. */
typedef struct {
    const ContextConfig *config;
} Classifier;

/* ── Public API ────────────────────────────────────────────── */

/* Initialise a classifier with a loaded ContextConfig. */
void classifier_init(Classifier *cls, const ContextConfig *config);

/*
 * classify_file — Determine the category of a single file.
 *
 * Extracts text, normalises it, scores against every category,
 * and writes the winning category into `result`.
 *
 * Returns 0 on success, -1 on failure.
 */
int  classify_file(const Classifier *cls,
                   const char *filepath,
                   const char *filename,
                   const char *extension,
                   ClassifyResult *result);

/* Free memory owned by a ClassifyResult. */
void classify_result_free(ClassifyResult *result);

#endif /* CLASSIFIER_H */
