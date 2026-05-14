#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cmake -B "${SCRIPT_DIR}/build" -S "${SCRIPT_DIR}"
cmake --build "${SCRIPT_DIR}/build"

cp "${SCRIPT_DIR}/build/clip2path" "${SCRIPT_DIR}/dist/clip2path"

echo "Build complete: ${SCRIPT_DIR}/dist/clip2path"
