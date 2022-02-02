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

# remove local gh-pages branch, if any
git branch -D gh-pages
# get remote gh-pages branch
git checkout --track origin/gh-pages
git pull --rebase
git checkout master
python Python/RunShapeWorksAutoDoc.py --md_filename docs/tools/ShapeWorksCommands.md
doxybook2 -i ${INSTALL_DIR}/Documentation/Doxygen/xml -o docs/api -c docs/doxygen/doxybook2.config.json
mkdocs gh-deploy --config-file "${GITHUB_WORKSPACE}/mkdocs.yml"

      
