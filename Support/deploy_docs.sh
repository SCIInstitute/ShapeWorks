#!/bin/bash

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

# Update auto-documentation
PATH=$INSTALL_DIR/bin:$PATH

# Check pip status
echo "pip list:"
pip list

# check that 'shapeworks -h' is working
shapeworks -h
if [ $? -eq 0 ]; then
    echo "shapeworks -h is working"
else
    echo "shapeworks -h is not working"
    exit 1
fi

# install doxybook2
${GITHUB_WORKSPACE}/Support/build_docs.sh $INSTALL_DIR

git config --global user.name "${GITHUB_ACTOR}"
git config --global user.email "${GITHUB_ACTOR}@users.noreply.github.com"

remote_repo="https://x-access-token:${GITHUB_TOKEN}@github.com/${GITHUB_REPOSITORY}.git"

git reset --hard HEAD
git remote rm origin
git remote add origin "${remote_repo}"

# get remote gh-pages branch
git checkout --track origin/gh-pages
git pull --rebase

# build docs from master
git checkout master

# clean out old api docs as mkdocs will just find whatever is there.
rm -rf docs/api
mkdir docs/api

# update command line docs and generate markdown using doxygen
python Python/RunShapeWorksAutoDoc.py --md_filename docs/tools/ShapeWorksCommands.md
doxybook2 -i ${INSTALL_DIR}/Documentation/Doxygen/xml -o docs/api -c docs/doxygen/doxybook2.config.json

# use mike to mkdocs w/ version
mike deploy --config-file ./mkdocs.yml --title "6.3.0b (dev)" 6.3 dev --branch gh-pages --update-aliases
mike set-default dev

# update docs on github
git push origin gh-pages
