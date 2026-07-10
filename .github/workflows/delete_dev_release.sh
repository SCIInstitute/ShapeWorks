#!/bin/bash
# Delete the existing rolling "dev-*" prerelease and its git tag so that
# softprops/action-gh-release recreates the release/tag at the current commit.
# Without this, an already-existing tag is not moved and the dev release would
# keep pointing at a stale commit.
#
# Usage: delete_dev_release.sh <tag>
# Requires: GH_TOKEN (GITHUB_TOKEN), GITHUB_REPOSITORY (provided by GitHub Actions)
#
# Note: no `set -x` here on purpose -- it would echo the auth header (token).

echo "#############################"
echo "# Delete previous dev release"
echo "#############################"

TAG="$1"
if [[ -z "$TAG" ]]; then
    echo "Usage: $0 <tag>"
    exit 1
fi
if [[ -z "$GH_TOKEN" ]]; then
    echo "GH_TOKEN not set; skipping dev release cleanup"
    exit 0
fi

API="https://api.github.com/repos/${GITHUB_REPOSITORY}"
AUTH="Authorization: token ${GH_TOKEN}"

# Find and delete the existing release for this rolling tag (if any).
release_id=$(curl -sS -H "$AUTH" "${API}/releases/tags/${TAG}" \
    | grep -m1 '"id":' | grep -o '[0-9]\+' | head -1 || true)

if [[ -n "$release_id" ]]; then
    echo "Deleting existing release ${release_id} for tag ${TAG}"
    curl -sS -o /dev/null -w "delete release HTTP %{http_code}\n" \
        -X DELETE -H "$AUTH" "${API}/releases/${release_id}" || true
else
    echo "No existing release found for tag ${TAG}"
fi

# Delete the tag ref so the tag is recreated at the current commit on deploy.
echo "Deleting tag ref ${TAG} (if present)"
curl -sS -o /dev/null -w "delete tag ref HTTP %{http_code}\n" \
    -X DELETE -H "$AUTH" "${API}/git/refs/tags/${TAG}" || true

echo "Dev release cleanup complete for ${TAG}"
exit 0
