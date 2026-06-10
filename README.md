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
git clone https://github.com/you/otter.git
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
| `--help` | `-h` | Show detailed help & feature list |
| `--version` | `-v` | Print version |
| `--dry-run` | `-n` | Preview changes without moving files |

### Examples

```bash
# Organize Downloads
otter ~/Downloads

# Preview what would happen (nothing gets moved)
otter --dry-run ~/Documents

# Organize the current directory
otter .
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
| 📊 **Summary Report** | Files scanned, moved, skipped, dirs created |

---

## 🔨 Build

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

## 🏗️ Project Structure

```
otter/
├── Makefile                  # Build automation
├── README.md                 # You are here! 👋
├── .gitignore
├── include/
│   ├── otter.h               # 🎯 Umbrella header (includes everything)
│   ├── scanner.h             # 🔍 Directory scanning API
│   ├── organizer.h           # 📦 File organization API
│   └── utils.h               # 🔧 Shared utility helpers
└── src/
    ├── main.c                # 🚀 CLI entry point & arg parsing
    ├── scanner.c             # 📂 opendir/readdir/stat logic
    ├── organizer.c           # 🚚 mkdir + rename logic
    └── utils.c               # 🛠️  Extension extraction, path joining
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
| **Dependencies** | None (just libc + POSIX) |

---

## 🚀 Roadmap

- [ ] Recursive directory scanning (`--recursive`)
- [ ] Watch mode with `inotify` (`--watch`)
- [ ] Custom extension mapping config file
- [ ] Undo / rollback support
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
