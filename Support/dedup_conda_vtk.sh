#!/usr/bin/env bash
#
# Deduplicate VTK in the active conda environment.
#
# Why: ShapeWorks' C++ links the shared, Python-wrapped VTK built by
# build_dependencies.sh. If a *different* VTK -- the pip `vtk` wheel that
# pyvista drags into the conda env via python_requirements.txt -- is imported
# into one of those VTK-linked processes (the in-process Python interpreter
# embedded by Optimize/Studio, which does `import vtk`), two complete VTK
# builds end up loaded in a single process. On Linux's ELF flat-namespace
# loader this is fatal -- the heap corrupts ("malloc(): invalid size" /
# HDF5 I/O errors). macOS Mach-O's two-level namespace and Windows' per-DLL
# import-table resolution both insulate against this; only Linux requires the
# fix in practice. Still safe to run on macOS.
#
# Approach: pyvista hard-depends on vtk and python_requirements.txt is a
# pinned pip-compile lock, so we can't keep pip from installing the wheel.
# Instead remove it after the fact and point the env's Python at our dep
# build via a .pth file. The dep vtkmodules' $ORIGIN-relative RPATH resolves
# libvtk* under the dep lib dir, so no copying or RPATH patching is needed.
#
# Why this lives outside build_dependencies.sh: the conda env is rebuilt
# every CI run by `pip install -r python_requirements.txt`, which puts pip
# vtk back. build_dependencies.sh is cached in GHA (cache key = sha1sum of
# the script) so on cache hits its side effects on the conda env never
# re-apply. Running this as a separate step after pip install closes that
# hole.
#
# Usage:
#   INSTALL_DIR=/path/to/deps/install bash Support/dedup_conda_vtk.sh
#   bash Support/dedup_conda_vtk.sh /path/to/deps/install   # positional form
#
# Idempotent: re-running is a no-op.

set -eu

INSTALL_DIR="${1:-${INSTALL_DIR:-}}"

if [[ "${OSTYPE}" == msys* || "${OSTYPE}" == cygwin* ]]; then
  echo "Windows: skipping conda VTK dedup (PE per-DLL filename resolution avoids the collision)."
  exit 0
fi

if [[ -z "${CONDA_PREFIX:-}" ]]; then
  echo "No active conda env (CONDA_PREFIX unset); skipping conda VTK dedup."
  exit 0
fi

if [[ -z "${INSTALL_DIR}" ]]; then
  echo "ERROR: INSTALL_DIR not provided (pass as argv1 or env var)."
  exit 1
fi

dep_sp=""
for cand in "${INSTALL_DIR}"/lib/python3.*/site-packages; do
  if [[ -d "${cand}/vtkmodules" ]]; then dep_sp="${cand}"; break; fi
done
if [[ -z "${dep_sp}" ]]; then
  echo "WARNING: no dep-built vtkmodules under ${INSTALL_DIR}; skipping conda VTK dedup."
  exit 0
fi

conda_sp=$(python -c "import sysconfig; print(sysconfig.get_paths()['purelib'])")

echo ""
echo "## Deduplicating VTK in conda env"
echo "   removing pip vtk (if present) and pointing ${conda_sp} at ${dep_sp}"
python -m pip uninstall -y vtk >/dev/null 2>&1 || true
echo "${dep_sp}" > "${conda_sp}/_shapeworks_dep_vtk.pth"
