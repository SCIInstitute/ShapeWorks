#!/bin/bash -x

set -e

# Don't run this script if you are not a GitHub Action

if [[ -z "${GITHUB_TOKEN}" ]]; then
    echo "This script should only be run by GitHub Actions"
    exit 1
fi

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <install_dir>"
    exit 1
fi
INSTALL_DIR=$1

# Git ref the docs are built from.  Normally "master", which is always the
# development version.  Run the workflow manually against a release tag (e.g.
# v6.7.0) to rebuild the docs for a version that has already shipped.
DOCS_REF=${DOCS_REF:-master}

# Update auto-documentation
PATH=$INSTALL_DIR/bin:$PATH

# Check pip status
echo "pip list:"
pip list

# check that 'shapeworks -h' is working
if ! shapeworks -h; then
    echo "shapeworks -h is not working"
    exit 1
fi
echo "shapeworks -h is working"

# install doxybook2
${GITHUB_WORKSPACE}/Support/build_docs.sh $INSTALL_DIR

git config --global user.name "${GITHUB_ACTOR}"
git config --global user.email "${GITHUB_ACTOR}@users.noreply.github.com"
git config http.postBuffer 524288000

remote_repo="https://x-access-token:${GITHUB_TOKEN}@github.com/${GITHUB_REPOSITORY}.git"

git reset --hard HEAD
git remote rm origin
git remote add origin "${remote_repo}"

# Make gh-pages available locally for mike, but do not check it out.  nbstripout
# installs an *.ipynb clean filter into .git/info/attributes, which applies on
# every branch, so checking out gh-pages leaves the notebooks it publishes dirty
# and the working tree can never be switched back off of it.
git fetch origin gh-pages

# build docs from the requested ref
git checkout ${DOCS_REF}

# clean out old api docs as mkdocs will just find whatever is there.
rm -rf docs/api
mkdir docs/api

# update command line docs and generate markdown using doxygen
python Python/RunShapeWorksAutoDoc.py --md_filename docs/tools/ShapeWorksCommands.md
doxybook2 -i ${INSTALL_DIR}/Documentation/Doxygen/xml -o docs/api -c docs/doxygen/doxybook2.config.json

# Derive the docs version (e.g. "6.8") from the source tree so this script does
# not have to be edited every release.
SW_MAJOR=$(sed -n 's/^SET(SHAPEWORKS_MAJOR_VERSION \([0-9][0-9]*\).*/\1/p' CMakeLists.txt)
SW_MINOR=$(sed -n 's/^SET(SHAPEWORKS_MINOR_VERSION \([0-9][0-9]*\).*/\1/p' CMakeLists.txt)
if [[ -z "${SW_MAJOR}" || -z "${SW_MINOR}" ]]; then
    echo "Could not determine the ShapeWorks version from CMakeLists.txt"
    exit 1
fi
DOCS_VERSION="${SW_MAJOR}.${SW_MINOR}"

# master is the development version, so it is titled "<version> (dev)" and owns
# the "dev" alias.  Anything else is a release rebuild and gets a plain title.
# "latest" always points at the most recently released version; it is moved by
# rebuilding that release's docs with DOCS_ALIAS=latest.
if [ "${DOCS_REF}" = "master" ]; then
    DOCS_TITLE=${DOCS_TITLE:-"${DOCS_VERSION} (dev)"}
    DOCS_ALIAS=${DOCS_ALIAS:-dev}
else
    DOCS_TITLE=${DOCS_TITLE:-"${DOCS_VERSION}"}
    DOCS_ALIAS=${DOCS_ALIAS:-}
fi

# use mike to mkdocs w/ version
mike deploy --config-file ./mkdocs.yml --title "${DOCS_TITLE}" --branch gh-pages --update-aliases "${DOCS_VERSION}" ${DOCS_ALIAS}
mike set-default latest --branch gh-pages

# update docs on github
git push origin gh-pages
