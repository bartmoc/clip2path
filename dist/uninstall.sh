#!/usr/bin/env bash
set -euo pipefail

rm -f "$HOME/.local/bin/clip2path"
rm -f "$HOME/.local/share/applications/clip2path.desktop"
rm -f "$HOME/.local/share/icons/hicolor/scalable/apps/clip2path.svg"
rm -f "$HOME/.config/autostart/clip2path.desktop"

read -r -p "Remove config too? [y/N] " REMOVE_CONFIG

case "$REMOVE_CONFIG" in
  y|Y|yes|YES)
    rm -rf "$HOME/.config/clip2path"
    ;;
esac

echo "Uninstalled clip2path."
