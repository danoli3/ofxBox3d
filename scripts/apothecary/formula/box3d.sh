#!/usr/bin/env bash

FORMULA_TYPES=("osx" "vs" "linux")
FORMULA_DEPENDS=()
VER=0.1.0
BUILD_ID=1
GIT_URL=https://github.com/erincatto/box3d
GIT_TAG=c52908c9a907714e4d3a8a30be5272a1761158e1
URL=${GIT_URL}/archive/${GIT_TAG}

function download() {
  . "$DOWNLOADER_SCRIPT"
  if [[ "$TYPE" == vs ]]; then
    downloader "${URL}.zip"
    unzip -q "${GIT_TAG}.zip"
    mv "box3d-${GIT_TAG}" box3d
    rm "${GIT_TAG}.zip"
  else
    downloader "${URL}.tar.gz"
    tar -xf "${GIT_TAG}.tar.gz"
    mv "box3d-${GIT_TAG}" box3d
    rm "${GIT_TAG}.tar.gz"
  fi
}

function prepare() { :; }

function build() {
  local dir="build_${TYPE}_${PLATFORM}"
  mkdir -p "$dir"
  local args=(-S . -B "$dir" -DBUILD_SHARED_LIBS=OFF -DBOX3D_UNIT_TESTS=OFF -DBOX3D_SAMPLES=OFF -DBOX3D_BENCHMARKS=OFF -DBOX3D_DOCS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_POSITION_INDEPENDENT_CODE=ON)
  if [[ "$TYPE" == vs ]]; then
    args+=(-A "$PLATFORM" -G "Visual Studio ${VS_VER_GEN}")
  elif [[ "$TYPE" == linux && "${CROSSCOMPILING:-0}" == 1 ]]; then
    args+=(-DCMAKE_TOOLCHAIN_FILE="$APOTHECARY_DIR/toolchains/${TYPE}${PLATFORM}.toolchain.cmake")
  fi
  cmake "${args[@]}"
  cmake --build "$dir" --config Release --parallel "${PARALLEL_MAKE:-2}" --target box3d
}

function copy() {
  local dest="$1" dir="build_${TYPE}_${PLATFORM}" lib
  mkdir -p "$dest/include" "$dest/license" "$dest/lib/$TYPE/$PLATFORM"
  cp -R include/box3d "$dest/include/"
  cp LICENSE "$dest/license/"
  if [[ "$TYPE" == vs ]]; then lib="$(find "$dir" -name box3d.lib | head -1)"; else lib="$(find "$dir" -name libbox3d.a | head -1)"; fi
  [[ -n "$lib" ]] || return 1
  cp "$lib" "$dest/lib/$TYPE/$PLATFORM/"
}

function clean() { rm -rf build_*; }

