"""Provision dep-built (shared) ITK into the bundled Python's site-packages.

Copies the ITK Python tree from the dependency install's site-packages into the
bundled Python's site-packages and writes a minimal `.dist-info` for *each* of
the pip wheels we're replacing, so pip's resolver treats them as already
satisfied. Without this, transitive ITK dependents (itk-elastix in our
requirements list; itkwidgets when present) pull the full pip itk-* family
(~1.5 GB) back into the bundle alongside our shared ITK.

ITK's pip distribution is split across many wheels that all install into the
same `itk/` package directory — that's the main shape difference vs VTK
(`provision_bundled_vtk.py`), which had one wheel name and one top-level
package. We write one dist-info per replaced wheel; pip then satisfies each
metadata requirement individually.

itk-elastix is intentionally NOT stubbed — pip will still install its wheel,
and its files overlay onto the same `itk/` dir without conflict (the elastix
wheel only adds new files; it does not overwrite anything dep ITK ships).
itk-meshtopolydata is dropped from the bundled requirements entirely (unused
in our code), so it needs no stub either.

Run as the first step of `bundled_pip_install`, before pip installs the
requirements (which include itk-elastix).

Usage: provision_bundled_itk.py <dep_site_packages> <bundled_site_packages> <itk_version>
"""

import os
import shutil
import sys

dep_sp, bundled_sp, version = sys.argv[1], sys.argv[2], sys.argv[3]

os.makedirs(bundled_sp, exist_ok=True)

# Both the `itk` package dir and the top-level `itkConfig.py` ship in the dep
# install's site-packages. itk/__init__.py imports `from itkConfig import ...`
# at module load, so itkConfig.py must be present alongside the package.
for name in ("itk", "itkConfig.py"):
    src = os.path.join(dep_sp, name)
    dst = os.path.join(bundled_sp, name)
    if not os.path.exists(src):
        continue
    if os.path.isdir(src):
        shutil.rmtree(dst, ignore_errors=True)
        shutil.copytree(src, dst, symlinks=True)
    else:
        shutil.copy2(src, dst)

# Stub a dist-info for every pip itk-* wheel name our dep build covers. The
# version string matches what python_requirements.txt currently pins so pip's
# resolver accepts dependents that require `itk~=5.4.0` etc.
for wheel_name in (
    "itk",
    "itk_core",
    "itk_numerics",
    "itk_io",
    "itk_filtering",
    "itk_registration",
    "itk_segmentation",
):
    # PEP 427: dist-info directory name uses underscore-normalized package
    # name + version. Pip's resolver matches the normalized name against
    # Requires-Dist entries, so e.g. `itk-core` (Requires-Dist) matches the
    # `itk_core-<ver>.dist-info` directory we write here.
    distinfo = os.path.join(bundled_sp, f"{wheel_name}-{version}.dist-info")
    os.makedirs(distinfo, exist_ok=True)
    display_name = wheel_name.replace("_", "-")
    with open(os.path.join(distinfo, "METADATA"), "w") as f:
        f.write(
            "Metadata-Version: 2.1\n"
            f"Name: {display_name}\n"
            f"Version: {version}\n"
            "Summary: ITK built from source by ShapeWorks build_dependencies.sh\n"
        )
    with open(os.path.join(distinfo, "INSTALLER"), "w") as f:
        f.write("shapeworks-build_dependencies\n")

print(f"Provisioned dep-built ITK {version} into {bundled_sp}", file=sys.stderr)
