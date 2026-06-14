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

To build and run Otter, you will need a C compiler supporting **C17** and GNU `make`. You can install these packages on common Linux distributions:

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

### 🔌 Working with Strategies & Rules

Otter's core architecture allows extending and adding organization strategies. If you want to modify or add strategies, here's what you need to know:

#### 1. Command Line Integration
Strategies are defined and parsed in [main.c](file:///home/mole/Documents/Projects/otter/src/main.c). The `--strategy` flag accepts a string that maps to the execution logic:
- `extension` strategy calls `organize_files()` in [organizer.c](file:///home/mole/Documents/Projects/otter/src/organizer.c).
- `context` strategy calls `organize_files_context()` in [organizer.c](file:///home/mole/Documents/Projects/otter/src/organizer.c).

#### 2. Adding a New Strategy
If you want to add a new strategy (e.g., `date-based` or `checksum-based` sorting):
1. Register the strategy name in the argument parsing logic in [main.c](file:///home/mole/Documents/Projects/otter/src/main.c).
2. Create your strategy implementation header in `include/` and source in `src/`.
3. Implement a function in your source that processes the `FileList` (obtained from directory scanning) and performs directory creation and file moving.
4. Hook your function up inside `main()` after evaluating the strategy string.

#### 3. Rules & Config Parsing (`rules.toml`)
The context-aware strategy relies on parsing `rules.toml` via `tomlc99` (a lightweight, compliant TOML parser vendored in `vendor/tomlc99`).
- **Parsing Configuration**: Done in [config.c](file:///home/mole/Documents/Projects/otter/src/config.c). The parsing maps TOML entries to the `ContextConfig` struct (defined in [config.h](file:///home/mole/Documents/Projects/otter/include/config.h)).
- **Classification Logic**: Defined in [classifier.c](file:///home/mole/Documents/Projects/otter/src/classifier.c). It rates files using a score calculated from content keywords, filename keywords, and file extension matches.

If you are modifying configuration fields or changing how weights and settings work, make sure to update:
1. `ContextConfig` struct in [config.h](file:///home/mole/Documents/Projects/otter/include/config.h).
2. `config_load()` and `config_free()` in [config.c](file:///home/mole/Documents/Projects/otter/src/config.c).
3. The evaluation logic in `classify_file()` in [classifier.c](file:///home/mole/Documents/Projects/otter/src/classifier.c).

---

## 🧪 Testing Guidelines

Currently, Otter does not use a framework for automated unit testing. We rely on manual verification before commits. Here is how you should test your changes:

### 1. Manual Verification Setup
Create a messy test directory with dummy files containing various content, filenames, and extensions:
```bash
mkdir -p test_messy
touch test_messy/homework_uts.pdf
echo "This is an assignment for semester 2." > test_messy/tugas_kuliah.txt
touch test_messy/invoice_june.pdf
touch test_messy/vacation.png
```

### 2. Test Execution
Verify both strategies:

- **Test default extension strategy**:
  ```bash
  # Compile debug version
  make debug

  # Perform a dry-run and verify the output log looks correct
  ./bin/otter -n test_messy

  # Perform the move
  ./bin/otter test_messy

  # Verify the structure is correct (e.g., PDF/, TXT/, PNG/)
  # Undo the organization and verify the files return to their original locations
  ./bin/otter -u test_messy
  ```

- **Test context strategy (rules-based)**:
  ```bash
  # Dry-run with context strategy and custom rules
  ./bin/otter -s context -r ./rules.toml -n test_messy

  # Perform the organization
  ./bin/otter -s context -r ./rules.toml -V test_messy

  # Verify files are grouped into semantic categories (like Academic/, Finance/)
  # Undo the context organization
  ./bin/otter -u -V test_messy
  ```

Ensure there are no memory leaks or crashes when running these commands. You can verify memory allocation correctness under `valgrind` if available:
```bash
valgrind --leak-check=full ./bin/otter -s context -r ./rules.toml test_messy
```

---

<div align="center">

*Thank you for contributing! Let's keep those folders clean together.* 🦦🫧

</div>
