#!/bin/bash


if [ "$#" -ne 1 ]; then
    echo "Run from Shapeworks git clone root"
    echo ""
    echo "Usage: $0 <install_or_build_dir>"
    exit 1
fi
INSTALL_DIR=$1

if [ ! -d ".git" ]; then
    echo "Error, must be run from ShapeWorks git clone root"
    exit 1
fi

# Update auto-documentation
PATH=$INSTALL_DIR/bin:$PATH

# check that 'shapeworks -h' is working
shapeworks -h
if [ $? -eq 0 ]; then
    echo "shapeworks -h is working"
else
    echo "shapeworks -h is not working"
    exit 1
fi

if [ ! -f $INSTALL_DIR/bin/doxybook2 ] ; then
    if [[ "$OSTYPE" == "darwin"* ]]; then
	curl -L -o doxybook2-osx-amd64-v1.4.0.zip "https://github.com/matusnovak/doxybook2/releases/download/v1.4.0/doxybook2-osx-amd64-v1.4.0.zip"
	unzip doxybook2-osx-amd64-v1.4.0.zip bin/doxybook2 -d $INSTALL_DIR
    else
	curl -L -o doxybook2-linux-amd64-v1.4.0.zip "https://github.com/matusnovak/doxybook2/releases/download/v1.4.0/doxybook2-linux-amd64-v1.4.0.zip"
	unzip doxybook2-linux-amd64-v1.4.0.zip bin/doxybook2 -d $INSTALL_DIR
    fi
    rm -f doxybook*.zip
fi

python Python/RunShapeWorksAutoDoc.py --md_filename docs/tools/ShapeWorksCommands.md
# clean out old api docs as mkdocs will just find whatever is there.
rm -rf docs/api
mkdir docs/api
doxybook2 -i ${INSTALL_DIR}/Documentation/Doxygen/xml -o docs/api -c docs/doxygen/doxybook2.config.json
      
