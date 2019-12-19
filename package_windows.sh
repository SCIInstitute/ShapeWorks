#!/bin/bash -x


if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <version>"
    exit 1
fi

export SW_VERSION=$1
ROOT=`pwd`

cp -r ../build/bin/Release bin
rm -rf Post
windeployqt "bin/ShapeWorksStudio.exe"
windeployqt "bin/ShapeWorksView2.exe"
../NSISPortable/App/NSIS/makensis.exe shapeworks.nsi

