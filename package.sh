#!/bin/bash -x


ROOT=`pwd`

cd install/bin


if [[ "$OSTYPE" == "darwin"* ]]; then
    # Mac OSX
    macdeployqt ShapeWorksStudio.app -no-strip
    install_name_tool -add_rpath @executable_path/../Frameworks ShapeWorksStudio.app/Contents/MacOS/ShapeWorksStudio
    install_name_tool -add_rpath @executable_path/../../../../lib ShapeWorksStudio.app/Contents/MacOS/ShapeWorksStudio
    QT_LIB_LOCATION="@executable_path/ShapeWorksStudio.app/Contents/Frameworks"

    for i in * ; do
	install_name_tool -add_rpath $QT_LIB_LOCATION $i
    done

    
fi

cd $ROOT

mkdir -p install/examples/ellipsoid
cp -a UseCaseDocumentation/* install/examples/ellipsoid

