/*
 * config.c — Context configuration implementation.
 *
 * Loads classification rules from a TOML file using tomlc99,
 * with built-in defaults as fallback.
 */

#include "otter.h"
#include "toml.h"

/* ── Internal helpers ──────────────────────────────────────── */

/*
 * add_keyword — Append a keyword to a category.
 */
static int add_keyword(Category *cat, const char *word, int weight)
{
    Keyword *tmp = realloc(cat->keywords,
                           (cat->keyword_count + 1) * sizeof(Keyword));
    if (!tmp) return -1;
    cat->keywords = tmp;

    cat->keywords[cat->keyword_count].word   = strdup(word);
    cat->keywords[cat->keyword_count].weight = weight;
    if (!cat->keywords[cat->keyword_count].word) return -1;

    cat->keyword_count++;
    return 0;
}

/*
 * add_extension — Append an extension string to a category.
 */
static int add_extension(Category *cat, const char *ext)
{
    char **tmp = realloc(cat->extensions,
                         (cat->extension_count + 1) * sizeof(char *));
    if (!tmp) return -1;
    cat->extensions = tmp;

    cat->extensions[cat->extension_count] = strdup(ext);
    if (!cat->extensions[cat->extension_count]) return -1;

    cat->extension_count++;
    return 0;
}

/*
 * add_category — Append an empty category and return a pointer to it.
 */
static Category *add_category(ContextConfig *cfg, const char *name)
{
    Category *tmp = realloc(cfg->categories,
                            (cfg->category_count + 1) * sizeof(Category));
    if (!tmp) return NULL;
    cfg->categories = tmp;

    Category *cat = &cfg->categories[cfg->category_count];
    memset(cat, 0, sizeof(Category));
    cat->name = strdup(name);
    if (!cat->name) return NULL;

    cfg->category_count++;
    return cat;
}

/*
 * Helper: add a category with keywords from static arrays.
 */
static int add_default_category(ContextConfig *cfg,
                                const char *name,
                                const char *const *keywords,
                                size_t kw_count,
                                const char *const *extensions,
                                size_t ext_count)
{
    Category *cat = add_category(cfg, name);
    if (!cat) return -1;

    for (size_t i = 0; i < kw_count; i++) {
        if (add_keyword(cat, keywords[i], 1) != 0) return -1;
    }
    for (size_t i = 0; i < ext_count; i++) {
        if (add_extension(cat, extensions[i]) != 0) return -1;
    }
    return 0;
}

/* ── Default categories ────────────────────────────────────── */

int config_load_defaults(ContextConfig *cfg)
{
    memset(cfg, 0, sizeof(ContextConfig));

    cfg->confidence_threshold = CONFIG_DEFAULT_THRESHOLD;
    cfg->weight_content       = 0.70f;
    cfg->weight_filename      = 0.20f;
    cfg->weight_extension     = 0.10f;

    /* ── Academic ──────────────────────────────────────────── */
    {
        static const char *const kw[] = {
            "assignment", "lecture", "semester", "uts", "uas",
            "praktikum", "modul", "tugas", "kuliah", "mata kuliah",
            "silabus", "rps", "materi", "pertemuan", "kuis",
            "ujian", "skripsi", "thesis", "dosen", "mahasiswa"
        };
        if (add_default_category(cfg, "Academic", kw, 20, NULL, 0) != 0)
            return -1;
    }

    /* ── Cybersecurity ─────────────────────────────────────── */
    {
        static const char *const kw[] = {
            "owasp", "pentest", "vulnerability", "exploit", "security",
            "intrusion", "malware", "firewall", "encryption", "ctf",
            "penetration testing", "sql injection", "cross site scripting",
            "xss", "brute force", "reverse engineering", "forensic"
        };
        if (add_default_category(cfg, "Cybersecurity", kw, 17, NULL, 0) != 0)
            return -1;
    }

    /* ── Research ──────────────────────────────────────────── */
    {
        static const char *const kw[] = {
            "abstract", "methodology", "results", "conclusion",
            "journal", "paper", "literature review", "hypothesis",
            "survey", "dataset", "experiment", "analysis",
            "citation", "references", "findings", "peer review"
        };
        if (add_default_category(cfg, "Research", kw, 16, NULL, 0) != 0)
            return -1;
    }

    /* ── Finance ───────────────────────────────────────────── */
    {
        static const char *const kw[] = {
            "invoice", "payment", "tax", "receipt", "balance",
            "budget", "financial", "accounting", "revenue", "expense",
            "transaction", "bank", "salary", "billing", "profit"
        };
        if (add_default_category(cfg, "Finance", kw, 15, NULL, 0) != 0)
            return -1;
    }

    /* ── Programming ───────────────────────────────────────── */
    {
        static const char *const kw[] = {
            "algorithm", "database", "api", "framework", "compiler",
            "debugging", "source code", "repository", "git", "docker",
            "function", "variable", "class", "object", "inheritance",
            "polymorphism", "syntax", "runtime"
        };
        if (add_default_category(cfg, "Programming", kw, 18, NULL, 0) != 0)
            return -1;
    }

    /* ── Design ────────────────────────────────────────────── */
    {
        static const char *const kw[] = {
            "wireframe", "mockup", "prototype", "ui", "ux",
            "figma", "typography", "layout", "branding", "color palette",
            "graphic", "illustration", "photoshop", "canva", "design system"
        };
        if (add_default_category(cfg, "Design", kw, 15, NULL, 0) != 0)
            return -1;
    }

    /* ── Images (extension-only) ───────────────────────────── */
    {
        static const char *const ext[] = {
            "jpg", "jpeg", "png", "gif", "bmp",
            "svg", "webp", "ico", "tiff", "raw"
        };
        if (add_default_category(cfg, "Images", NULL, 0, ext, 10) != 0)
            return -1;
    }

    /* ── Videos (extension-only) ───────────────────────────── */
    {
        static const char *const ext[] = {
            "mp4", "mkv", "avi", "mov", "webm",
            "flv", "wmv", "m4v"
        };
        if (add_default_category(cfg, "Videos", NULL, 0, ext, 8) != 0)
            return -1;
    }

    /* ── Archives (extension-only) ─────────────────────────── */
    {
        static const char *const ext[] = {
            "zip", "rar", "7z", "tar", "gz",
            "bz2", "xz", "iso"
        };
        if (add_default_category(cfg, "Archives", NULL, 0, ext, 8) != 0)
            return -1;
    }

    /* ── Music (extension-only) ────────────────────────────── */
    {
        static const char *const ext[] = {
            "mp3", "wav", "flac", "ogg", "aac",
            "wma", "m4a", "opus"
        };
        if (add_default_category(cfg, "Music", NULL, 0, ext, 8) != 0)
            return -1;
    }

    return 0;
}

