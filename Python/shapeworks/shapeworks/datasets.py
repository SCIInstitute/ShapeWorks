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

Downloads resume, within a run and across runs, from the bytes already on disk.

Set SW_DATA_URL to point at a different server or a local mirror when testing.
Archives are built by Support/build_dataset_archives.py.
"""
import hashlib
import http.client
import json
import os
import socket
import ssl
import sys
import time
import urllib.error
import urllib.parse
import urllib.request
import zipfile

INDEX_FILE = os.path.join(os.path.dirname(os.path.abspath(__file__)), "datasets.json")

_CHUNK = 1024 * 256
_TIMEOUT = 60           # seconds one read may stall
_STALLED_RETRIES = 6    # consecutive attempts moving no bytes before giving up
_MAX_ATTEMPTS = 40
_MAX_BACKOFF = 30
_REPORT_EVERY = 30      # seconds between progress lines in a log

# socket.timeout only aliases TimeoutError from Python 3.10 on.
_TRANSIENT = (urllib.error.URLError, http.client.HTTPException, ssl.SSLError,
              socket.timeout, TimeoutError, ConnectionError, EOFError)
_RETRY_STATUS = (408, 425, 429, 500, 502, 503, 504)

_index_cache = None
_reported = [0.0]


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


def list_datasets():
    """Names of every dataset this release knows how to download."""
    return sorted(get_index().get("datasets", {}))


def _progress(name, done, total):
    """Report progress: continuously on a terminal, every _REPORT_EVERY seconds in a log."""
    now = time.monotonic()
    if sys.stdout.isatty():
        line_end = "\r"
    elif now - _reported[0] >= _REPORT_EVERY:
        line_end = "\n"
    else:
        return
    _reported[0] = now
    if total:
        sys.stdout.write(f"  {name}: {100.0 * done / total:5.1f}%  "
                         f"({done / 1e6:.1f} / {total / 1e6:.1f} MB){line_end}")
    else:
        sys.stdout.write(f"  {name}: {done / 1e6:.1f} MB{line_end}")
    sys.stdout.flush()


def _size(path):
    return os.path.getsize(path) if os.path.exists(path) else 0


def _sha256(path):
    digest = hashlib.sha256()
    with open(path, "rb") as fh:
        for block in iter(lambda: fh.read(_CHUNK), b""):
            digest.update(block)
    return digest.hexdigest()


def _transient(error):
    """Whether another attempt could get past this failure."""
    if isinstance(error, urllib.error.HTTPError):
        return error.code in _RETRY_STATUS
    return isinstance(error, _TRANSIENT)


def _stream(url, destination, name, total, tag):
    """One attempt at fetching url, appending to whatever destination already holds.

    total and tag (length and ETag) carry over between attempts. Returns the bytes on
    disk, short of total if the connection dropped.
    """
    have = _size(destination)
    request = urllib.request.Request(url)
    if have:
        request.add_header("Range", f"bytes={have}-")
        if tag:
            # A changed archive answers 200, not 206, so we start over rather than
            # splice two different downloads together.
            request.add_header("If-Range", tag)

    with urllib.request.urlopen(request, timeout=_TIMEOUT) as response:
        tag = response.headers.get("ETag") or tag
        length = int(response.headers.get("Content-Length") or 0)
        if response.getcode() != 206:
            have = 0  # not partial content: start over
        total = total or (have + length)
        with open(destination, "ab" if have else "wb") as out:
            while True:
                chunk = response.read(_CHUNK)
                if not chunk:
                    break
                out.write(chunk)
                have += len(chunk)
                _progress(name, have, total)
    return have, total, tag


def _download(url, destination, name, expected_size):
    """Fetch a URL to disk and return its sha256.

    Each attempt resumes from the bytes already on disk, and the partial file is left
    behind when we give up.
    """
    total = expected_size or 0
    tag = None
    have = _size(destination)
    stalled = 0
    problem = None

    for attempt in range(_MAX_ATTEMPTS):
        before = have
        try:
            have, total, tag = _stream(url, destination, name, total, tag)
            if have >= total:
                break
            problem = f"connection closed after {have} of {total} bytes"
        except _TRANSIENT as e:
            if isinstance(e, urllib.error.HTTPError) and e.code == 416:
                # A partial longer than the archive cannot belong to it.
                os.remove(destination)
                total = expected_size or 0
                tag = None
            elif not _transient(e):
                raise RuntimeError(f"Could not download {url}\n  {e}\n"
                                   f"Set $SW_DATA_URL to a mirror if the server has "
                                   f"moved.") from None
            have = _size(destination)
            problem = str(e) or e.__class__.__name__

        stalled = 0 if have > before else stalled + 1
        if stalled >= _STALLED_RETRIES:
            problem = f"{problem} (nothing transferred in {stalled} attempts)"
            break
        if attempt + 1 >= _MAX_ATTEMPTS:
            problem = f"{problem} (gave up after {_MAX_ATTEMPTS} attempts)"
            break
        delay = min(_MAX_BACKOFF, 2 ** stalled)
        if sys.stdout.isatty():
            sys.stdout.write("\n")
        print(f"  {name}: {problem}; resuming in {delay}s "
              f"(attempt {attempt + 2} of {_MAX_ATTEMPTS})", flush=True)
        time.sleep(delay)

    if sys.stdout.isatty():
        sys.stdout.write("\n")
    if have < total or (expected_size and have != expected_size):
        raise RuntimeError(
            f"Could not download {name} from {url}\n  {problem}\n"
            f"{_size(destination) / 1e6:.1f} MB were kept in {destination}, so running "
            f"this again resumes rather than starting over.\n"
            f"Check your network connection, or set $SW_DATA_URL to a mirror.")
    return _sha256(destination)


def _read_marker(path):
    """What a previous download recorded, or {} for the empty markers older releases and
    the CI tiny test cache wrote."""
    try:
        with open(path) as fh:
            return json.load(fh)
    except json.JSONDecodeError:
        return {}
    except OSError:
        return None


def download_dataset(datasetName, outputDirectory, force=False):
    """Download a use case dataset and extract it into outputDirectory.

    The archive is verified against the sha256 in the manifest before it is
    extracted, so a partial or corrupted transfer fails loudly rather than leaving
    a half-populated directory behind for the use case to silently run on.

    The marker records the checksum that was installed, so pointing this release at
    a newer version of a dataset makes the next run replace it instead of quietly
    reusing whatever is already on disk.

    A download that gives up leaves its partial archive behind for the next run.
    """
    marker = os.path.join("Output", datasetName + ".downloaded")
    installed = _read_marker(marker) if os.path.exists(marker) else None

    datasets = get_index().get("datasets", {})
    if datasetName not in datasets:
        raise RuntimeError(
            f"Unknown dataset '{datasetName}'\n"
            f"Available datasets:\n    " + "\n    ".join(sorted(datasets)))

    entry = datasets[datasetName]
    recorded = installed.get("sha256") if installed is not None else None

    # Nothing here touches the network, so re-running a use case works offline. A marker
    # naming no checksum is taken at its word: CI pre-populates the tiny test data.
    if installed is not None and not force and recorded in (None, entry.get("sha256")):
        print(f"Dataset {datasetName} already downloaded ({marker} exists)")
        if os.environ.get("SW_PORTAL_DOWNLOAD_ONLY") == "1":
            sys.exit(0)
        return
    if recorded and recorded != entry.get("sha256"):
        print(f"Dataset {datasetName} is pinned to {entry['file']}, downloading it")

    url = urllib.parse.urljoin(base_url(), entry["file"])
    os.makedirs(outputDirectory, exist_ok=True)
    os.makedirs(os.path.dirname(marker) or ".", exist_ok=True)

    print(f"Downloading {datasetName} from {url}")
    partial = os.path.join(outputDirectory, entry["file"] + ".part")
    if os.path.exists(partial):
        print(f"Resuming from {_size(partial) / 1e6:.1f} MB already in {partial}")

    # Kept on a failed download so the next run resumes, removed once judged.
    actual = _download(url, partial, datasetName, entry.get("size"))
    try:
        expected = entry.get("sha256")
        if expected and actual != expected:
            raise RuntimeError(
                f"{datasetName}: checksum mismatch for {entry['file']}\n"
                f"  expected {expected}\n  got      {actual}\n"
                f"The bytes we ended up with do not match what this release of "
                f"ShapeWorks was built against. Published archives are immutable, so "
                f"{entry['file']} was either overwritten on the server or arrived "
                f"corrupt. It has been discarded, so running this again starts fresh.")

        print(f"Extracting {datasetName} to {outputDirectory}")
        with zipfile.ZipFile(partial) as archive:
            members = [m for m in archive.namelist() if not m.endswith("/")]
            archive.extractall(outputDirectory)

        expected_files = entry.get("files")
        if expected_files is not None and len(members) != expected_files:
            raise RuntimeError(
                f"{datasetName}: archive holds {len(members)} files, "
                f"datasets.json expects {expected_files}. The archive needs rebuilding.")
    finally:
        if os.path.exists(partial):
            os.remove(partial)

    with open(marker, "w") as fh:
        json.dump({"dataset": datasetName, "sha256": entry.get("sha256"),
                   "files": len(members)}, fh)
        fh.write("\n")
    print(f"Dataset {datasetName} downloaded to {outputDirectory} "
          f"({len(members)} files)")

    if os.environ.get("SW_PORTAL_DOWNLOAD_ONLY") == "1":
        sys.exit(0)
