#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
VERSION="${VERSION:-$(tr -d '[:space:]' < "${ROOT_DIR}/VERSION")}"
TAG="${TAG:-v${VERSION}}"
TITLE="${TITLE:-Buffle Audio Align ${VERSION}}"
PKG="${ROOT_DIR}/dist/BuffleAudioAlign-${VERSION}-macOS.pkg"
STAGE="${ROOT_DIR}/dist/stage"
ARCHIVE="${ROOT_DIR}/dist/BuffleAudioAlign-${VERSION}-macOS-bundles.zip"
RELEASE_MODE="${RELEASE_MODE:-zip}"
DRAFT_RELEASE="${DRAFT_RELEASE:-1}"
PRERELEASE="${PRERELEASE:-1}"

if [[ -z "${VERSION}" ]]; then
  echo "Release version is empty. Set VERSION or update ${ROOT_DIR}/VERSION." >&2
  exit 1
fi

case "${RELEASE_MODE}" in
  zip|pkg|full) ;;
  *)
    echo "Unsupported RELEASE_MODE=${RELEASE_MODE}. Use zip, pkg, or full." >&2
    exit 1
    ;;
esac

verify_package_payload_hygiene() {
  local pkg="$1"

  if ! command -v pkgutil >/dev/null 2>&1; then
    echo "pkgutil is required to verify package payload hygiene." >&2
    return 1
  fi

  local matches
  matches="$(pkgutil --payload-files "${pkg}" | grep -E '(^|/)\._|\.DS_Store' || true)"

  if [[ -n "${matches}" ]]; then
    echo "Package payload contains AppleDouble or .DS_Store metadata:" >&2
    echo "${matches}" >&2
    echo "Refusing to publish a dirty installer payload. Publish the clean bundle zip or rebuild a clean package first." >&2
    return 1
  fi
}

verify_archive_payload_hygiene() {
  local archive="$1"

  if ! command -v unzip >/dev/null 2>&1; then
    echo "unzip is required to verify bundle archive hygiene." >&2
    return 1
  fi

  local matches
  matches="$(unzip -l "${archive}" | grep -E '(^|/)\._|\.DS_Store' || true)"

  if [[ -n "${matches}" ]]; then
    echo "Bundle archive contains AppleDouble or .DS_Store metadata:" >&2
    echo "${matches}" >&2
    return 1
  fi
}

if [[ ! -f "${ROOT_DIR}/RELEASE_NOTES.md" ]]; then
  echo "Release notes not found: ${ROOT_DIR}/RELEASE_NOTES.md" >&2
  exit 1
fi

if [[ ! -d "${STAGE}" ]]; then
  echo "Staged bundles not found: ${STAGE}" >&2
  echo "Run scripts/build_and_package_macos.sh first." >&2
  exit 1
fi

if [[ -z "$(find "${STAGE}" -mindepth 1 -maxdepth 1 -print -quit)" ]]; then
  echo "Staged bundle directory is empty: ${STAGE}" >&2
  exit 1
fi

if git -C "${ROOT_DIR}" rev-parse --is-inside-work-tree >/dev/null 2>&1; then
  if [[ "${ALLOW_DIRTY:-0}" != "1" && -n "$(git -C "${ROOT_DIR}" status --short)" ]]; then
    echo "Working tree is dirty. Commit changes or set ALLOW_DIRTY=1 for an intentional preview publish." >&2
    git -C "${ROOT_DIR}" status --short >&2
    exit 1
  fi
fi

if [[ "${RELEASE_MODE}" == "pkg" || "${RELEASE_MODE}" == "full" ]]; then
  if [[ ! -f "${PKG}" ]]; then
    echo "Installer not found: ${PKG}" >&2
    echo "Run scripts/build_and_package_macos.sh first, or use RELEASE_MODE=zip." >&2
    exit 1
  fi

  verify_package_payload_hygiene "${PKG}"
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
verify_archive_payload_hygiene "${ARCHIVE}"

if gh release view "${TAG}" --repo "${REPO}" >/dev/null 2>&1; then
  echo "Release ${TAG} already exists in ${REPO}. Refusing to overwrite it." >&2
  exit 1
fi

assets=()
if [[ "${RELEASE_MODE}" == "pkg" || "${RELEASE_MODE}" == "full" ]]; then
  assets+=("${PKG}")
fi
if [[ "${RELEASE_MODE}" == "zip" || "${RELEASE_MODE}" == "full" ]]; then
  assets+=("${ARCHIVE}")
fi

release_flags=()
if [[ "${DRAFT_RELEASE}" == "1" ]]; then
  release_flags+=(--draft)
fi
if [[ "${PRERELEASE}" == "1" ]]; then
  release_flags+=(--prerelease)
fi

gh release create "${TAG}" \
  --repo "${REPO}" \
  --title "${TITLE}" \
  --notes-file "${ROOT_DIR}/RELEASE_NOTES.md" \
  "${release_flags[@]}" \
  "${assets[@]}"
