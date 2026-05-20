#!/usr/bin/env bash
# Smoke test for the packaged ShapeWorks bundle.
#
# Runs a few sanity checks against the post-package tree (the directory that
# contains bin/, lib/, etc. ready to ship) with conda stripped from the env,
# so we're truly testing the bundled Python. Each invocation here mirrors
# what a real end user would do on a clean machine.
#
# Catches regressions like:
#   - CLI silently falling back to conda's Python instead of bundled
#   - shapeworks_py failing to load due to missing/old libstdc++ on Linux,
#     missing DLLs on Windows, or RPATH mistakes on macOS
#   - swpython.bat / swpython missing from the package
#   - PATH/PYTHONHOME wrappers broken
#
# Usage: smoke_test_bundle.sh <packaged-tree>
#   where <packaged-tree> is the directory containing bin/ (e.g. package/$VERSION
#   on Linux/macOS, or the build's staging bin/ parent on Windows).
set -euo pipefail

PKG="${1:?usage: smoke_test_bundle.sh <packaged-tree>}"

case "$(uname -s)" in
  MINGW*|CYGWIN*|MSYS*) BAT=".bat"; EXE=".exe" ;;
  *)                    BAT="";     EXE=""    ;;
esac

# Strip conda from env so we're truly testing the bundled Python.
unset CONDA_PREFIX CONDA_DEFAULT_ENV CONDA_PYTHON_EXE CONDA_SHLVL CONDA_EXE 2>/dev/null || true
unset PYTHONHOME PYTHONPATH 2>/dev/null || true
PATH=$(echo "$PATH" | tr ':' '\n' | grep -v -i -E '(conda|miniconda|anaconda)' | tr '\n' ':' | sed 's/:$//')
export PATH

echo "=== Smoke test: $PKG ==="

echo "=== shapeworks --version ==="
"$PKG/bin/shapeworks$EXE" --version

echo "=== swpython: import shapeworks core modules ==="
"$PKG/bin/swpython$BAT" -c "
import shapeworks
import shapeworks_py
print('shapeworks pkg :', shapeworks.__file__)
print('shapeworks_py  :', shapeworks_py.__file__)
print('OK')
"

echo "=== swpip --help (verifies wrapper + module path) ==="
"$PKG/bin/swpip$BAT" --help > /dev/null

echo "=== ALL SMOKE TESTS PASSED ==="
