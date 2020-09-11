#!/bin/bash -x

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <version>"
    exit 1
fi

VERSION=$1
PLATFORM="windows"

if [[ "$VERSION" == "tag" ]]; then
    VERSION="ShapeWorks-$(git describe --tags)-${PLATFORM}"
fi

# Special case for when we are on the master branch (dev releases)
BRANCH=$(git rev-parse --abbrev-ref HEAD)
if [[ "$BRANCH" == "master" ]]; then
    NUMBER=$(git rev-list start_dev_releases..HEAD --count)
    HASH=$(git rev-parse --short HEAD)
    VERSION="ShapeWorks-dev-${NUMBER}-${HASH}-${PLATFORM}"
fi


export SW_VERSION=$VERSION
ROOT=`pwd`

cp -r ../build/bin/RelWithDebInfo bin
rm -rf Post

# Run auto-documentation
PATH=../build/bin/RelWithDebInfo:$PATH
python -c "import DocumentationUtils;DocumentationUtils.generateShapeWorksCommandDocumentation('docs/tools/ShapeWorksCommands.md')"
mkdocs build
mv site Documentation
cp -a Documentation bin/

# Remove tests, they won't work for users anyway
rm bin/*Tests.exe

windeployqt "bin/ShapeWorksStudio.exe"
windeployqt "bin/ShapeWorksView2.exe"
../NSISPortable/App/NSIS/makensis.exe shapeworks.nsi
mkdir artifacts
cp *.exe artifacts

