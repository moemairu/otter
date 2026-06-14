# ─────────────────────────────────────────────────────────────
#  Otter — Makefile
# ─────────────────────────────────────────────────────────────

CC       := gcc
CFLAGS   := -std=c17 -Wall -Wextra -Wpedantic -O2
CPPFLAGS := -Iinclude -Ivendor/tomlc99 -D_GNU_SOURCE

SRC_DIR    := src
OBJ_DIR    := build
BIN_DIR    := bin
VENDOR_DIR := vendor/tomlc99

# Project sources + vendored sources
SRCS := $(wildcard $(SRC_DIR)/*.c) $(VENDOR_DIR)/toml.c
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(wildcard $(SRC_DIR)/*.c)) \
        $(OBJ_DIR)/toml.o

TARGET := $(BIN_DIR)/otter

# ── Default target ─────────────────────────────────────────
.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

# Vendored toml.c — compile with relaxed warnings
$(OBJ_DIR)/toml.o: $(VENDOR_DIR)/toml.c | $(OBJ_DIR)
	$(CC) -std=c17 -O2 $(CPPFLAGS) -w -c -o $@ $<

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

# ── Clean ──────────────────────────────────────────────────
.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# ── Install (optional, to /usr/local/bin) ──────────────────
PREFIX ?= /usr/local

.PHONY: install
install: $(TARGET)
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 $(TARGET) $(DESTDIR)$(PREFIX)/bin/otter

.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/otter

# ── Debug build ────────────────────────────────────────────
.PHONY: debug
debug: CFLAGS := -std=c17 -Wall -Wextra -Wpedantic -g -O0 -DDEBUG
debug: clean $(TARGET)

# ── Help ───────────────────────────────────────────────────
.PHONY: help
help:
	@echo ""
	@echo "  🦦  Otter Build System"
	@echo ""
	@echo "  Targets:"
	@echo "    all       Build the otter binary (default)"
	@echo "    clean     Remove build artifacts"
	@echo "    debug     Build with debug symbols"
	@echo "    install   Install to $(PREFIX)/bin"
	@echo "    uninstall Remove from $(PREFIX)/bin"
	@echo "    help      Show this message"
	@echo ""
