#!/bin/bash
#
# Workaround for ITK's exported CMake targets hardcoding absolute paths to macOS SDK ".tbd" stub
# libraries (e.g. libiconv.tbd, libm.tbd) in their INTERFACE_LINK_LIBRARIES. Our dependency install
# is cached in CI keyed only on the dependency-script hashes, so when the GitHub runner's Xcode/SDK
# is updated those absolute paths no longer exist and linking fails with, for example:
#
#   make: *** No rule to make target
#     '/Applications/Xcode_16.4.app/.../SDKs/MacOSX15.5.sdk/usr/lib/libiconv.tbd', needed by '...'.
#
# These are standard system libraries, so rewrite the SDK-versioned absolute paths to plain
# -l<name> link flags. The linker then resolves them regardless of the installed SDK version.
#
# See: https://github.com/SCIInstitute/ShapeWorks/issues/2570

set -uo pipefail

install_dir="${1:-$HOME/install}"

if [[ ! -d "$install_dir" ]]; then
  echo "install dir '$install_dir' not found; nothing to patch"
  exit 0
fi

# find dependency files that reference an SDK .tbd system library by absolute path
files=$(grep -rlE 'MacOSX[0-9.]*\.sdk/usr/lib/lib[A-Za-z0-9_+-]*\.tbd' "$install_dir" 2>/dev/null || true)

if [[ -z "$files" ]]; then
  echo "no hardcoded SDK .tbd library paths found under $install_dir"
  exit 0
fi

for f in $files; do
  sed -i.bak -E 's#/[^";[:space:]]*/MacOSX[0-9.]*\.sdk/usr/lib/lib([A-Za-z0-9_+-]*)\.tbd#-l\1#g' "$f"
  rm -f "$f.bak"
  echo "patched hardcoded SDK library paths in: $f"
done