/* ── TOML loading ──────────────────────────────────────────── */

int config_load(ContextConfig *cfg, const char *path)
{
    /* Start with defaults. */
    if (config_load_defaults(cfg) != 0) return -1;

    if (!path) return 0;

    FILE *fp = fopen(path, "r");
    if (!fp) {
        /* No file — just use defaults silently. */
        return 0;
    }

    char errbuf[256];
    toml_table_t *root = toml_parse_file(fp, errbuf, sizeof(errbuf));
    fclose(fp);

    if (!root) {
        fprintf(stderr, "otter: failed to parse '%s': %s\n", path, errbuf);
        return 0;  /* non-fatal: keep defaults */
    }

    /* ── Parse [settings] table if present ─────────────────── */
    toml_table_t *settings = toml_table_in(root, "settings");
    if (settings) {
        toml_datum_t val;

        val = toml_int_in(settings, "confidence_threshold");
        if (val.ok) cfg->confidence_threshold = (int)val.u.i;

        val = toml_double_in(settings, "weight_content");
        if (val.ok) cfg->weight_content = (float)val.u.d;

        val = toml_double_in(settings, "weight_filename");
        if (val.ok) cfg->weight_filename = (float)val.u.d;

        val = toml_double_in(settings, "weight_extension");
        if (val.ok) cfg->weight_extension = (float)val.u.d;
    }

    /* ── Parse category tables ─────────────────────────────── */
    for (int i = 0; ; i++) {
        const char *cat_name = toml_key_in(root, i);
        if (!cat_name) break;

        /* Skip the "settings" table. */
        if (strcmp(cat_name, "settings") == 0) continue;

        toml_table_t *cat_table = toml_table_in(root, cat_name);
        if (!cat_table) continue;

        /* Check if this category already exists in defaults. */
        Category *cat = NULL;
        for (size_t c = 0; c < cfg->category_count; c++) {
            if (strcasecmp(cfg->categories[c].name, cat_name) == 0) {
                cat = &cfg->categories[c];
                break;
            }
        }

        /* If not found, create a new one. */
        if (!cat) {
            cat = add_category(cfg, cat_name);
            if (!cat) { toml_free(root); return -1; }
        }

        /* Parse "keywords" array. */
        toml_array_t *kw_arr = toml_array_in(cat_table, "keywords");
        if (kw_arr) {
            for (int k = 0; ; k++) {
                toml_datum_t kw = toml_string_at(kw_arr, k);
                if (!kw.ok) break;
                add_keyword(cat, kw.u.s, 1);
                free(kw.u.s);
            }
        }

        /* Parse "extensions" array. */
        toml_array_t *ext_arr = toml_array_in(cat_table, "extensions");
        if (ext_arr) {
            for (int k = 0; ; k++) {
                toml_datum_t ext = toml_string_at(ext_arr, k);
                if (!ext.ok) break;
                add_extension(cat, ext.u.s);
                free(ext.u.s);
            }
        }

        /* Parse "weights" sub-table for keyword-specific weights. */
        toml_table_t *wt_table = toml_table_in(cat_table, "weights");
        if (wt_table) {
            for (int w = 0; ; w++) {
                const char *wkey = toml_key_in(wt_table, w);
                if (!wkey) break;
                toml_datum_t wval = toml_int_in(wt_table, wkey);
                if (!wval.ok) continue;

                /* Find the keyword in this category and update weight. */
                for (size_t ki = 0; ki < cat->keyword_count; ki++) {
                    if (strcmp(cat->keywords[ki].word, wkey) == 0) {
                        cat->keywords[ki].weight = (int)wval.u.i;
                        break;
                    }
                }
            }
        }
    }

    toml_free(root);
    return 0;
}

/* ── Cleanup ───────────────────────────────────────────────── */

void config_free(ContextConfig *cfg)
{
    if (!cfg) return;

    for (size_t i = 0; i < cfg->category_count; i++) {
        Category *cat = &cfg->categories[i];
        free(cat->name);
        for (size_t k = 0; k < cat->keyword_count; k++) {
            free(cat->keywords[k].word);
        }
        free(cat->keywords);
        for (size_t e = 0; e < cat->extension_count; e++) {
            free(cat->extensions[e]);
        }
        free(cat->extensions);
    }
    free(cfg->categories);

    cfg->categories     = NULL;
    cfg->category_count = 0;
}
