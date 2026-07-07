#!/usr/bin/env bash
set -euo pipefail
export COPYFILE_DISABLE=1

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build/cmake-release"
DIST_DIR="${ROOT_DIR}/dist"
STAGE_DIR="${DIST_DIR}/stage"
PKGROOT_DIR="${DIST_DIR}/pkgroot"
JUCE_PATH="${JUCE_PATH:-/Users/hostin/vibecoding/waveform-visualizer/JUCE}"
VERSION="${VERSION:-$(tr -d '[:space:]' < "${ROOT_DIR}/VERSION")}"
PKG_ID="com.buffleaudio.align"

if [[ -z "${VERSION}" ]]; then
  echo "Release version is empty. Set VERSION or update ${ROOT_DIR}/VERSION." >&2
  exit 1
fi

if [[ "${CLEAN_DIST:-0}" == "1" ]]; then
  find "${DIST_DIR}" -maxdepth 1 -type f -name "BuffleAudioAlign-*-macOS*" -delete 2>/dev/null || true
fi

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

repair_package_payload_hygiene() {
  local pkg="$1"
  local temp_dir expanded_dir payload_root repaired_pkg

  for tool in pkgutil gunzip cpio mkbom gzip xar; do
    if ! command -v "${tool}" >/dev/null 2>&1; then
      echo "${tool} is required to repair package payload hygiene." >&2
      return 1
    fi
  done

  temp_dir="$(mktemp -d "${TMPDIR:-/tmp}/buffle-pkg-repair.XXXXXX")"
  expanded_dir="${temp_dir}/expanded"
  payload_root="${temp_dir}/payload-root"
  repaired_pkg="${temp_dir}/repaired.pkg"

  pkgutil --expand "${pkg}" "${expanded_dir}"
  mkdir -p "${payload_root}"

  (
    cd "${payload_root}"
    gunzip -c "${expanded_dir}/Payload" | cpio -idm --quiet
    find . \( -name "._*" -o -name ".DS_Store" \) -delete
    mkbom . "${expanded_dir}/Bom"
    find . -print | cpio -o --format odc --quiet | gzip -c > "${expanded_dir}/Payload"
  )

  (
    cd "${expanded_dir}"
    xar -cf "${repaired_pkg}" --compression none Bom Payload PackageInfo
  )

  mv "${repaired_pkg}" "${pkg}"
  rm -rf "${temp_dir}"
}

clean_macos_metadata() {
  local path

  for path in "$@"; do
    if [[ -e "${path}" ]]; then
      find "${path}" \( -name "._*" -o -name ".DS_Store" \) -delete
      find "${path}" -exec xattr -c "{}" \;

      if command -v dot_clean >/dev/null 2>&1; then
        dot_clean -m "${path}"
      fi
    fi
  done
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

clean_macos_metadata "${STAGE_DIR}" "${PKGROOT_DIR}"

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

clean_macos_metadata "${STAGE_DIR}" "${PKGROOT_DIR}"

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

repair_package_payload_hygiene "${DIST_DIR}/BuffleAudioAlign-${VERSION}-macOS.pkg"
verify_package_payload_hygiene "${DIST_DIR}/BuffleAudioAlign-${VERSION}-macOS.pkg"

ditto -c -k --norsrc --noextattr --noqtn \
  "${STAGE_DIR}" \
  "${DIST_DIR}/BuffleAudioAlign-${VERSION}-macOS-bundles.zip"

echo "Built artifacts:"
find "${STAGE_DIR}" -mindepth 1 -maxdepth 1 -print
echo "Installer: ${DIST_DIR}/BuffleAudioAlign-${VERSION}-macOS.pkg"
echo "Bundle archive: ${DIST_DIR}/BuffleAudioAlign-${VERSION}-macOS-bundles.zip"
