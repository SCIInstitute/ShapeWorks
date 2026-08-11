#!/usr/bin/env python3
"""Build the use case dataset archives served from sci.utah.edu.

Turns a local export of the use case data into one zip per dataset name, plus a
manifest.json describing them. The zips are laid out so that extracting
<dataset>.zip into Output/<use case>/ produces exactly the directory structure the
use case scripts glob for.

Usage:
    python3 Support/build_dataset_archives.py [--source DIR] [--dest DIR]
                                              [--only NAME ...] [--skip NAME ...]
                                              [--list] [--jobs N]

The resulting --dest directory is what gets uploaded to
https://www.sci.utah.edu/~shapeworks/data-sets/use-case-data-v4/
"""
import argparse
import concurrent.futures
import fnmatch
import hashlib
import json
import os
import sys
import zipfile

DEFAULT_SOURCE = os.path.expanduser("~/sci/datasets")
DEFAULT_DEST = os.path.expanduser("~/sci/datasets-web")

BASE_URL = "https://www.sci.utah.edu/~shapeworks/data-sets/use-case-data-v4/"

# Fixed timestamp so rebuilding identical content produces an identical zip.
FIXED_DATE = (1980, 1, 1, 0, 0, 0)

# dataset name (as requested by the use cases) -> how to build it
#   dir     : directory under --source holding the content
#   prefix  : directory level to insert at the root of the zip (the export is
#             missing a level the use case globs for)
#   include : keep only files matching one of these globs
#   subset  : (glob, n) keep only the first n files matching glob, plus every file
#             that does not match it at all
DATASETS = {
    # --- tiny tests -------------------------------------------------------
    "deep_ssm_femur_tiny_test": {"dir": "deep_ssm_femur_tiny_test"},
    "ellipsoid_tiny_test": {"dir": "ellipsoid_tiny_test"},
    "ellipsoid_cut_tiny_test": {"dir": "ellipsoid_cut_tiny_test"},
    "ellipsoid_mesh_tiny_test": {"dir": "ellipsoid_mesh_tiny_test"},
    "ellipsoid_multiple_domain_tiny_test": {"dir": "ellipsoid_multiple_domain_tiny_test"},
    "ellipsoid_multiple_domain_mesh_tiny_test": {"dir": "ellipsoid_multiple_domain_mesh_tiny_test"},
    "femur_cut_tiny_test": {"dir": "femur_cut_tiny_test"},
    "hip_multiple_domain_tiny_test": {"dir": "hip_multiple_domain_tiny_test"},
    "incremental_supershapes_tiny_test": {"dir": "incremental_supershapes_tiny_test"},
    "left_atrium_tiny_test": {"dir": "left_atrium_tiny_test"},
    "lumps_tiny_test": {"dir": "lumps_tiny_test"},
    "peanut_shared_boundary_tiny_test": {"dir": "peanut_shared_boundary_tiny_test"},
    # No tiny test was exported for thin_cavity_bean, so cut one from the full set.
    "thin_cavity_bean_tiny_test": {"dir": "thin_cavity_bean",
                                   "prefix": "thin_cavity_bean",
                                   "include": ["meshes/*.ply"],
                                   "subset": ("meshes/*.ply", 3)},

    # --- full datasets ----------------------------------------------------
    "deep_ssm_femur": {"dir": "deep_ssm_full"},
    "ellipsoid": {"dir": "ellipsoid_multiscale"},
    "ellipsoid_cut": {"dir": "ellipsoid_cut_multiscale"},
    "ellipsoid_fd": {"dir": "ellipsoid_fd"},
    "ellipsoid_fd_multiscale": {"dir": "ellipsoid_fd_multiscale"},
    "ellipsoid_mesh": {"dir": "ellipsoid_mesh_multiscale"},
    "ellipsoid_multiple_domain": {"dir": "ellipsoid_multiple_domain"},
    "ellipsoid_multiple_domain_mesh": {"dir": "ellipsoid_multiple_domain_mesh_singlescale"},
    "femur_cut": {"dir": "femur_cut_multiscale"},
    "hip_multiple_domain": {"dir": "hip_multiple_domain_singlescale"},
    "incremental_supershapes": {"dir": "incremental_supershapes"},
    "left_atrium": {"dir": "left_atrium_multiscale"},
    "lumps": {"dir": "lumps"},
    "peanut_shared_boundary": {"dir": "peanut_shared_boundary_multiscale"},
    # The export is missing the inner directory level the use case globs for.
    "thin_cavity_bean": {"dir": "thin_cavity_bean", "prefix": "thin_cavity_bean"},
}


