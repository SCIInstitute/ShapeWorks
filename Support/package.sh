#!/bin/bash -x

if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <version> <install_dir> <install_dep_dir>"
    exit 1
fi

VERSION=$1
INSTALL_DIR=$2
INSTALL_DEP_DIR=$3
ROOT=`pwd`

if [[ "$OSTYPE" == "darwin"* ]]; then
    PLATFORM="mac"
else
    PLATFORM="linux"
fi

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

echo "Version: $VERSION"

rm -rf "package/$VERSION"

mkdir -p "package/$VERSION"

BASE_LIB=${INSTALL_DEP_DIR}/lib
cp -a $INSTALL_DEP_DIR/* "package/${VERSION}"
cp -a $INSTALL_DIR/* "package/${VERSION}"
cp -a Examples "package/${VERSION}"
cp -a Python "package/${VERSION}"
cp conda_installs.sh package/${VERSION}
cp docs/about/release-notes.md package/${VERSION}

if [[ "$OSTYPE" == "darwin"* ]]; then
    cp docs/users/Mac_README.txt package/${VERSION}/README.txt
    if [ $? -ne 0 ]; then
	echo "Failed to copy Mac package README"
	exit 1
    fi
else
    cp docs/users/Linux_README.txt package/${VERSION}/README.txt
    if [ $? -ne 0 ]; then
	echo "Failed to copy Linux package README"
	exit 1
    fi
fi

cd "package/${VERSION}"
rm bin/h5cc bin/h5c++ bin/itkTestDriver
rm -rf include share v3p plugins

if [[ "$OSTYPE" == "darwin"* ]]; then
    # Mac OSX
    cd bin
    macdeployqt ShapeWorksStudio.app -no-strip
    install_name_tool -add_rpath @executable_path/../Frameworks ShapeWorksStudio.app/Contents/MacOS/ShapeWorksStudio
    install_name_tool -add_rpath @executable_path/../../../../lib ShapeWorksStudio.app/Contents/MacOS/ShapeWorksStudio
    QT_LIB_LOCATION="@executable_path/ShapeWorksStudio.app/Contents/Frameworks"
    QT_LOADER_LIB_LOCATION="@loader_path/ShapeWorksStudio.app/Contents/Frameworks"


    # copy platform plugins for View2
    cp -a ShapeWorksStudio.app/Contents/PlugIns .

    for i in *.so ; do
	install_name_tool -add_rpath "@loader_path/../lib" $i
	install_name_tool -add_rpath $QT_LOADER_LIB_LOCATION $i
    done

    for i in * ; do
	install_name_tool -add_rpath $QT_LIB_LOCATION $i
    done

    cd ../lib
    # Copy libraries from anaconda
    conda_libs="libpython"
    for clib in $conda_libs; do
        cp ${CONDA_PREFIX}/lib/${clib}* .
    done
    # Fix transitive loaded libs
    for i in *.dylib ; do
	install_name_tool -change ${BASE_LIB}/libitkgdcmopenjp2-5.0.1.dylib @rpath/libitkgdcmopenjp2-5.0.1.dylib $i
    done
    install_name_tool -id @rpath/libitkgdcmopenjp2-5.0.1.dylib libitkgdcmopenjp2-5.0.1.dylib

    cd ..
else
    # Copy libraries from anaconda
    conda_libs="libboost_iostreams libbz2 liblzma liblz4 libtbb libHalf libpython"
    for clib in $conda_libs; do
        cp ${CONDA_PREFIX}/lib/${clib}* lib
    done

    cd bin
    linuxdeployqt ShapeWorksView2 -verbose=2
    linuxdeployqt ShapeWorksStudio -verbose=2
fi

# Run auto-documentation
cd $ROOT
PATH=$ROOT/package/${VERSION}/bin:$PATH
# check that 'shapeworks -h' is working
shapeworks -h
if [ $? -eq 0 ]; then
    echo "shapeworks -h is working"
else
    echo "shapeworks -h is not working"
    exit 1
fi
python Python/RunShapeWorksAutoDoc.py --md_filename docs/tools/ShapeWorksCommands.md
mkdocs build
mv site Documentation
cp -a Documentation "${ROOT}/package/${VERSION}"

mkdir ${ROOT}/artifacts
cd ${ROOT}/package
zip -r ${ROOT}/artifacts/${VERSION}.zip ${VERSION}
if [ $? -ne 0 ]; then
    echo "Failed to zip artifact"
    exit 1
fi

# Additionally on Mac, create an installer
if [[ "$OSTYPE" == "darwin"* ]]; then
    cp ${ROOT}/docs/users/Mac_README.txt ${VERSION}/README.txt
    pkgbuild --quiet --analyze --root ${VERSION} ShapeWorks.plist
    plutil -replace BundleIsRelocatable -bool NO ShapeWorks.plist
    pkgbuild --component-plist ShapeWorks.plist --install-location /Applications/ShapeWorks --root ${VERSION} --identifier edu.utah.sci.shapeworks ${ROOT}/artifacts/${VERSION}.pkg
fi

cd $ROOT


