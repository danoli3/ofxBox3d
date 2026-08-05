#!/usr/bin/env bash
set -euo pipefail

BOX3D_VERSION="${BOX3D_VERSION:-0.1.0}"
LIBS_TAG="${LIBS_TAG:-libs-v${BOX3D_VERSION}}"

platform_field() {
  case "$1:$2" in
    osx:apo_type) echo osx ;; osx:arch) echo MAC ;; osx:of_path) echo osx ;; osx:library) echo libbox3d.a ;;
    linux64:apo_type) echo linux ;; linux64:arch) echo 64 ;; linux64:of_path) echo linux64 ;; linux64:library) echo libbox3d.a ;;
    vs-x64:apo_type) echo vs ;; vs-x64:arch) echo x64 ;; vs-x64:of_path) echo vs/x64 ;; vs-x64:library) echo box3d.lib ;;
    *) return 1 ;;
  esac
}

platform_ids() { printf '%s\n' osx linux64 vs-x64; }

detect_host() {
  case "$(uname -s)" in
    Darwin) echo osx ;;
    Linux) echo linux64 ;;
    MINGW*|MSYS*|CYGWIN*) echo vs-x64 ;;
    *) return 1 ;;
  esac
}

asset_name() { echo "box3d-${BOX3D_VERSION}-$1.tar.gz"; }

if [[ "${BASH_SOURCE[0]}" == "$0" ]]; then
  printf '%-12s %-10s %s\n' PACKAGE_ID APOTHECARY OF_PATH
  for id in $(platform_ids); do
    printf '%-12s %-10s %s\n' "$id" "$(platform_field "$id" apo_type)" "$(platform_field "$id" of_path)"
  done
fi

