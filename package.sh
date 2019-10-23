#!/bin/bash -x


if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <version>"
    exit 1
fi

VERSION=$1
ROOT=`pwd`

rm -rf "package/$VERSION"

mkdir "package"

cp -a install "package/${VERSION}"
mkdir -p package/${VERSION}/examples/ellipsoid
cp -a UseCaseDocumentation/* package/${VERSION}/examples/ellipsoid
cp conda_installs.sh package/${VERSION}
cp PACKAGE_README.txt package/${VERSION}/README.txt
cd "package/${VERSION}"
rm bin/h5cc bin/h5c++ bin/itkTestDriver
rm -rf include share

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
	install_name_tool -change ${PWD}/libitkgdcmopenjp2-5.0.1.dylib @rpath/libitkgdcmopenjp2-5.0.1.dylib $i
    done
    cd ..
else
    linuxdeployqt bin/ShapeWorksView2 -verbose=2
    linuxdeployqt bin/ShapeWorksStudio -verbose=2
fi

cd ${ROOT}/package
zip -r ${VERSION}.zip ${VERSION}

cd $ROOT


