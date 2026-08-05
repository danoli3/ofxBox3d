#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
source "$SCRIPT_DIR/platforms.sh"

id="${1:-host}"
[[ "$id" == host ]] && id="$(detect_host)"
of_path="$(platform_field "$id" of_path)"
library="$(platform_field "$id" library)"
src="$ROOT/libs/box3d-prebuilt/lib/$of_path/$library"
[[ -f "$src" ]] || { echo "Missing $src; run build_libs.sh first" >&2; exit 1; }

stage="$(mktemp -d)"
trap 'rm -rf "$stage"' EXIT
mkdir -p "$stage/box3d/include" "$stage/box3d/lib/$of_path" "$stage/box3d/license"
cp -R "$ROOT/libs/box3d/include/box3d" "$stage/box3d/include/"
cp "$src" "$stage/box3d/lib/$of_path/$library"
cp "$ROOT/libs/box3d/LICENSE" "$stage/box3d/license/BOX3D-LICENSE"
printf 'version=%s\npackage_id=%s\nsource_commit=%s\n' "$BOX3D_VERSION" "$id" "$(git -C "$ROOT/libs/box3d" rev-parse HEAD)" > "$stage/box3d/PACKAGE.txt"
mkdir -p "$SCRIPT_DIR/dist"
tar -czf "$SCRIPT_DIR/dist/$(asset_name "$id")" -C "$stage" box3d
echo "Created scripts/dist/$(asset_name "$id")"

