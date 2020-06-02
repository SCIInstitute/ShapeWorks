#!/bin/bash -x


if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <version>"
    exit 1
fi

VERSION=$1

if [[ "$VERSION" == "tag" ]]; then
    VERSION="ShapeWorks-$(git describe --tags)-windows"
fi

# Special case for when we are on the master branch (dev releases)
BRANCH=$(git rev-parse --abbrev-ref HEAD)
if [[ "$BRANCH" == "master" ]]; then
    VERSION="ShapeWorks-dev-windows"
fi


export SW_VERSION=$VERSION
ROOT=`pwd`

cp -r ../build/bin/Release bin
rm -rf Post

# Remove tests, they won't work for users anyway
rm bin/*Tests.exe

windeployqt "bin/ShapeWorksStudio.exe"
windeployqt "bin/ShapeWorksView2.exe"
../NSISPortable/App/NSIS/makensis.exe shapeworks.nsi
mkdir artifacts
cp *.exe artifacts

