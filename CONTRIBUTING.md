<div align="center">

# 🤝 Contributing to Otter 🦦

### *Help us make the terminal's tidiest little helper even better!*

</div>

---

First off, thank you for considering contributing to **Otter**! It's people like you that make open source tools great. 

Otter is a lightweight CLI utility written in C that organizes files by extension. We welcome contributions of all kinds, including bug reports, feature requests, documentation improvements, and code contributions.

---

## 📑 Table of Contents

- [Code of Conduct](#-code-of-conduct)
- [Getting Started](#-getting-started)
  - [Prerequisites](#prerequisites)
  - [Build Instructions](#build-instructions)
- [How to Contribute](#-how-to-contribute)
  - [Reporting Bugs](#reporting-bugs)
  - [Suggesting Enhancements](#suggesting-enhancements)
  - [Pull Requests](#pull-requests)
- [Development Guidelines](#-development-guidelines)
  - [Code Style](#code-style)
  - [Project Structure](#project-structure)

---

## 📜 Code of Conduct

By participating in this project, you are expected to uphold a welcoming and inclusive environment. Please be respectful and considerate of others when interacting in issues and pull requests. 

---

## 🚀 Getting Started

### Prerequisites

To build and run Otter, you will need:
- A Linux environment (or compatible POSIX system).
- A C compiler supporting **C17** (e.g., `gcc` or `clang`).
- GNU `make`.

### Build Instructions

1. **Clone the repository** locally:
   ```bash
   git clone https://github.com/moemairu/otter.git
   cd otter
   ```

2. **Build the project** using `make`:
   ```bash
   # Standard release build (C17, -O2, all warnings)
   make

   # Debug build (-g, no optimization)
   make debug
   ```

3. **Run the compiled binary**:
   ```bash
   ./bin/otter --help
   ```

4. **Clean build artifacts** (when necessary):
   ```bash
   make clean
   ```

---

## 💡 How to Contribute

### 🐛 Reporting Bugs

If you find a bug, please create an issue on GitHub. Include as much detail as possible:
- Your operating system and compiler version.
- The command you ran (e.g., `otter ~/Downloads`).
- The expected behavior vs. the actual behavior.
- Any error messages or logs (you can use `--verbose` to get more details).

### ✨ Suggesting Enhancements

Have an idea for a new feature? Check the [Roadmap](README.md#🚀-roadmap) in the README first. If it's not there, open an issue to discuss it before you start writing code! This helps ensure your idea aligns with the project's goals.

### 🛠️ Pull Requests

1. **Fork the repository** on GitHub and clone your fork locally.
2. **Create a branch** for your feature or bug fix:
   ```bash
   git checkout -b feature/my-awesome-feature
   # or
   git checkout -b fix/issue-number
   ```

3. **Make your changes**. Keep your commits small, atomic, and well-documented.

4. **Test your changes**. Ensure the project still builds cleanly with `make` and `make debug`. If you add new functionality, please test it thoroughly, including edge cases (e.g., hidden files, files with no extensions, duplicate names).

5. **Push your branch** to your fork:
   ```bash
   git push origin feature/my-awesome-feature
   ```

6. **Open a Pull Request** against the `main` branch of `moemairu/otter`. Provide a clear description of the problem you are solving and how your changes address it.

---

## 📖 Development Guidelines

### 🎨 Code Style

Otter aims to be simple, fast, and dependency-free.
- **Language**: C17 Standard.
- **APIs**: POSIX standards (e.g., `opendir`, `readdir`, `stat`, `rename`, `mkdir`). Do not introduce external dependencies or non-POSIX libraries unless absolutely necessary and discussed first.
- **Warnings**: The project compiles with `-Wall -Wextra -Wpedantic`. Your code should not introduce any new compiler warnings.
- **Formatting**: Try to follow the existing code formatting. Use consistent indentation, meaningful variable names, and add comments for complex logic.

### 🏗️ Project Structure

Familiarize yourself with the project layout before diving in:

```
otter/
├── Makefile                  # Build automation
├── README.md                 # Project README
├── CONTRIBUTING.md           # This file! 🤝
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

<div align="center">

*Thank you for contributing! Let's keep those folders clean together.* 🦦🫧

</div>
