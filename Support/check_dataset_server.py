#!/usr/bin/env python3
"""Check the data server actually serves what datasets.json pins.

Issues one HEAD request per dataset and compares the size the server reports
against the pinned size, so a missing, truncated, or half-uploaded archive is
caught right after uploading rather than by a user running a use case.

Pass --full to download each archive and verify its SHA-256 instead. That is the
real check, but it transfers everything, so it is off by default.

Usage:
    python3 Support/check_dataset_server.py [--index FILE] [--url BASE] [--full]
"""
import argparse
import hashlib
import json
import os
import sys
import urllib.error
import urllib.parse
import urllib.request

TIMEOUT = 60


def head(url):
    """Return (status, content-length) for a URL."""
    request = urllib.request.Request(url, method="HEAD")
    try:
        with urllib.request.urlopen(request, timeout=TIMEOUT) as response:
            return response.status, int(response.headers.get("Content-Length") or 0)
    except urllib.error.HTTPError as e:
        return e.code, 0
    except (urllib.error.URLError, TimeoutError) as e:
        print(f"    {e}", file=sys.stderr)
        return 0, 0


def sha256_of(url):
    digest = hashlib.sha256()
    with urllib.request.urlopen(url, timeout=TIMEOUT) as response:
        for chunk in iter(lambda: response.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def main():
    here = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--index", default=os.path.join(
        here, "Python", "shapeworks", "shapeworks", "datasets.json"),
        help="pinned dataset table to check the server against")
    parser.add_argument("--url", help="base URL to check (default: the pinned base_url)")
    parser.add_argument("--full", action="store_true",
                        help="download each archive and verify its checksum")
    args = parser.parse_args()

    with open(args.index) as fh:
        index = json.load(fh)
    base = args.url or index.get("base_url", "")
    if not base.endswith("/"):
        base += "/"
    datasets = index.get("datasets", {})

    print(f"checking {len(datasets)} datasets against {base}\n")
    bad = 0
    for name, entry in sorted(datasets.items()):
        url = urllib.parse.urljoin(base, entry["file"])
        status, length = head(url)
        if status != 200:
            print(f"  MISSING   {entry['file']:44} HTTP {status}")
            bad += 1
            continue
        if length != entry["size"]:
            print(f"  SIZE      {entry['file']:44} "
                  f"server {length} != pinned {entry['size']}")
            bad += 1
            continue
        if args.full:
            actual = sha256_of(url)
            if actual != entry["sha256"]:
                print(f"  CHECKSUM  {entry['file']:44} {actual[:16]}... "
                      f"!= {entry['sha256'][:16]}...")
                bad += 1
                continue
            print(f"  ok        {entry['file']:44} checksum verified")
        else:
            print(f"  ok        {entry['file']:44} {length / 1e6:>9.1f} MB")

    print()
    if bad:
        print(f"{bad} of {len(datasets)} datasets are wrong on the server", file=sys.stderr)
        return 1
    print(f"all {len(datasets)} datasets present and the right size"
          + ("" if args.full else "; re-run with --full to verify checksums"))
    return 0


if __name__ == "__main__":
    sys.exit(main())
