#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PORT="${PORT:-8088}"

python3 -m http.server "${PORT}" --bind 127.0.0.1 --directory "${ROOT_DIR}/landing"
