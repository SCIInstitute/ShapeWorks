#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Download use case datasets over plain HTTPS.

The use case data is published as one zip per dataset alongside a manifest.json
describing each one. This module fetches those archives directly, with no account,
login, or client library needed, so the use cases keep working regardless of the
state of the ShapeWorks Cloud portal.

Set SW_DATA_URL to point at a different server or a local directory listing when
testing. Archives are built by Support/build_dataset_archives.py.
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

DEFAULT_BASE_URL = "https://www.sci.utah.edu/~shapeworks/data-sets/use-case-data-v4/"

_CHUNK = 1024 * 256
_RETRIES = 3
_TIMEOUT = 60

_manifest_cache = None


def base_url():
    """Server holding the dataset archives; override with $SW_DATA_URL."""
    url = os.environ.get("SW_DATA_URL", DEFAULT_BASE_URL)
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


def get_manifest(refresh=False):
    """Fetch and cache the dataset manifest."""
    global _manifest_cache
    if _manifest_cache is not None and not refresh:
        return _manifest_cache
    url = base_url() + "manifest.json"
    with _open(url) as response:
        try:
            _manifest_cache = json.loads(response.read().decode("utf-8"))
        except json.JSONDecodeError as e:
            raise RuntimeError(f"Malformed dataset manifest at {url}: {e}")
    return _manifest_cache


def list_datasets():
    """Names of every dataset available on the server."""
    return sorted(get_manifest().get("datasets", {}))


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

    The marker records the checksum that was installed, so republishing a dataset
    on the server makes the next run re-download it automatically instead of
    quietly reusing whatever is already on disk.
    """
    marker = os.path.join("Output", datasetName + ".downloaded")
    installed = _read_marker(marker) if os.path.exists(marker) else None

    try:
        manifest = get_manifest()
    except RuntimeError:
        # Offline, but the data is already here: let the use case run on it.
        if installed:
            print(f"Warning: cannot reach {base_url()}, using the copy of "
                  f"{datasetName} already in {outputDirectory}")
            return
        raise

    datasets = manifest.get("datasets", {})
    if datasetName not in datasets:
        raise RuntimeError(
            f"Unknown dataset '{datasetName}' on {base_url()}\n"
            f"Available datasets:\n    " + "\n    ".join(sorted(datasets)))

    entry = datasets[datasetName]

    if installed and not force and installed.get("sha256") == entry.get("sha256"):
        print(f"Dataset {datasetName} already downloaded ({marker} exists)")
        if os.environ.get("SW_PORTAL_DOWNLOAD_ONLY") == "1":
            sys.exit(0)
        return
    if installed and installed.get("sha256") != entry.get("sha256"):
        print(f"Dataset {datasetName} changed on the server, downloading again")

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
                f"{datasetName}: checksum mismatch\n"
                f"  expected {expected}\n  got      {actual}\n"
                f"The copy on the server may be corrupt or mid-update. "
                f"Re-run to retry; if it persists the archive needs rebuilding.")

        print(f"Extracting {datasetName} to {outputDirectory}")
        with zipfile.ZipFile(temporary) as archive:
            members = [m for m in archive.namelist() if not m.endswith("/")]
            archive.extractall(outputDirectory)

        expected_files = entry.get("files")
        if expected_files is not None and len(members) != expected_files:
            raise RuntimeError(
                f"{datasetName}: archive holds {len(members)} files, "
                f"manifest says {expected_files}. The archive needs rebuilding.")
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
