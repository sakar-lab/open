#!/usr/bin/env bash
set -euo pipefail

echo "[open] Install starting..."

# --- Where the repo/script lives ----------------------------------------------
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)"

# --- Ensure config directory + .config file --------------------------------
CONF_DIR="$HOME/.config/open"
CONF_FILE="$CONF_DIR/.config"

mkdir -p "$CONF_DIR"
echo "[open] Ensured config directory: $CONF_DIR"

if [[ ! -f "$CONF_FILE" ]]; then
    touch "$CONF_FILE"
    echo "[open] Created empty config file: $CONF_FILE"
else
    echo "[open] Config file already exists: $CONF_FILE"
fi

# --- auto_completion.sh into config directory -------------------------

COMPLETION_SRC="$SCRIPT_DIR/auto_completion.sh"
COMPLETION_DEST="$CONF_DIR/auto_completion.sh"

if [[ ! -f "$COMPLETION_SRC" ]]; then
    echo "[open] ERROR: auto_completion.sh not found in repo."
    echo "       Expected at: $COMPLETION_SRC"
    exit 1
fi

cp "$COMPLETION_SRC" "$COMPLETION_DEST"
chmod +x "$COMPLETION_DEST"
echo "[open] Installed completion script to: $COMPLETION_DEST"

# --- source line to ~/.bashrc (idempotent) -----------------------------

BASHRC="$HOME/.bashrc"
SNIPPET_MARKER="# >>> open auto-completion >>>"
SNIPPET_END_MARKER="# <<< open auto-completion <<<"

if ! grep -q "$SNIPPET_MARKER" "$BASHRC" 2>/dev/null; then
    {
        echo ""
        echo "$SNIPPET_MARKER"
        echo 'if [ -f "$HOME/.config/open/auto_completion.sh" ]; then'
        echo '    . "$HOME/.config/open/auto_completion.sh"'
        echo "fi"
        echo "$SNIPPET_END_MARKER"
        echo ""
    } >> "$BASHRC"

    echo "[open] Added auto-completion snippet to $BASHRC"
else
    echo "[open] Auto-completion snippet already present in $BASHRC (skipping)"
fi

# --- Compile with CMake -------------------------------------------

BUILD_DIR="$SCRIPT_DIR/build"

echo "[open] Configuring CMake build in: $BUILD_DIR"
mkdir -p "$BUILD_DIR"
cmake -S "$SCRIPT_DIR" -B "$BUILD_DIR"

echo "[open] Building target 'open'..."
cmake --build "$BUILD_DIR" --target open --config Release

OPEN_BIN_BUILD="$BUILD_DIR/open"
if [[ ! -x "$OPEN_BIN_BUILD" ]]; then
    echo "[open] ERROR: Built binary not found at: $OPEN_BIN_BUILD"
    echo "       Adjust the script if your binary is in a different location."
    exit 1
fi

echo "[open] Built binary: $OPEN_BIN_BUILD"

# --- binary to /usr/local/bin -----------------------------------------

INSTALL_PATH="/usr/local/bin/open"

echo "[open] Installing binary to: $INSTALL_PATH (requires sudo)"
sudo cp "$OPEN_BIN_BUILD" "$INSTALL_PATH"
sudo chmod +x "$INSTALL_PATH"

echo "[open] Installed 'open' to: $INSTALL_PATH"

# --- Clean up build directory -----------------------------------------

if [[ -d "$BUILD_DIR" ]]; then
    echo "[open] Removing build directory: $BUILD_DIR"
    rm -rf "$BUILD_DIR"
fi

# --- ending ------------------------------------------------------------------
echo ""
echo "[open] Install finished!"
echo "  -> Binary: $INSTALL_PATH"
echo "  -> Config dir: $CONF_DIR"
echo "  -> Config file: $CONF_FILE"
echo "  -> Completion script: $COMPLETION_DEST"
echo ""
echo "Now reload your shell:"
echo "  source \"$BASHRC\""
echo "or open a new terminal."
echo ""
echo "Then try:"
echo "  open"
echo "  open <projectName><Tab>"
