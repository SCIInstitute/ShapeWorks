#!/bin/bash -x

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <version>"
    exit 1
fi

VERSION=$1
PLATFORM="windows"


echo "VERSION = $VERSION"
echo "PR_NUMBER = $PR_NUMBER"
echo "GITHUB_REF = $GITHUB_REF"
echo "PLATFORM = $PLATFORM"

if [[ "$PR_NUMBER" != "" ]]; then
    VERSION="ShapeWorks-PR-${PR_NUMBER}-${PLATFORM}"
else
    if [[ "$VERSION" == "tag" ]]; then
	VERSION="ShapeWorks-$(git describe --tags)-${PLATFORM}"
    fi
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

BUILD="/c/build"
CONDA_LOC="/c/Miniconda3/envs/shapeworks"
cp ${CONDA_LOC}/python*.dll ${CONDA_LOC}/Library/bin/zlib.dll ${CONDA_LOC}/Library/bin/tbb12.dll ${CONDA_LOC}/Library/bin/zstd.dll ${CONDA_LOC}/Library/bin/libpng16.dll ${CONDA_LOC}/Library/bin/half.dll ${CONDA_LOC}/Library/bin/boost_filesystem.dll ${CONDA_LOC}/Library/bin/spdlog.dll $BUILD/bin/Release


cp -r $BUILD/bin/Release bin
rm bin/*Tests.pdb bin/Recon*.pdb bin/Mesh*.pdb
rm -rf Post

# Build python packages
for package in DataAugmentationUtilsPackage DatasetUtilsPackage DeepSSMUtilsPackage DocumentationUtilsPackage ShapeCohortGenPackage shapeworks ; do
    python Python/${package}/setup.py sdist --formats=zip
done
mv dist/* Python


# Remove tests, they won't work for users anyway
rm bin/*Tests.exe
rm -rf docs

windeployqt "bin/ShapeWorksStudio.exe"
../NSISPortable/App/NSIS/makensis.exe -V4 shapeworks.nsi 
mkdir artifacts
cp *.exe artifacts

