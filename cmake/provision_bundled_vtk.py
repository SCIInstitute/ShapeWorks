"""Provision dep-built (shared) VTK into the bundled Python's site-packages.

Copies `vtkmodules` + `vtk.py` from the dependency install's site-packages into
the bundled Python's site-packages and writes a minimal `vtk-<ver>.dist-info` so
pip's resolver treats vtk as already satisfied. Without this, transitive vtk
dependents (pyvista) pull the ~379 MB pip vtk wheel and reintroduce a duplicate
VTK alongside the shared one we build and bundle.

Run as the first step of `bundled_pip_install`, before pip installs the
requirements (which include pyvista).

Usage: provision_bundled_vtk.py <dep_site_packages> <bundled_site_packages> <vtk_version>
"""

import os
import shutil
import sys

dep_sp, bundled_sp, version = sys.argv[1], sys.argv[2], sys.argv[3]

os.makedirs(bundled_sp, exist_ok=True)

for name in ("vtkmodules", "vtk.py"):
    src = os.path.join(dep_sp, name)
    dst = os.path.join(bundled_sp, name)
    if not os.path.exists(src):
        continue
    if os.path.isdir(src):
        shutil.rmtree(dst, ignore_errors=True)
        shutil.copytree(src, dst, symlinks=True)
    else:
        shutil.copy2(src, dst)

distinfo = os.path.join(bundled_sp, f"vtk-{version}.dist-info")
os.makedirs(distinfo, exist_ok=True)
with open(os.path.join(distinfo, "METADATA"), "w") as f:
    f.write(
        "Metadata-Version: 2.1\n"
        "Name: vtk\n"
        f"Version: {version}\n"
        "Summary: VTK built from source by ShapeWorks build_dependencies.sh\n"
    )
with open(os.path.join(distinfo, "INSTALLER"), "w") as f:
    f.write("shapeworks-build_dependencies\n")

print(f"Provisioned dep-built VTK {version} into {bundled_sp}", file=sys.stderr)
