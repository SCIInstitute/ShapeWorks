#!/bin/bash
# Build ShapeWorks-<tag>-src-with-git-lfs.zip for a release.  GitHub's automatic
# source archives only hold LFS pointer files, so this one is uploaded by hand.
#
# Usage: Support/make_source_zip.sh <tag> [output_dir]
#
# The zip holds every file tracked at the tag with the real LFS content, and no
# .git folder.  LFS objects come from this checkout's cache, and any that are
# missing are downloaded from GitHub.

set -euo pipefail

if [ "$#" -lt 1 ] || [ "$#" -gt 2 ]; then
    echo "Usage: $0 <tag> [output_dir]"
    exit 1
fi
TAG=$1
OUT_DIR=$(cd "${2:-.}" && pwd)
REPO=$(git rev-parse --show-toplevel)
LFS_STORAGE=$(git -C "$REPO" rev-parse --path-format=absolute --git-common-dir)/lfs
LFS_URL=https://github.com/SCIInstitute/ShapeWorks.git/info/lfs
NAME=ShapeWorks-${TAG}-src-with-git-lfs

git -C "$REPO" rev-parse --verify --quiet "refs/tags/${TAG}" >/dev/null || { echo "No tag ${TAG}"; exit 1; }
[ -e "${OUT_DIR}/${NAME}" ] && { echo "${OUT_DIR}/${NAME} already exists"; exit 1; }
[ -e "${OUT_DIR}/${NAME}.zip" ] && { echo "${OUT_DIR}/${NAME}.zip already exists"; exit 1; }

cd "$OUT_DIR"
GIT_LFS_SKIP_SMUDGE=1 git -c advice.detachedHead=false clone --quiet --no-local --branch "$TAG" --depth 1 "file://${REPO}" "$NAME"
cd "$NAME"
git config lfs.storage "$LFS_STORAGE"
git config lfs.url "$LFS_URL"
git lfs pull

POINTERS=$(git lfs ls-files | grep -c ' - ' || true)
if [ "$POINTERS" -ne 0 ]; then
    echo "${POINTERS} LFS files are still pointers"
    exit 1
fi
EXPECTED=$(git ls-tree -r --name-only HEAD | wc -l | tr -d ' ')

rm -rf .git
cd ..
zip -qr -X "${NAME}.zip" "$NAME"

ACTUAL=$(zipinfo -1 "${NAME}.zip" | grep -vc '/$' || true)
if [ "$ACTUAL" -ne "$EXPECTED" ]; then
    echo "Zip has ${ACTUAL} files, but ${TAG} tracks ${EXPECTED}"
    exit 1
fi
rm -rf "$NAME"
echo "${OUT_DIR}/${NAME}.zip: ${ACTUAL} files, $(du -h "${NAME}.zip" | cut -f1)"
