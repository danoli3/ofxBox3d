#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
source "$SCRIPT_DIR/platforms.sh"

id="${1:-host}"
[[ "$id" == host ]] && id="$(detect_host)"
type="$(platform_field "$id" apo_type)" || { echo "Unknown platform: $id" >&2; exit 2; }
arch="$(platform_field "$id" arch)"
of_path="$(platform_field "$id" of_path)"
library="$(platform_field "$id" library)"

apo_root="${APOTHECARY_DIR:-$ROOT/scripts/.cache/apothecary}"
if [[ ! -d "$apo_root/apothecary" ]]; then
  mkdir -p "$(dirname "$apo_root")"
  git clone --depth 1 --branch "${APOTHECARY_REF:-bleeding}" https://github.com/openframeworks/apothecary.git "$apo_root"
fi

formula_dir="$apo_root/apothecary/formulas/box3d"
mkdir -p "$formula_dir"
cp "$SCRIPT_DIR/apothecary/formula/box3d.sh" "$formula_dir/box3d.sh"

apo="$apo_root/apothecary/apothecary"
out="$ROOT/scripts/.cache/output-$id"
rm -rf "$out"
mkdir -p "$out"
bash "$apo" -t "$type" -a "$arch" -d "$out" update box3d

hit="$(find "$out" -type f -name "$library" | head -1)"
[[ -n "$hit" ]] || { echo "Could not find $library in Apothecary output" >&2; exit 1; }
dest="$ROOT/libs/box3d-prebuilt/lib/$of_path"
mkdir -p "$dest"
cp "$hit" "$dest/$library"
echo "Built $dest/$library"

