#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Download use case datasets over plain HTTPS.

The use case data is published as one zip per dataset, fetched directly with no
account, login, or client library, so the use cases keep working regardless of the
state of the ShapeWorks Cloud portal.

Which archive each dataset name resolves to is pinned in datasets.json, next to
this file and checked into the source tree. Archives are versioned individually
(<name>-v<version>.zip) and are never overwritten on the server, so a given release
of ShapeWorks keeps downloading exactly the data it shipped against: republishing a
dataset for a newer release cannot change what an older one gets. Updating a dataset
means building a new version and pointing datasets.json at it.

Pinning the checksums here rather than reading them from the server is also what
makes verification worth doing, since a bad re-upload cannot rewrite the hash it is
checked against.

Set SW_DATA_URL to point at a different server or a local mirror when testing.
Archives are built by Support/build_dataset_archives.py.
"""
import hashlib
import json
import os
import ssl
import sys
import tempfile
import time
import urllib.error
import urllib.parse
import urllib.request
import zipfile

INDEX_FILE = os.path.join(os.path.dirname(os.path.abspath(__file__)), "datasets.json")

_CHUNK = 1024 * 256
_RETRIES = 3
_TIMEOUT = 60

_index_cache = None


def get_index():
    """The pinned dataset table shipped alongside this module."""
    global _index_cache
    if _index_cache is None:
        try:
            with open(INDEX_FILE) as fh:
                _index_cache = json.load(fh)
        except OSError as e:
            raise RuntimeError(f"Dataset index missing at {INDEX_FILE}: {e}")
        except json.JSONDecodeError as e:
            raise RuntimeError(f"Malformed dataset index at {INDEX_FILE}: {e}")
    return _index_cache


def base_url():
    """Server holding the dataset archives; override with $SW_DATA_URL."""
    url = os.environ.get("SW_DATA_URL") or get_index().get("base_url", "")
    return url if url.endswith("/") else url + "/"


def _open(url):
    """Open a URL, retrying transient failures."""
    last = None
    for attempt in range(_RETRIES):
        try:
            return urllib.request.urlopen(url, timeout=_TIMEOUT)
        except (urllib.error.URLError, ssl.SSLError, TimeoutError) as e:
            last = e
            if attempt < _RETRIES - 1:
                time.sleep(2 ** attempt)
    raise RuntimeError(f"Could not reach {url}\n  {last}\n"
                       f"Check your network connection, or set $SW_DATA_URL to a mirror.")


def list_datasets():
    """Names of every dataset this release knows how to download."""
    return sorted(get_index().get("datasets", {}))


def _progress(name, done, total):
    if not sys.stdout.isatty():
        return
    if total:
        pct = 100.0 * done / total
        sys.stdout.write(f"\r  {name}: {pct:5.1f}%  "
                         f"({done / 1e6:.1f} / {total / 1e6:.1f} MB)")
    else:
        sys.stdout.write(f"\r  {name}: {done / 1e6:.1f} MB")
    sys.stdout.flush()


def _download(url, destination, name, expected_size):
    """Stream a URL to disk, returning its sha256."""
    digest = hashlib.sha256()
    downloaded = 0
    with _open(url) as response:
        total = expected_size or int(response.headers.get("Content-Length") or 0)
        with open(destination, "wb") as out:
            while True:
                chunk = response.read(_CHUNK)
                if not chunk:
                    break
                out.write(chunk)
                digest.update(chunk)
                downloaded += len(chunk)
                _progress(name, downloaded, total)
    if sys.stdout.isatty():
        sys.stdout.write("\n")
    if expected_size and downloaded != expected_size:
        raise RuntimeError(
            f"{name}: download truncated, got {downloaded} bytes, expected {expected_size}")
    return digest.hexdigest()


def _read_marker(path):
    """Return what a previous download recorded, or None."""
    try:
        with open(path) as fh:
            return json.load(fh)
    except (OSError, json.JSONDecodeError):
        # Pre-existing marker files were empty, so treat them as unknown content.
        return None


def download_dataset(datasetName, outputDirectory, force=False):
    """Download a use case dataset and extract it into outputDirectory.

    The archive is verified against the sha256 in the manifest before it is
    extracted, so a partial or corrupted transfer fails loudly rather than leaving
    a half-populated directory behind for the use case to silently run on.

    The marker records the checksum that was installed, so pointing this release at
    a newer version of a dataset makes the next run replace it instead of quietly
    reusing whatever is already on disk.
    """
    marker = os.path.join("Output", datasetName + ".downloaded")
    installed = _read_marker(marker) if os.path.exists(marker) else None

    datasets = get_index().get("datasets", {})
    if datasetName not in datasets:
        raise RuntimeError(
            f"Unknown dataset '{datasetName}'\n"
            f"Available datasets:\n    " + "\n    ".join(sorted(datasets)))

    entry = datasets[datasetName]

    # Nothing here touches the network, so re-running a use case works offline.
    if installed and not force and installed.get("sha256") == entry.get("sha256"):
        print(f"Dataset {datasetName} already downloaded ({marker} exists)")
        if os.environ.get("SW_PORTAL_DOWNLOAD_ONLY") == "1":
            sys.exit(0)
        return
    if installed and installed.get("sha256") != entry.get("sha256"):
        print(f"Dataset {datasetName} is pinned to {entry['file']}, downloading it")

    url = urllib.parse.urljoin(base_url(), entry["file"])
    os.makedirs(outputDirectory, exist_ok=True)
    os.makedirs(os.path.dirname(marker) or ".", exist_ok=True)

    print(f"Downloading {datasetName} from {url}")
    handle, temporary = tempfile.mkstemp(suffix=".zip", prefix=datasetName + "-",
                                         dir=outputDirectory)
    os.close(handle)
    try:
        actual = _download(url, temporary, datasetName, entry.get("size"))
        expected = entry.get("sha256")
        if expected and actual != expected:
            raise RuntimeError(
                f"{datasetName}: checksum mismatch for {entry['file']}\n"
                f"  expected {expected}\n  got      {actual}\n"
                f"The copy on the server does not match what this release of "
                f"ShapeWorks was built against. Published archives are immutable, "
                f"so {entry['file']} was either overwritten or is corrupt.")

        print(f"Extracting {datasetName} to {outputDirectory}")
        with zipfile.ZipFile(temporary) as archive:
            members = [m for m in archive.namelist() if not m.endswith("/")]
            archive.extractall(outputDirectory)

        expected_files = entry.get("files")
        if expected_files is not None and len(members) != expected_files:
            raise RuntimeError(
                f"{datasetName}: archive holds {len(members)} files, "
                f"datasets.json expects {expected_files}. The archive needs rebuilding.")
    finally:
        if os.path.exists(temporary):
            os.remove(temporary)

    with open(marker, "w") as fh:
        json.dump({"dataset": datasetName, "sha256": entry.get("sha256"),
                   "files": len(members)}, fh)
        fh.write("\n")
    print(f"Dataset {datasetName} downloaded to {outputDirectory} "
          f"({len(members)} files)")

    if os.environ.get("SW_PORTAL_DOWNLOAD_ONLY") == "1":
        sys.exit(0)
