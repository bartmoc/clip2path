#!/usr/bin/env bash
set -euo pipefail

APP_NAME="clip2path"
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

BIN_DIR="$HOME/.local/bin"
APP_DIR="$HOME/.local/share/applications"
ICON_DIR="$HOME/.local/share/icons/hicolor/scalable/apps"
CONFIG_DIR="$HOME/.config/clip2path"
AUTOSTART_DIR="$HOME/.config/autostart"

mkdir -p "$BIN_DIR" "$APP_DIR" "$ICON_DIR" "$CONFIG_DIR"

install -m 755 "$ROOT_DIR/clip2path" "$BIN_DIR/clip2path"
install -m 644 "$ROOT_DIR/clip2path.desktop" "$APP_DIR/clip2path.desktop"
install -m 644 "$ROOT_DIR/clip2path.svg" "$ICON_DIR/clip2path.svg"

if [ ! -f "$CONFIG_DIR/config.ini" ]; then
    touch "$CONFIG_DIR/config.ini"
fi

read -r -p "Enable autostart on login? [y/N] " ENABLE_AUTOSTART

case "$ENABLE_AUTOSTART" in
  y|Y|yes|YES)
    mkdir -p "$AUTOSTART_DIR"
    install -m 644 "$ROOT_DIR/clip2path.desktop" "$AUTOSTART_DIR/clip2path.desktop"
    echo "Autostart enabled."
    ;;
esac

if command -v update-desktop-database >/dev/null 2>&1; then
  update-desktop-database "$APP_DIR" >/dev/null 2>&1 || true
fi

echo "Installed clip2path."
echo "Binary: $BIN_DIR/clip2path"
echo "Desktop entry: $APP_DIR/clip2path.desktop"

pkill -f "$BIN_DIR/clip2path" 2>/dev/null || true

nohup "$BIN_DIR/clip2path" >/dev/null 2>&1 &

echo "clip2path started in background."
