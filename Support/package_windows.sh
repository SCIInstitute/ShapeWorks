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
CONDA_LOC="/c/Miniconda/envs/shapeworks"

# Check if bundled Python is present (installed by cmake into lib/python/)
if [ -d "$BUILD/install/lib/python" ]; then
    echo "Bundled Python detected — skipping conda Python DLL copy"

    # Copy DLLs placed in install/bin/ by InstallBundledPython.cmake
    # (python312.dll, python3.dll, libffi-8.dll, libssl, libcrypto, etc.)
    cp $BUILD/install/bin/*.dll $BUILD/bin/Release/ 2>/dev/null || true

    # Still need non-Python dependency DLLs from conda
    for dll in zlib.dll tbb12.dll zstd.dll libpng16.dll half.dll boost_filesystem.dll spdlog.dll pcre2-16.dll ; do
        if [ -f "${CONDA_LOC}/Library/bin/${dll}" ]; then
            cp "${CONDA_LOC}/Library/bin/${dll}" $BUILD/bin/Release
        fi
    done
else
    ls -la ${CONDA_LOC}/Library/bin

    cp ${CONDA_LOC}/python*.dll ${CONDA_LOC}/Library/bin/zlib.dll ${CONDA_LOC}/Library/bin/tbb12.dll ${CONDA_LOC}/Library/bin/zstd.dll ${CONDA_LOC}/Library/bin/libpng16.dll ${CONDA_LOC}/Library/bin/half.dll ${CONDA_LOC}/Library/bin/boost_filesystem.dll ${CONDA_LOC}/Library/bin/spdlog.dll ${CONDA_LOC}/Library/bin/*ffi*.dll ${CONDA_LOC}/Library/bin/libbz2.dll ${CONDA_LOC}/Library/bin/libssl-3-x64.dll ${CONDA_LOC}/Library/bin/libcrypto-3-x64.dll ${CONDA_LOC}/Library/bin/pcre2-16.dll $BUILD/bin/Release
fi

# install visual studio redistributables
WIN_DIR="/c/Windows/system32"
for i in "msvcp140.dll" "msvcp140_1.dll" "vcomp140.dll" "vcruntime140.dll" "vcruntime140_1.dll" ; do
    cp ${WIN_DIR}/${i} $BUILD/bin/Release
done

cp -r $BUILD/bin/Release bin
rm bin/*Tests.pdb bin/Recon*.pdb bin/Mesh*.pdb
rm -rf Post

# Copy bundled Python from install tree if present
if [ -d "$BUILD/install/lib/python" ]; then
    echo "Copying bundled Python to package"
    mkdir -p lib
    cp -r $BUILD/install/lib/python lib/
fi

# Build python package tarballs
# Pip can't install these otherwise from a read-only area like C:\Program Files\ShapeWorks
if [ ! -d "$BUILD/install/lib/python" ]; then
    # Only build tarballs for legacy conda workflow
    for package in DataAugmentationUtilsPackage DatasetUtilsPackage MONAILabelPackage DeepSSMUtilsPackage DocumentationUtilsPackage ShapeCohortGenPackage shapeworks ; do
        cd Python
        tar czvf ${package}.tar.gz $package
        cd ..
    done
fi

# Remove tests, they won't work for users anyway
rm bin/*Tests.exe
rm -rf docs

windeployqt "bin/ShapeWorksStudio.exe"
../NSISPortable/App/NSIS/makensis.exe -V4 shapeworks.nsi 
mkdir artifacts
cp *.exe artifacts