def collect(root, spec):
    """Return [(absolute path, name inside the zip)] for one dataset."""
    entries = []
    for dirpath, dirnames, filenames in os.walk(root):
        dirnames.sort()
        for filename in sorted(filenames):
            if filename == ".DS_Store":
                continue
            full = os.path.join(dirpath, filename)
            rel = os.path.relpath(full, root)
            entries.append((full, rel))

    include = spec.get("include")
    if include:
        entries = [e for e in entries
                   if any(fnmatch.fnmatch(e[1], pat) for pat in include)]

    subset = spec.get("subset")
    if subset:
        pattern, count = subset
        matching = sorted(e for e in entries if fnmatch.fnmatch(e[1], pattern))
        keep = set(matching[:count])
        entries = [e for e in entries if e not in set(matching) or e in keep]

    prefix = spec.get("prefix")
    if prefix:
        entries = [(full, os.path.join(prefix, rel)) for full, rel in entries]

    return sorted(entries, key=lambda e: e[1])


def build_one(name, spec, source, dest):
    root = os.path.join(source, spec["dir"])
    if not os.path.isdir(root):
        return name, None, f"source directory missing: {root}"

    entries = collect(root, spec)
    if not entries:
        return name, None, f"no files found under {root}"

    out = os.path.join(dest, name + ".zip")
    tmp = out + ".partial"
    with zipfile.ZipFile(tmp, "w", zipfile.ZIP_DEFLATED, compresslevel=6) as zf:
        for full, arcname in entries:
            info = zipfile.ZipInfo(arcname, date_time=FIXED_DATE)
            info.compress_type = zipfile.ZIP_DEFLATED
            info.external_attr = 0o644 << 16
            with open(full, "rb") as fh:
                zf.writestr(info, fh.read())
    os.replace(tmp, out)

    digest = hashlib.sha256()
    with open(out, "rb") as fh:
        for chunk in iter(lambda: fh.read(1024 * 1024), b""):
            digest.update(chunk)

    return name, {
        "file": name + ".zip",
        "sha256": digest.hexdigest(),
        "size": os.path.getsize(out),
        "files": len(entries),
    }, None


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--source", default=DEFAULT_SOURCE, help="local dataset export")
    parser.add_argument("--dest", default=DEFAULT_DEST, help="staging directory to write")
    parser.add_argument("--only", nargs="+", metavar="NAME", help="build only these datasets")
    parser.add_argument("--skip", nargs="+", metavar="NAME", default=[], help="skip these datasets")
    parser.add_argument("--jobs", type=int, default=4, help="parallel zip workers")
    parser.add_argument("--list", action="store_true", help="list dataset names and exit")
    args = parser.parse_args()

    if args.list:
        for name in sorted(DATASETS):
            print(name)
        return 0

    names = sorted(args.only if args.only else DATASETS)
    unknown = [n for n in names if n not in DATASETS]
    if unknown:
        parser.error("unknown dataset(s): " + ", ".join(unknown))
    names = [n for n in names if n not in args.skip]

    os.makedirs(args.dest, exist_ok=True)

    # Merge into any manifest already staged so partial rebuilds are additive.
    manifest_path = os.path.join(args.dest, "manifest.json")
    datasets = {}
    if os.path.exists(manifest_path):
        with open(manifest_path) as fh:
            datasets = json.load(fh).get("datasets", {})

    failures = []
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:
        futures = {pool.submit(build_one, n, DATASETS[n], args.source, args.dest): n
                   for n in names}
        for future in concurrent.futures.as_completed(futures):
            name, entry, error = future.result()
            if error:
                failures.append((name, error))
                print(f"  FAIL  {name}: {error}", file=sys.stderr)
                continue
            datasets[name] = entry
            print(f"  ok    {name:44} {entry['files']:>5} files  "
                  f"{entry['size'] / 1e6:>9.1f} MB")

    with open(manifest_path, "w") as fh:
        json.dump({"version": 1, "base_url": BASE_URL,
                   "datasets": dict(sorted(datasets.items()))}, fh, indent=2)
        fh.write("\n")

    total = sum(d["size"] for d in datasets.values())
    print(f"\n{len(datasets)} datasets in manifest, {total / 1e9:.2f} GB total")
    print(f"staged in {args.dest}")
    if failures:
        print(f"\n{len(failures)} failed:", file=sys.stderr)
        for name, error in failures:
            print(f"  {name}: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
