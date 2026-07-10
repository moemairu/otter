<div align="center">

# 🦦 Otter

### *Your terminal's tidiest little helper.*

**Organize files by extension — fast, simple, Unix-style.**

[![Language](https://img.shields.io/badge/C-C17-blue?style=flat-square&logo=c)](https://en.cppreference.com/w/c/17)
[![Platform](https://img.shields.io/badge/Platform-Linux-yellow?style=flat-square&logo=linux&logoColor=white)](https://kernel.org)
[![License](https://img.shields.io/badge/License-MIT-green?style=flat-square)](LICENSE)

</div>

---

> *"One command. Clean folder. Happy otter."* 🫧

Otter is a lightweight CLI utility written in C that scans a directory and
automatically sorts files into subdirectories based on their file extension.
Think of it as Marie Kondo for your filesystem — except it's an otter, and
it really likes organizing things.

---

## ⚡ Quick Start

```bash
# Clone & build
git clone https://github.com/moemairu/otter.git
cd otter && make

# Let the otter loose on your messy folder
./bin/otter ~/Downloads
```

That's it. Your files are now organized. The otter is pleased. 🦦

---

## 🎬 Before & After

```
 BEFORE                           AFTER
 ──────                           ─────
 Downloads/                       Downloads/
 ├── tugas.pdf                    ├── PDF/
 ├── foto.jpg                     │   └── tugas.pdf
 ├── video.mp4                    ├── JPG/
 ├── script.py                    │   └── foto.jpg
 ├── data.csv                     ├── MP4/
 └── arsip.zip                    │   └── video.mp4
                                  ├── PY/
                                  │   └── script.py
                                  ├── CSV/
                                  │   └── data.csv
                                  └── ZIP/
                                      └── arsip.zip
```

*Messy → Tidy in under a second.* ✨

---

## 📖 Usage

```
otter [OPTIONS] <directory>
```

| Flag | Short | Description |
|------|-------|-------------|
| `--help` | `-h` | Show help & usage |
| `--version` | `-v` | Print version |
| `--dry-run` | `-n` | Preview changes without moving files |
| `--undo` | `-u` | Undo the last organize operation |
| `--verbose` | `-V` | Show detailed per-file output |
| `--strategy <type>` | `-s` | Organization strategy: `extension` (default) or `context` |
| `--rules <path>` | `-r` | Path to custom rules config (defaults to `./rules.toml` for context) |

### Examples

```bash
# Organize Downloads using the default extension strategy
otter ~/Downloads

# Organize using the context-aware strategy with rules.toml
otter --strategy context --rules ./rules.toml ~/Downloads

# Preview what would happen (dry-run) with context strategy
otter -s context -r ./rules.toml -n ~/Downloads

# Oops! Undo the last organize operation
otter --undo ~/Downloads
```

---

## ✦ Features

| Feature | Details |
|---------|---------|
| 📂 **Directory Scanning** | Scans all regular files, skips subdirectories |
| 🔍 **Smart Extension Detection** | Case-insensitive (`.pdf`, `.PDF`, `.Pdf` → `PDF/`) |
| 📁 **Auto Folder Creation** | Creates extension folders on the fly |
| 🚚 **File Organization** | Moves files into matching folders via `rename(2)` |
| 👻 **Hidden File Handling** | `.bashrc`, `.gitignore` → `NO_EXT/` |
| 📄 **No-Extension Support** | Files without extensions → `NO_EXT/` |
| 🔄 **Duplicate Safety** | `photo.jpg` → `photo_1.jpg`, `photo_2.jpg`, ... |
| 👀 **Dry-Run Mode** | See what would happen without touching anything |
| ↩️ **Undo Support** | Reverse the last organize with `--undo` |
| 📢 **Verbose Mode** | Detailed per-file logs and summary with `--verbose` |
| 📊 **Summary Report** | Clean one-liner by default, full box in verbose mode |
| ⚙️ **Custom Rules** | Map specific extensions via `rules.toml` |

---

## 🔨 Build

### Prerequisites

You need a C compiler supporting C17 (like GCC or Clang) and GNU Make. You can install them on common Linux distributions using:

- **Debian / Ubuntu / Pop!_OS:**
  ```bash
  sudo apt update && sudo apt install build-essential
  ```
- **Arch Linux / Manjaro / EndeavourOS:**
  ```bash
  sudo pacman -S base-devel
  ```
- **Fedora / RHEL:**
  ```bash
  sudo dnf groupinstall "Development Tools"
  ```

### Compiling Otter

```bash
make              # Release build (C17, -O2, all warnings)
make debug        # Debug build (-g, no optimization)
make clean        # Remove build artifacts
```

### Install System-Wide

```bash
sudo make install       # → /usr/local/bin/otter
sudo make uninstall     # Remove it
```

---

## 🧠 How It Works

```
   ┌──────────┐     ┌──────────┐     ┌──────────┐     ┌──────────┐     ┌──────────┐
   │  1.SCAN  │────▶│ 2.DETECT │────▶│ 3.CREATE │────▶│  4.MOVE  │────▶│ 5.REPORT │
   │          │     │          │     │          │     │          │     │          │
   │ Read all │     │ Extract  │     │ mkdir()  │     │ rename() │     │ Print    │
   │ files in │     │ extension│     │ for each │     │ file to  │     │ summary  │
   │ the dir  │     │ per file │     │ ext type │     │ its dir  │     │ stats    │
   └──────────┘     └──────────┘     └──────────┘     └──────────┘     └──────────┘
```

---

## 💡 Organization Strategies & Rules

Otter supports two main organization strategies:

1. **`extension` (Default)**: A simple strategy that organizes files purely by their file extensions (e.g. `document.pdf` -> `PDF/`, `image.png` -> `PNG/`).
2. **`context` (Rule-Based)**: A smarter strategy that reads file content, filenames, and extensions to classify them into conceptual folders (like `Academic`, `Research`, `Finance`) defined in `rules.toml`.

### ⚙️ Context Classification via `rules.toml`

When using `--strategy context`, Otter evaluates files based on keywords and scores defined in a TOML config file.

#### Example Config (`rules.toml`)
```toml
[settings]
confidence_threshold = 3        # Minimum score to classify a file
weight_content       = 0.70     # Weight for content matches
weight_filename      = 0.20     # Weight for filename matches
weight_extension     = 0.10     # Weight for extension matches

[Academic]
keywords = ["assignment", "lecture", "semester", "uts", "uas", "tugas"]
extensions = ["pdf", "docx"]

[Academic.weights]
uts = 5                         # Higher weight for specific keywords
uas = 5
```

#### How Scoring Works:
1. **Extension Match**: If a file's extension matches a category's list of extensions, the category receives a score.
2. **Filename Match**: Otter scans the filename for keywords and aggregates their weights.
3. **Content Match**: Otter scans the file's text content (first few KB) for keywords and aggregates their weights.
4. **Final Evaluation**: The category with the highest weighted score above the `confidence_threshold` is chosen. If no category meets the threshold, the file is placed in `Unclassified/` or defaults back to its extension folder.

---

## 🏗️ Project Structure

```
otter/
├── Makefile                  # Build automation
├── README.md                 # You are here! 👋
├── CONTRIBUTING.md           # Contribution guidelines 🤝
├── rules.toml                # Custom extension mapping config
├── .gitignore
├── include/
│   ├── otter.h               # 🎯 Umbrella header (includes everything)
│   ├── config.h              # ⚙️ TOML configuration parsing
│   ├── classifier.h          # 🗂️ Extension-to-folder mapping logic
│   ├── extractor.h           # ✂️ File extension extraction
│   ├── scanner.h             # 🔍 Directory scanning API
│   ├── organizer.h           # 📦 File organization API
│   ├── history.h             # ↩️ Undo / history API
│   └── utils.h               # 🔧 Shared utility helpers
├── src/
│   ├── main.c                # 🚀 CLI entry point & arg parsing
│   ├── config.c              # ⚙️ rules.toml parser
│   ├── classifier.c          # 🗂️ Classification logic
│   ├── extractor.c           # ✂️ Extractor logic
│   ├── scanner.c             # 📂 opendir/readdir/stat logic
│   ├── organizer.c           # 🚚 mkdir + rename logic
│   ├── history.c             # ↩️ Move history & undo logic
│   └── utils.c               # 🛠️ Path manipulation helpers
└── vendor/
    └── tomlc99/              # 📚 Vendored TOML parser
```

---

## 🛠️ Technical Details

| | |
|---|---|
| **Language** | C (C17 standard) |
| **Platform** | Linux |
| **APIs** | POSIX — `opendir`, `readdir`, `stat`, `rename`, `mkdir` |
| **Build** | GNU Make |
| **Compiler Flags** | `-std=c17 -Wall -Wextra -Wpedantic` |
| **Dependencies** | tomlc99 (vendored) |

---

## 🚀 Roadmap

- [x] ~~Undo / rollback support~~ ✅
- [x] ~~Custom extension mapping config file~~ ✅
- [x] ~~Recursive directory scanning (`--recursive`)~~ ✅
- [ ] Watch mode with `inotify` (`--watch`)
- [ ] Usage statistics & logging
- [ ] Colorized terminal output

---

## 📜 License

MIT — do whatever you want. Just don't blame the otter. 🦦

---

<div align="center">

*Otter does one job well: organizing files by extension.*

**One command. Clean folder. Happy otter.** 🦦🫧

</div>
