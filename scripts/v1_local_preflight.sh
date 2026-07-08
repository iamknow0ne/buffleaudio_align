#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build/cmake-debug"
DIST_DIR="${ROOT_DIR}/dist"
JUCE_PATH="${JUCE_PATH:-/Users/hostin/vibecoding/waveform-visualizer/JUCE}"
VERSION="$(tr -d '[:space:]' < "${ROOT_DIR}/VERSION")"
RUN_PACKAGE=0

for arg in "$@"; do
  case "${arg}" in
    --package)
      RUN_PACKAGE=1
      ;;
    -h|--help)
      echo "Usage: scripts/v1_local_preflight.sh [--package]"
      exit 0
      ;;
    *)
      echo "Unknown option: ${arg}" >&2
      exit 2
      ;;
  esac
done

pass() {
  printf 'PASS %s\n' "$1"
}

skip() {
  printf 'SKIP %s\n' "$1"
}

require_tool() {
  if command -v "$1" >/dev/null 2>&1; then
    pass "tool available: $1"
  else
    echo "FAIL missing required tool: $1" >&2
    exit 1
  fi
}

expect_no_matches() {
  local label="$1"
  shift

  local output
  if output="$("$@" 2>&1)"; then
    if [[ -n "${output}" ]]; then
      echo "FAIL ${label}: unexpected metadata matches" >&2
      echo "${output}" >&2
      exit 1
    fi
  else
    local status=$?
    if [[ ${status} -eq 1 ]]; then
      pass "${label}"
      return
    fi

    echo "FAIL ${label}: command failed with exit ${status}" >&2
    echo "${output}" >&2
    exit "${status}"
  fi

  pass "${label}"
}

echo "Buffle Audio Align V1 local preflight"
echo "Root: ${ROOT_DIR}"
echo "Version: ${VERSION:-unknown}"
echo

if [[ -z "${VERSION}" ]]; then
  echo "FAIL VERSION is empty" >&2
  exit 1
fi

for tool in git cmake ctest pkgutil unzip codesign shasum rg bash; do
  require_tool "${tool}"
done

if [[ -d "${JUCE_PATH}" ]]; then
  pass "JUCE_PATH exists: ${JUCE_PATH}"
else
  echo "FAIL JUCE_PATH does not exist: ${JUCE_PATH}" >&2
  exit 1
fi

echo
git status --short --branch
if [[ -n "$(git status --short)" ]]; then
  skip "working tree is dirty; local preflight can continue, release publish cannot"
else
  pass "working tree clean"
fi

echo
bash -n \
  "${ROOT_DIR}/scripts/build_and_package_macos.sh" \
  "${ROOT_DIR}/scripts/publish_github_release.sh" \
  "${ROOT_DIR}/scripts/expose_landing_cloudflared.sh" \
  "${ROOT_DIR}/scripts/serve_landing.sh" \
  "${ROOT_DIR}/scripts/v1_local_preflight.sh"
pass "shell scripts parse"

echo
cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}" \
  -DJUCE_PATH="${JUCE_PATH}" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUFFLE_BUILD_TESTS=ON
cmake --build "${BUILD_DIR}" --target BuffleAlignDSPTests --config Debug --parallel
cmake --build "${BUILD_DIR}" --target BuffleAlignProcessorLatencyContractTests --config Debug --parallel
ctest --test-dir "${BUILD_DIR}" --output-on-failure
pass "DSP and processor contract tests"

if [[ "${RUN_PACKAGE}" == "1" ]]; then
  echo
  CLEAN_DIST=1 "${ROOT_DIR}/scripts/build_and_package_macos.sh"
  pass "package build"
fi

if [[ -d "${DIST_DIR}/stage" ]]; then
  echo
  for bundle in \
    "${DIST_DIR}/stage/Buffle Audio Align.app" \
    "${DIST_DIR}/stage/Buffle Audio Align.vst3" \
    "${DIST_DIR}/stage/Buffle Audio Align.component"
  do
    if [[ -d "${bundle}" ]]; then
      codesign --verify --deep --strict --verbose=2 "${bundle}"
      pass "codesign verify: $(basename "${bundle}")"
    else
      skip "missing staged bundle: $(basename "${bundle}")"
    fi
  done
else
  skip "dist/stage missing; run with --package for artifact checks"
fi

PKG="${DIST_DIR}/BuffleAudioAlign-${VERSION}-macOS.pkg"
ZIP="${DIST_DIR}/BuffleAudioAlign-${VERSION}-macOS-bundles.zip"

if [[ -f "${PKG}" ]]; then
  echo
  expect_no_matches "pkg payload has no AppleDouble or .DS_Store" \
    bash -c "pkgutil --payload-files \"${PKG}\" | rg '(^|/)\\._|\\.DS_Store'"
  pkgutil --check-signature "${PKG}" || skip "pkg is not Developer ID Installer signed"
else
  skip "missing pkg artifact: ${PKG}"
fi

if [[ -f "${ZIP}" ]]; then
  expect_no_matches "zip has no AppleDouble or .DS_Store" \
    bash -c "unzip -l \"${ZIP}\" | rg '(^|/)\\._|\\.DS_Store'"
else
  skip "missing zip artifact: ${ZIP}"
fi

if [[ -f "${PKG}" || -f "${ZIP}" ]]; then
  shasum -a 256 "${PKG}" "${ZIP}" 2>/dev/null || true
fi

echo
if command -v auval >/dev/null 2>&1; then
  skip "AU validation is interactive/host-local; run: auval -v aufx BfAl BfAu after installing the AU"
else
  skip "auval not available"
fi

PLUGINVAL_BIN="${PLUGINVAL:-}"
if [[ -z "${PLUGINVAL_BIN}" ]] && command -v pluginval >/dev/null 2>&1; then
  PLUGINVAL_BIN="$(command -v pluginval)"
fi
if [[ -z "${PLUGINVAL_BIN}" && -x "/Applications/pluginval.app/Contents/MacOS/pluginval" ]]; then
  PLUGINVAL_BIN="/Applications/pluginval.app/Contents/MacOS/pluginval"
fi

if [[ -n "${PLUGINVAL_BIN}" ]]; then
  skip "pluginval available at ${PLUGINVAL_BIN}; run strict VST3 validation manually"
else
  skip "pluginval not available on PATH"
fi

skip "Developer ID signing, notarization, clean-account install smoke, and DAW matrix require external/manual setup"
echo
pass "local preflight complete"
