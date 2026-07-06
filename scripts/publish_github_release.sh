#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
VERSION="${VERSION:-0.3.0}"
TAG="${TAG:-v${VERSION}}"
TITLE="${TITLE:-Buffle Audio Align ${VERSION}}"
PKG="${ROOT_DIR}/dist/BuffleAudioAlign-${VERSION}-macOS.pkg"
STAGE="${ROOT_DIR}/dist/stage"
ARCHIVE="${ROOT_DIR}/dist/BuffleAudioAlign-${VERSION}-macOS-bundles.zip"

if [[ ! -f "${PKG}" ]]; then
  echo "Installer not found: ${PKG}" >&2
  echo "Run scripts/build_and_package_macos.sh first." >&2
  exit 1
fi

if ! gh auth status >/dev/null 2>&1; then
  echo "GitHub CLI is not authenticated. Run: gh auth login -h github.com" >&2
  exit 1
fi

REPO="${GITHUB_REPO:-}"
if [[ -z "${REPO}" ]]; then
  if git -C "${ROOT_DIR}" rev-parse --is-inside-work-tree >/dev/null 2>&1; then
    REPO="$(gh repo view --json nameWithOwner -q .nameWithOwner)"
  else
    echo "This directory is not a git repo. Set GITHUB_REPO=owner/name." >&2
    exit 1
  fi
fi

COPYFILE_DISABLE=1 ditto -c -k --norsrc --noextattr --keepParent "${STAGE}" "${ARCHIVE}"

gh release create "${TAG}" \
  --repo "${REPO}" \
  --title "${TITLE}" \
  --notes-file "${ROOT_DIR}/RELEASE_NOTES.md" \
  "${PKG}" \
  "${ARCHIVE}"
