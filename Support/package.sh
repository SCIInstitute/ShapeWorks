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
    
if [[ "$VERSION" == "tag" ]]; then
    VERSION="ShapeWorks-$(git describe --tags)-$PLATFORM"
fi

# Special case for when we are on the master branch (dev releases)
BRANCH=$(git rev-parse --abbrev-ref HEAD)
if [[ "$BRANCH" == "master" ]]; then
    VERSION="ShapeWorks-dev-$PLATFORM"
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
cp ChangeLog package/${VERSION}
cp documentation/install/PACKAGE_README.txt package/${VERSION}/README.txt
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

    # copy platform plugins for View2
    cp -a ShapeWorksStudio.app/Contents/PlugIns .

    for i in * ; do
	install_name_tool -add_rpath $QT_LIB_LOCATION $i
    done

    # Fix transitive loaded libs
    cd ../lib
    for i in *.dylib ; do
	install_name_tool -change ${BASE_LIB}/libitkgdcmopenjp2-5.0.1.dylib @rpath/libitkgdcmopenjp2-5.0.1.dylib $i
    done

    install_name_tool -id @rpath/libitkgdcmopenjp2-5.0.1.dylib libitkgdcmopenjp2-5.0.1.dylib
    
    cd ..
else
    # Copy libraries from anaconda
    conda_libs="libboost_iostreams libbz2 liblzma liblz4 libtbb libHalf"
    for clib in $conda_libs; do
        cp ${CONDA_PREFIX}/lib/${clib}* lib
    done

    cd bin
    linuxdeployqt ShapeWorksView2 -verbose=2
    linuxdeployqt ShapeWorksStudio -verbose=2
fi

mkdir ${ROOT}/artifacts
cd ${ROOT}/package
cp ${ROOT}/Documentation/install/PACKAGE_README.txt ${VERSION}/README.txt
zip -r ${ROOT}/artifacts/${VERSION}.zip ${VERSION}

# Additionally on Mac, create an installer
if [[ "$OSTYPE" == "darwin"* ]]; then
    cp ${ROOT}/Documentation/install/Mac_README.txt ${VERSION}/README.txt
    pkgbuild --quiet --analyze --root ${VERSION} ShapeWorks.plist
    plutil -replace BundleIsRelocatable -bool NO ShapeWorks.plist
    pkgbuild --component-plist ShapeWorks.plist --install-location /Applications/ShapeWorks --root ${VERSION} --identifier edu.utah.sci.shapeworks ${ROOT}/artifacts/${VERSION}.pkg
fi

cd $ROOT


