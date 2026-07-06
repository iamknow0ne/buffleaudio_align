#!/usr/bin/env bash
set -euo pipefail

PORT="${PORT:-8088}"
CLOUDFLARED="${CLOUDFLARED:-/opt/homebrew/bin/cloudflared}"

if [[ ! -x "${CLOUDFLARED}" ]]; then
  echo "cloudflared was not found at ${CLOUDFLARED}." >&2
  echo "Install with: brew install cloudflared" >&2
  exit 1
fi

exec "${CLOUDFLARED}" tunnel --url "http://127.0.0.1:${PORT}"
