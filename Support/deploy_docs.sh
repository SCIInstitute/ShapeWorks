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
python Python/RunShapeWorksAutoDoc.py --md_filename docs/tools/ShapeWorksCommands.md

git config --global user.name "${GITHUB_ACTOR}"
git config --global user.email "${GITHUB_ACTOR}@users.noreply.github.com"

remote_repo="https://x-access-token:${GITHUB_TOKEN}@github.com/${GITHUB_REPOSITORY}.git"

git reset --hard HEAD
git remote rm origin
git remote add origin "${remote_repo}"

git branch -D gh-pages
git checkout gh-pages
git pull --rebase
git checkout master
mkdocs gh-deploy --config-file "${GITHUB_WORKSPACE}/mkdocs.yml"

      
