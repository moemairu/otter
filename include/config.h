/*
 * config.h — Context-based organization configuration.
 *
 * Manages keyword categories and scoring weights loaded from
 * a TOML rules file, with hardcoded defaults as fallback.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>

/* ── Limits ────────────────────────────────────────────────── */
#define CONFIG_MAX_CATEGORIES   64
#define CONFIG_MAX_KEYWORDS    128
#define CONFIG_DEFAULT_THRESHOLD 3

/* ── Types ─────────────────────────────────────────────────── */

/* A single keyword with an optional weight. */
typedef struct {
    char *word;
    int   weight;       /* default = 1 */
} Keyword;

/* A classification category (e.g. "Academic", "Cybersecurity"). */
typedef struct {
    char    *name;              /* category / folder name             */
    Keyword *keywords;          /* array of keywords                  */
    size_t   keyword_count;
    char   **extensions;        /* extension-only matches (e.g. jpg)  */
    size_t   extension_count;
} Category;

/* Global configuration for the context classifier. */
typedef struct {
    Category *categories;
    size_t    category_count;
    int       confidence_threshold;   /* min score to classify       */
    float     weight_content;         /* default 0.70                */
    float     weight_filename;        /* default 0.20                */
    float     weight_extension;       /* default 0.10                */
} ContextConfig;

/* ── Public API ────────────────────────────────────────────── */

/*
 * config_load — Load configuration from a TOML file.
 *
 * If `path` is NULL or the file cannot be opened,
 * falls back to hardcoded defaults.
 *
 * Returns 0 on success, -1 on allocation failure.
 */
int  config_load(ContextConfig *cfg, const char *path);

/*
 * config_load_defaults — Populate `cfg` with built-in categories.
 * Returns 0 on success, -1 on failure.
 */
int  config_load_defaults(ContextConfig *cfg);

/* Free all memory owned by a ContextConfig. */
void config_free(ContextConfig *cfg);

#endif /* CONFIG_H */
