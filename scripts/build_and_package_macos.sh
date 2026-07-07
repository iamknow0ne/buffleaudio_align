#!/usr/bin/env bash
set -euo pipefail
export COPYFILE_DISABLE=1

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build/cmake-release"
DIST_DIR="${ROOT_DIR}/dist"
STAGE_DIR="${DIST_DIR}/stage"
PKGROOT_DIR="${DIST_DIR}/pkgroot"
JUCE_PATH="${JUCE_PATH:-/Users/hostin/vibecoding/waveform-visualizer/JUCE}"
VERSION="0.3.0"
PKG_ID="com.buffleaudio.align"

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
    echo "Refusing to publish a dirty installer payload. Use the bundle zip until this is clean." >&2
    return 1
  fi
}

cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}" \
  -DCMAKE_BUILD_TYPE=Release \
  -DJUCE_PATH="${JUCE_PATH}"

cmake --build "${BUILD_DIR}" --config Release --parallel

rm -rf "${STAGE_DIR}" "${PKGROOT_DIR}"
mkdir -p "${STAGE_DIR}" "${PKGROOT_DIR}/Applications" \
  "${PKGROOT_DIR}/Library/Audio/Plug-Ins/VST3" \
  "${PKGROOT_DIR}/Library/Audio/Plug-Ins/Components"

find "${BUILD_DIR}" -name "Buffle Audio Align.app" -type d -prune -exec cp -R "{}" "${STAGE_DIR}/" \;
find "${BUILD_DIR}" -name "Buffle Audio Align.vst3" -type d -prune -exec cp -R "{}" "${STAGE_DIR}/" \;
find "${BUILD_DIR}" -name "Buffle Audio Align.component" -type d -prune -exec cp -R "{}" "${STAGE_DIR}/" \;

if [[ -d "${STAGE_DIR}/Buffle Audio Align.app" ]]; then
  ditto --noextattr --noqtn "${STAGE_DIR}/Buffle Audio Align.app" "${PKGROOT_DIR}/Applications/Buffle Audio Align.app"
fi

if [[ -d "${STAGE_DIR}/Buffle Audio Align.vst3" ]]; then
  ditto --noextattr --noqtn "${STAGE_DIR}/Buffle Audio Align.vst3" "${PKGROOT_DIR}/Library/Audio/Plug-Ins/VST3/Buffle Audio Align.vst3"
fi

if [[ -d "${STAGE_DIR}/Buffle Audio Align.component" ]]; then
  ditto --noextattr --noqtn "${STAGE_DIR}/Buffle Audio Align.component" "${PKGROOT_DIR}/Library/Audio/Plug-Ins/Components/Buffle Audio Align.component"
fi

if [[ -z "$(find "${STAGE_DIR}" -mindepth 1 -maxdepth 1 -print -quit)" ]]; then
  echo "No build artifacts were found to package." >&2
  exit 1
fi

find "${STAGE_DIR}" "${PKGROOT_DIR}" -name "._*" -delete
find "${STAGE_DIR}" "${PKGROOT_DIR}" -exec xattr -c "{}" \;

for bundle in \
  "${STAGE_DIR}/Buffle Audio Align.app" \
  "${STAGE_DIR}/Buffle Audio Align.vst3" \
  "${STAGE_DIR}/Buffle Audio Align.component" \
  "${PKGROOT_DIR}/Applications/Buffle Audio Align.app" \
  "${PKGROOT_DIR}/Library/Audio/Plug-Ins/VST3/Buffle Audio Align.vst3" \
  "${PKGROOT_DIR}/Library/Audio/Plug-Ins/Components/Buffle Audio Align.component"
do
  if [[ -d "${bundle}" ]]; then
    codesign --force --deep --sign - "${bundle}"
  fi
done

find "${STAGE_DIR}" "${PKGROOT_DIR}" -name "._*" -delete
find "${STAGE_DIR}" "${PKGROOT_DIR}" -exec xattr -c "{}" \;

pkgbuild \
  --root "${PKGROOT_DIR}" \
  --filter '(^|/)\._[^/]*$' \
  --filter '(^|/)\.DS_Store$' \
  --filter '(^|/)\.svn($|/)' \
  --filter '(^|/)CVS($|/)' \
  --identifier "${PKG_ID}" \
  --version "${VERSION}" \
  --install-location "/" \
  "${DIST_DIR}/BuffleAudioAlign-${VERSION}-macOS.pkg"

verify_package_payload_hygiene "${DIST_DIR}/BuffleAudioAlign-${VERSION}-macOS.pkg"

ditto -c -k --norsrc --noextattr --noqtn \
  "${STAGE_DIR}" \
  "${DIST_DIR}/BuffleAudioAlign-${VERSION}-macOS-bundles.zip"

echo "Built artifacts:"
find "${STAGE_DIR}" -mindepth 1 -maxdepth 1 -print
echo "Installer: ${DIST_DIR}/BuffleAudioAlign-${VERSION}-macOS.pkg"
echo "Bundle archive: ${DIST_DIR}/BuffleAudioAlign-${VERSION}-macOS-bundles.zip"
