root=$(pwd)
depDir=${root}/shapeworks-dependencies

rm -rf ${depDir}
mkdir $depDir

Build_ITK=1
Build_VTK=1
Build_VXL=1

VTK_DIR=${depDir}/ITK-build
ITK_DIR=${depDir}/vtk-build
VXL_DIR=${depDir}/vxl-build

Build_View2=OFF # vtk will be build with QT only if this is ON
Build_Post=OFF #gcc5 and above required

######## VXL ########
if [ $Build_VXL -eq 1 ]
then
    cd ${depDir}
    git clone https://github.com/vxl/vxl.git
    cd vxl/
    git checkout -f tags/github-migration
    sed -i '1,10 s/^/#/' CMakeLists.txt
    cd ${depDir}

    rm -rf vxl-build/
    mkdir vxl-build/

    cd vxl-build/
    cmake -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -Wno-dev ../vxl/
    make -j100
fi

######## VTK ########
if [ $Build_VTK -eq 1 ]
then
    cd ${depDir}
    git clone https://gitlab.kitware.com/vtk/vtk.git
    cd vtk/
    git checkout -f tags/v5.10.1
    cd ${depDir}

    ## fix for deprecated sstream in vtkIOStream.h, needed for gcc 4.8.5 in Red Hat Linux
    sed -i "108,112 s/^/\/\//" vtk/Common/vtkIOStream.h
    sed -i "113i #include <sstream>" vtk/Common/vtkIOStream.h
    sed -i "114i using std::basic_istringstream;" vtk/Common/vtkIOStream.h
    sed -i "115i using std::basic_ostringstream;" vtk/Common/vtkIOStream.h
    sed -i "116i using std::basic_stringstream;" vtk/Common/vtkIOStream.h


    rm -rf vtk-build/
    mkdir vtk-build/

    cd vtk-build/
    cmake -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DVTK_USE_QT:BOOL=${Build_View2} -DCMAKE_C_FLAGS=-DGLX_GLXEXT_LEGACY -DCMAKE_CXX_FLAGS=-DGLX_GLXEXT_LEGACY -Wno-dev ../vtk/
    make -j100
fi

######## ITK ########
if [ $Build_ITK -eq 1 ]
then
    cd ${depDir}
    git clone https://github.com/InsightSoftwareConsortium/ITK.git
    cd ITK/
    git checkout -f tags/v4.7.2
    cd ${depDir}

    rm -rf ITK-build/
    mkdir ITK-build/

    cd ITK-build/
    cmake -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DBUILD_EXAMPLES:BOOL=OFF -Wno-dev ../ITK/
    make -j100
fi
######## ShapeWorks ########
cd ${root}
git clone https://github.com/SCIInstitute/shapeworks.git
cd shapeworks
sed -i '11,13 s/^/#/' CMakeLists.txt
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

cmake -DITK_DIR=${ITK_DIR} -DVXL_DIR=${VXL_DIR} -DVTK_DIR=${VTK_DIR} -DBuild_Post:BOOL=${Build_Post} -DBuild_View2:BOOL=${Build_View2} -Wno-dev -Wno-deprecated ../shapeworks
make -j100
cd ${root}
