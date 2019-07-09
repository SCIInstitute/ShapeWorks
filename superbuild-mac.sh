echo "ShapeWorks SuperBuild for Mac"

if ! [ -x "$(command -v qmake)" ]; then
    echo 'Error: Please make sure Qt4 is installed and that qmake is in the path.' >&2
    echo 'Please Download Qt4 from: https://download.qt.io/archive/qt/4.8/4.8.7/' >&2
  exit 1
fi

root=$(pwd)
depDir=${root}/shapeworks-dependencies
rm -rf ${depDir}
mkdir $depDir
 
# ######## VXL ########
cd ${depDir}
git clone https://github.com/vxl/vxl.git
cd vxl/
git checkout tags/github-migration
/usr/bin/sed -i bak '1,10 s/^/#/' CMakeLists.txt
/usr/bin/sed -i bak 's/JPEG_FOUND/JPEG_FOUND_DISABLE/' core/vil/CMakeLists.txt
/usr/bin/sed -i bak 's/JPEG_FOUND/JPEG_FOUND_DISABLE/' core/vil1/CMakeLists.txt
cd ${depDir}

rm -rf vxl-build/
mkdir vxl-build/

cd vxl-build/
cmake -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DBUILD_CORE_VIDEO:BOOL=OFF \
          -DBUILD_BRL:BOOL=OFF -DBUILD_CONTRIB:BOOL=OFF -DVNL_CONFIG_LEGACY_METHODS=ON -DVCL_STATIC_CONST_INIT_FLOAT=0 -Wno-dev ../vxl/
make -j10


######## VTK ########
cd ${depDir}
git clone https://gitlab.kitware.com/vtk/vtk.git
cd vtk/
git checkout tags/v5.10.1
cd ${depDir}
 
rm -rf vtk-build/
mkdir vtk-build/
 
cd vtk-build/
cmake -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DVTK_USE_QT:BOOL=ON -DCMAKE_C_FLAGS=-DGLX_GLXEXT_LEGACY -DCMAKE_CXX_FLAGS=-DGLX_GLXEXT_LEGACY -DVTK_USE_SYSTEM_TIFF=ON -Wno-dev ../vtk/
make -j10

######## ITK ########
cd ${depDir}
git clone https://github.com/InsightSoftwareConsortium/ITK.git
cd ITK/
git checkout tags/v4.7.2
/usr/bin/sed -i bak 's/nout > 0/this->nout/' Modules/ThirdParty/VNL/src/vxl/v3p/netlib/linalg/lsqrBase.cxx
cd ${depDir}
 
rm -rf ITK-build/
mkdir ITK-build/
 
cd ITK-build/
cmake -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DBUILD_EXAMPLES:BOOL=OFF -DITK_USE_SYSTEM_VXL=on -Wno-dev ../ITK/
make -j10

######## ShapeWorks ########
cd ${root}
git clone https://github.com/SCIInstitute/shapeworks.git
cd shapeworks
/usr/bin/sed -i bak '11,13 s/^/#/' CMakeLists.txt
git submodule init
git submodule update
cd ShapeWorks-Prep
git checkout master
git submodule init
git submodule update
cd Source/CommonLibraries/fim_v4
git checkout master
cd ../../../../ShapeWorks-Post
git checkout master
cd ../ShapeWorks-Run
git checkout master
git submodule init
git submodule update
cd source/fim_v4
git checkout master
cd ${root}
 
rm -rf shapeworks-build/
mkdir shapeworks-build/
 
cd shapeworks-build/
 
cmake -DITK_DIR=${depDir}/ITK-build -DVXL_DIR=${depDir}/vxl-build -DVTK_DIR=${depDir}/vtk-build -DBuild_Post:BOOL=OFF -DUSE_OPENMP=OFF -Wno-dev ../shapeworks
make -j10
cd ${root}
