#!/bin/bash

#set -v   #verbose execution for debugging
#set -x   #tracing execution for debugging (echos all commands from script)

# check to make sure `source` was not used
(return 0 2>/dev/null) && sourced=1 || sourced=0
if [[ "$sourced" == "1" ]]; then
  echo "ERROR: do not use \"source\" when executing this script"
  return
fi

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# defaults
BUILD_CLEAN=0
CLEAN_AFTER=0
NUM_PROCS=8
BUILD_GUI=1
BUILD_STUDIO=0
BUILD_SHAPEWORKS=1
BUILD_TYPE="Release"
BUILD_LOG="build_dependencies.log"
VTK_VER="v9.1.0"
VTK_VER_STR="9.1"
ITK_VER="v5.2.1"
ITK_VER_STR="5.2"
QT_MIN_VER="5.15.4"
XLNT_VER="538f80794c7d736afc0a452d21313606cc5538fc"
JKQTPLOTTER_VER="v2022.11.30-refix-rpath"
OpenVDB_VER="v9.1.0"
libigl_VER="v2.3.0"
geometry_central_VER="8b20898f6c7be1eab827a9f720c8fd45e58ae63c" # This library isn't using tagged versions
ACVD_VER="012917d300f1dde8552981e5a30031a23937625f" # This library isn't using tagged version

#WIN_CFLAGS="-FS /Zi /GL /MD /O2 /Ob3 /Zm250 /DNDEBUG /EHsc"  # windows release compilation flags
WIN_CFLAGS="-FS /Zi /MD /O2 /Ob3 /Zm250 /DNDEBUG /EHsc"  # windows release compilation flags
#WIN_LFLAGS="-LTCG /DEBUG" # windows release compilation flags
WIN_LFLAGS="-DEBUG" # windows release compilation flags
#FLAGS="-g" # turn on symbols for all builds
FLAGS=""

usage()
{
  echo "usage: ./build_dependencies.sh [[-n=<num-procs>] [-i=<install_path>] [-b=<build_path>] [--clean] [--no-gui] | [-h | --help]]"
  echo ""
  echo "If using Anaconda to install prerequisites please first run:"
  echo "source ./install_shapeworks.sh"
  echo ""
  echo "For GUI applications, please make sure the ShapeWorks conda env bin dir is at the head of the path."
  echo ""
  echo "Arguments:"
  echo "  -h,--help               : Show this screen."
  echo "  --clean                 : Remove all build directories and clone implicit dependencies."
  echo "                          : (note that user-specified paths such as --itk-dir=<path> will not be deleted)."
  echo "  --no-gui                : Do not build the dependencies with GUI support, which require Qt."
  echo "                          : The GUI is built by default if qmake >= $QT_MIN_VER is found in the path."
  echo "  -b,--build-dir=<path>   : Build directory for dependencies."
  echo "                          : By default uses a subdirectory of the current directory called 'dependencies/build'."
  echo "  -i,--install-dir=<path> : Install directory for dependencies."
  echo "                          : By default uses a subdirectory of the current directory called 'dependencies/install'."
  echo "  -n,--num-procs=<num>    : Number of processors to use for parallel builds (default is 8)."
  echo "  --build-type=<type>     : Build type (Debug, Release, RelWithDebInfo, MinSizeRel), default is Release"
  echo "  --clean-after           : Clean build folders after build to save disk space"
  echo ""
  echo "Example: ./build_dependencies.sh --num-procs=8 --install-dir=/path/to/desired/installation"
  echo "Build results are saved in ${BUILD_LOG}."
}

# full names of relative paths are needed to specify locations of dependencies
function fullpath {
  # unix full paths start with `/`; second character of windows full paths is `:`
  if [[ $1 = /* || ${1:1:1} = \: ]]; then
    echo $1
  else
    echo `pwd`/$1
  fi
}

parse_command_line()
{
  while [ "$1" != "" ]; do
    case $1 in
      -i=*|--install-dir=*)   INSTALL_DIR="${1#*=}"
                              INSTALL_DIR=`fullpath "${INSTALL_DIR}"`
                              ;;
      -b=*|--build-dir=*)     BUILD_DIR="${1#*=}"
                              BUILD_DIR=`fullpath "${BUILD_DIR}"`
                              ;;
      -n=*|--num-procs=*)     NUM_PROCS="${1#*=}"
                              ;;
      --build-type=*)         BUILD_TYPE="${1#*=}"
			      ;;
      --no-gui )              BUILD_GUI=0
                              ;;
      --clean )               BUILD_CLEAN=1
                              ;;
      --clean-after )         CLEAN_AFTER=1
                              ;;
      -h | --help )           usage
                              exit
                              ;;
      * )                     echo "----------------------"
                              echo "ERROR: Unknown parameter \"$1\""
                              echo "(remember to use '=' between a parameter name and its value)"
                              echo "----------------------"
                              echo "use '--help' to show usage"
                              exit 1
    esac
    shift
  done
}

# test for required version of something (versions with dots are okay):
# usage:
#   at_least_required_version "name" $curr $required
#   if [[ $? -eq 1 ]]; then echo "version sufficient"; fi
at_least_required_version()
{
  packagename=$1
  current=$2
  required=$3
  echo "## verifying ${packagename}..."
  if [ "$(printf '%s\n' "$required" "$current" | sort -V | head -n1)" = "$required" ]; then 
    echo "## SUCCESS: $current is greater than or equal to $required"
    return 1
  else
    echo "## FAILURE: ${packagename} version must be at least $required"
    return 0
  fi
}

build_vtk()
{
  echo ""
  echo "## Building vtk..."
  cd ${BUILD_DIR}
  git clone https://gitlab.kitware.com/vtk/vtk.git
  cd vtk
  git checkout -f tags/${VTK_VER}

  patch -p1 < ${SCRIPT_DIR}/Support/vtk-9.1.patch
  cat Rendering/OpenGL2/vtkCocoaRenderWindow.mm
  
  if [[ $BUILD_CLEAN = 1 ]]; then rm -rf build; fi
  mkdir -p build && cd build
  if [[ $OSTYPE == "msys" ]]; then
      cmake -DCMAKE_CXX_FLAGS_RELEASE="$WIN_CFLAGS" -DCMAKE_C_FLAGS_RELEASE="$WIN_CFLAGS" -DCMAKE_SHARED_LINKER_FLAGS_RELEASE="$WIN_LFLAGS" -DCMAKE_EXE_LINKER_FLAGS_RELEASE="$WIN_LFLAGS" -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" -DBUILD_SHARED_LIBS:BOOL=OFF -DBUILD_TESTING:BOOL=OFF -DVTK_Group_Qt:BOOL=${BUILD_GUI} -DVTK_QT_VERSION=5 -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DVTK_PYTHON_VERSION=3 -DVTK_GROUP_ENABLE_Qt=YES -DVTK_MODULE_ENABLE_VTK_GUISupportQtQuick:STRING=DONT_WANT -DBUILD_EXAMPLES:BOOL=OFF -DVTK_SMP_ENABLE_TBB=ON -DVTK_SMP_IMPLEMENTATION_TYPE=TBB -Wno-dev ..
      cmake --build . --config ${BUILD_TYPE} --parallel || exit 1
      cmake --build . --config ${BUILD_TYPE} --target install
      VTK_DIR="${INSTALL_DIR}/lib/cmake/vtk-${VTK_VER_STR}"
      VTK_DIR=$(echo $VTK_DIR | sed s/\\\\/\\//g)
  else
      cmake -DCMAKE_CXX_FLAGS="$FLAGS" -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DBUILD_SHARED_LIBS:BOOL=OFF -DBUILD_TESTING:BOOL=OFF -DVTK_Group_Qt:BOOL=${BUILD_GUI} -DVTK_QT_VERSION=5 -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DVTK_PYTHON_VERSION=3 -DVTK_GROUP_ENABLE_Qt=YES -DVTK_MODULE_ENABLE_VTK_GUISupportQtQuick:STRING=DONT_WANT -DBUILD_EXAMPLES:BOOL=OFF ${VTK_EXTRA_OPTIONS} -DVTK_MODULE_USE_EXTERNAL_VTK_pugixml=ON -DVTK_SMP_ENABLE_TBB=ON -DVTK_SMP_IMPLEMENTATION_TYPE=TBB -Wno-dev ..
      make -j${NUM_PROCS} install || exit 1
      VTK_DIR=${INSTALL_DIR}/lib/cmake/vtk-${VTK_VER_STR}
  fi
  if [[ $CLEAN_AFTER = 1 ]]; then make clean; fi
}

build_itk()
{
  echo ""
  echo "## Building itk..."
  cd ${BUILD_DIR}
  git clone https://github.com/InsightSoftwareConsortium/ITK.git
  cd ITK
  git checkout -f tags/${ITK_VER}

  if [[ $BUILD_CLEAN = 1 ]]; then rm -rf build; fi
  mkdir -p build && cd build

  if [[ $OSTYPE == "msys" ]]; then
      cmake -DCMAKE_CXX_FLAGS_RELEASE="$WIN_CFLAGS" -DCMAKE_C_FLAGS_RELEASE="$WIN_CFLAGS" -DCMAKE_SHARED_LINKER_FLAGS_RELEASE="$WIN_LFLAGS" -DCMAKE_EXE_LINKER_FLAGS_RELEASE="$WIN_LFLAGS" -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" -DBUILD_SHARED_LIBS:BOOL=OFF -DBUILD_TESTING:BOOL=OFF -DBUILD_EXAMPLES:BOOL=OFF -DVTK_DIR="${VTK_DIR}" -DITK_USE_SYSTEM_EIGEN=on -DEigen3_DIR=${EIGEN_DIR} -DModule_ITKVtkGlue:BOOL=ON -DModule_ITKDeprecated:BOOL=ON -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DModule_ITKIOHDF5=OFF -DITK_USE_SYSTEM_HDF5=ON -Wno-dev ..
      
      cmake --build . --config ${BUILD_TYPE} --parallel || exit 1
      cmake --build . --config ${BUILD_TYPE} --target install
  else
      cmake -DCMAKE_CXX_FLAGS="$FLAGS" -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DBUILD_SHARED_LIBS:BOOL=OFF -DBUILD_TESTING:BOOL=OFF -DBUILD_EXAMPLES:BOOL=OFF -DModule_ITKVtkGlue:BOOL=ON -DModule_ITKDeprecated:BOOL=ON -DITK_USE_SYSTEM_EIGEN=on -DEigen3_DIR=${EIGEN_DIR} -DVTK_DIR="${VTK_DIR}" -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -Wno-dev ..
      make -j${NUM_PROCS} install || exit 1
  fi

  ITK_DIR=${INSTALL_DIR}/lib/cmake/ITK-${ITK_VER_STR}
  if [[ $CLEAN_AFTER = 1 ]]; then make clean; fi
}

build_xlnt()
{
  echo ""
  echo "## Building Xlnt..."
  cd ${BUILD_DIR}
  git clone https://github.com/tfussell/xlnt.git
  cd xlnt
  git checkout -f ${XLNT_VER}
  git submodule init
  git submodule update

  # move conflicting file out of the way so it builds on osx
  mv third-party/libstudxml/version third-party/libstudxml/version.bak

  # fix rpath
  sed -i'.original' -e 's/INSTALL_NAME_DIR.*/)/' source/CMakeLists.txt

  if [[ $BUILD_CLEAN = 1 ]]; then rm -rf build; fi
  mkdir -p build && cd build

  if [[ $OSTYPE == "msys" ]]; then
      cmake -DCMAKE_CXX_FLAGS_RELEASE="$WIN_CFLAGS" -DCMAKE_C_FLAGS_RELEASE="$WIN_CFLAGS" -DCMAKE_SHARED_LINKER_FLAGS_RELEASE="$WIN_LFLAGS" -DCMAKE_EXE_LINKER_FLAGS_RELEASE="$WIN_LFLAGS" -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" -DSTATIC=OFF ..
      cmake --build . --config ${BUILD_TYPE} --parallel || exit 1
      cmake --build . --config ${BUILD_TYPE} --target install
  else
      cmake -DCMAKE_CXX_FLAGS="$FLAGS" -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DSTATIC=OFF -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCMAKE_DEBUG_POSTFIX="" ..
      make -j${NUM_PROCS} install || exit 1
  fi

  # move conflicting file back to where it was
  mv third-party/libstudxml/version.bak third-party/libstudxml/version

  XLNT_DIR=${INSTALL_DIR}
  if [[ $CLEAN_AFTER = 1 ]]; then make clean; fi
}

build_jkqtplotter()
{
  echo ""
  echo "## Building JKQTPlotter..."
  cd ${BUILD_DIR}
  git clone --depth 1 --branch ${JKQTPLOTTER_VER} https://github.com/akenmorris/JKQtPlotter.git jkqtplotter
  cd jkqtplotter
  # fix compile on windows
  sed -i '1s/^/#include <stdexcept>\n/' lib/jkqtcommon/jkqtpdebuggingtools.h

  
  if [[ $BUILD_CLEAN = 1 ]]; then rm -rf build; fi
  mkdir -p build && cd build

  if [[ $OSTYPE == "msys" ]]; then
      cmake -DCMAKE_CXX_FLAGS="-FS" -DCMAKE_C_FLAGS="-FS" -DJKQtPlotter_BUILD_EXAMPLES=OFF -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" ..
      cmake --build . --config ${BUILD_TYPE} --parallel || exit 1
      cmake --build . --config ${BUILD_TYPE} --target install
  else
      cmake -DCMAKE_CXX_FLAGS="$FLAGS" -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DJKQtPlotter_BUILD_EXAMPLES=OFF -DCMAKE_BUILD_TYPE=${BUILD_TYPE} ..
      make -j${NUM_PROCS} install || exit 1
  fi

  JKQTPLOTTER_DIR=${INSTALL_DIR}
  if [[ $CLEAN_AFTER = 1 ]]; then make clean; fi
}

build_openvdb()
{
  echo ""
  echo "## Building OpenVDB..."
  cd ${BUILD_DIR}
  git clone https://github.com/AcademySoftwareFoundation/openvdb.git
  cd openvdb
  git checkout ${OpenVDB_VER}

  if [[ $BUILD_CLEAN = 1 ]]; then rm -rf build; fi
  mkdir -p build && cd build

  CONCURRENT_FLAG="-DCONCURRENT_MALLOC=None"
      
  if [[ $OSTYPE == "msys" ]]; then
      cmake -DCMAKE_CXX_FLAGS_RELEASE="$WIN_CFLAGS" -DCMAKE_C_FLAGS_RELEASE="$WIN_CFLAGS" -DCMAKE_SHARED_LINKER_FLAGS_RELEASE="$WIN_LFLAGS" -DCMAKE_EXE_LINKER_FLAGS_RELEASE="$WIN_LFLAGS" -DCMAKE_SHARED_LINKER_FLAGS_RELEASE="$WIN_LFLAGS" -DUSE_BLOSC=OFF -DCMAKE_PREFIX_PATH=${CONDA_PREFIX} -DOPENVDB_CORE_STATIC=OFF -DUSE_EXPLICIT_INSTANTIATION=OFF -DOPENVDB_BUILD_BINARIES=OFF -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} ..
      cmake --build . --config ${BUILD_TYPE} || exit 1
      cmake --build . --config ${BUILD_TYPE} --target install
  elif [[ "$OSTYPE" == "linux"* ]]; then
      cmake -DCMAKE_CXX_FLAGS="$FLAGS" -DUSE_BLOSC=OFF ${CONCURRENT_FLAG} -DCMAKE_PREFIX_PATH=${CONDA_PREFIX} -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DCMAKE_INSTALL_LIBDIR=lib -DOPENVDB_CORE_STATIC=OFF -DUSE_EXPLICIT_INSTANTIATION=OFF -DOPENVDB_BUILD_BINARIES=OFF -DCMAKE_BUILD_TYPE=${BUILD_TYPE} ..
      make -j${NUM_PROCS} install || exit 1
  else
      cmake -DCMAKE_CXX_FLAGS="$FLAGS" -DUSE_BLOSC=OFF ${CONCURRENT_FLAG} -DCMAKE_PREFIX_PATH=${CONDA_PREFIX} -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DCMAKE_INSTALL_LIBDIR=lib -DOPENVDB_CORE_STATIC=OFF -DOPENVDB_BUILD_BINARIES=OFF -DCMAKE_BUILD_TYPE=${BUILD_TYPE} ..
      make -j${NUM_PROCS} install || exit 1
  fi

  OpenVDB_DIR=${INSTALL_DIR}/lib/cmake/OpenVDB/
  if [[ $CLEAN_AFTER = 1 ]]; then make clean; fi
}

build_igl()
{
  echo " "
  echo "## Building Libigl..."
  cd ${INSTALL_DIR}
  git clone https://github.com/libigl/libigl.git
  cd libigl
  git checkout -f tags/${libigl_VER}

  LIBIGL_DIR=${INSTALL_DIR}/libigl/cmake
}

build_geometry_central()
{
  echo " "
  echo "## Building Geometry central..."
  cd ${BUILD_DIR}
  git clone --recursive https://github.com/nmwsharp/geometry-central.git
  cd geometry-central
  git checkout -f ${geometry_central_VER}
  mkdir build
  
  if [[ $BUILD_CLEAN = 1 ]]; then rm -rf build; fi
  mkdir -p build && cd build

  if [[ $OSTYPE == "msys" ]]; then
      cmake -DCMAKE_CXX_FLAGS="-FS" -DCMAKE_C_FLAGS="-FS" -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" ..
      cmake --build . --config ${BUILD_TYPE} --parallel || exit 1
      # no make install, so we do this manually
      cd ..
      cp -a include/geometrycentral ${INSTALL_DIR}/include
      cp build/src/Release/geometry-central.lib ${INSTALL_DIR}/lib
  else
      cmake -DCMAKE_CXX_FLAGS="-fPIC $FLAGS" -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DCMAKE_BUILD_TYPE=${BUILD_TYPE} ..
      make -j${NUM_PROCS}
      # no make install, so we do this manually
      cd ..
      cp -a include/geometrycentral ${INSTALL_DIR}/include
      cp build/src/*geom* ${INSTALL_DIR}/lib
  fi

  
  GEOMETRY_CENTRAL_DIR=${INSTALL_DIR}/geometry-central
}

build_acvd()
{
  echo ""
  echo "## Building ACVD..."
  cd ${BUILD_DIR}
  git clone https://github.com/valette/ACVD.git acvd
  cd acvd
  git checkout -f ${ACVD_VER}

  # silence a bunch of output
  sed -i'.original' -e 's/cout/if(0)cout/' Common/vtkCurvatureMeasure.cxx
  sed -i'.original' -e 's/cout/if(0)cout/' DiscreteRemeshing/vtkDiscreteRemeshing.h
  sed -i'.original' -e 's/cout/if(0)cout/' DiscreteRemeshing/vtkQEMetricForClustering.h
  sed -i'.original' -e 's/cout/if(0)cout/' Common/vtkUniformClustering.h

  if [[ $BUILD_CLEAN = 1 ]]; then rm -rf build; fi
  mkdir -p build && cd build

  if [[ $OSTYPE == "msys" ]]; then
      cmake -DCMAKE_CXX_FLAGS="-FS" -DCMAKE_C_FLAGS="-FS" -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" -DVTK_DIR="${VTK_DIR}" -DBUILD_EXAMPLES:BOOL=OFF -DBUILD_SHARED_LIBS:BOOL=OFF ..
      cmake --build . --config ${BUILD_TYPE} --parallel || exit 1
      cmake --build . --config ${BUILD_TYPE} --target install
  else
      cmake -DCMAKE_CXX_FLAGS="$FLAGS" -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DVTK_DIR="${VTK_DIR}" -DBUILD_EXAMPLES:BOOL=OFF -DBUILD_SHARED_LIBS:BOOL=OFF ..
      make -j${NUM_PROCS} install || exit 1
  fi

  ACVD_DIR=${INSTALL_DIR}
  if [[ $CLEAN_AFTER = 1 ]]; then make clean; fi
}

show_shapeworks_build()
{
  echo ""
  echo "## To configure ShapeWorks, create a build directory and from there run..."
  echo ""

  OPENMP_FLAG="-DUSE_OPENMP=ON"
  if [ "$(uname)" == "Darwin" ]; then
    OPENMP_FLAG="-DUSE_OPENMP=OFF"
  fi

  FINDQT=""
  if [[ ${BUILD_GUI} -eq 1 ]]; then
    FINDQT="-DQt5_DIR=`qmake -query QT_INSTALL_PREFIX`"
  fi

  echo "cmake -DCMAKE_CXX_FLAGS="$FLAGS" -DCMAKE_PREFIX_PATH=${INSTALL_DIR} ${OPENMP_FLAG} -DBuild_Studio=${BUILD_GUI} ${FIND_QT} -Wno-dev -Wno-deprecated -DCMAKE_BUILD_TYPE=${BUILD_TYPE} ${SRC}"
}

# determine if we can build using the specified or discovered version of Qt
verify_qt()
{
  # If BUILD_GUI is true, verify that a sufficient version of Qt is installed using the qmake that's in the path.
  # If not, inform the user of where to go to download and install Qt.

  if [[ $BUILD_GUI = 1 ]]; then
    if [[ `which qmake` ]]; then
      QMAKE=`which qmake`
    fi

    # verify version
    if [[ -f ${QMAKE} ]]; then
      QT_VERSION=`${QMAKE} -v | grep "Using Qt version" | cut -d" " -f4`
      at_least_required_version "Qt" "${QT_VERSION}" "${QT_MIN_VER}"
      QT_VERSION_OK=$?
    else
      QT_VERSION_OK=0
    fi

    # inform user
    if [[ $QT_VERSION_OK = 0 ]]; then
	echo "For GUI applications, please make sure the ShapeWorks conda env bin dir is at the head of the path." >&2
      exit 1
    fi
  fi
}

build_all()
{
  ## create build and install directories
  if [[ -z $BUILD_DIR ]];   then BUILD_DIR=${SRC}/dependencies/build;     fi
  if [[ -z $INSTALL_DIR ]]; then INSTALL_DIR=${SRC}/dependencies/install; fi
  mkdir -p ${BUILD_DIR}
  mkdir -p ${INSTALL_DIR}

  ## build dependencies if their locations were not specified
  if [[ -z $OpenVDB_DIR ]]; then
    build_openvdb
  fi

  if [[ -z $VTK_DIR ]]; then
    build_vtk
  fi

  if [[ -z $ITK_DIR ]]; then
    build_itk
  fi

  if [[ -z $XLNT_DIR ]]; then
    build_xlnt
  fi

  if [[ -z $LIBIGL_DIR ]]; then
    build_igl
  fi

  if [[ -z $GEOMETRY_CENTRAL_DIR ]]; then
    build_geometry_central
  fi

  if [[ -z $JKQTPLOTTER_DIR ]]; then
    if [[ $BUILD_GUI = 1 ]]; then
      build_jkqtplotter
    fi
  fi

  if [[ -z $ACVD_DIR ]]; then
    build_acvd
  fi
  
  # echo dependency directories for easy reference in case the user is independently building ShapeWorks
  echo ""
  echo "Dependency paths:"
  echo "  VTK_DIR: ${VTK_DIR}"
  echo "  ITK_DIR: ${ITK_DIR}"
  echo "  EIGEN_DIR: ${EIGEN_DIR}"
  echo "  OpenVDB_DIR: ${OpenVDB_DIR}"
  echo "  LIBIGL_DIR: ${LIBIGL_DIR}"
  echo "  GEOMETRY_CENTRAL_DIR: ${GEOMETRY_CENTRAL_DIR}"
  echo "  ACVD_DIR: ${ACVD_DIR}"
  echo ""
  
  show_shapeworks_build
}

#
# main
#

SRC=`pwd`
parse_command_line $*

echo "##-------------------------------"
echo "## ShapeWorks Build Dependencies "
echo "##-------------------------------"
echo "##"
echo "## called using these arguments: "
echo "##  $*"
echo "##"
echo ""

if [[ $OSTYPE != "msys" ]]; then
    verify_qt
fi

echo "INSTALL_DIR: ${INSTALL_DIR}"
echo "BUILD_DIR: ${BUILD_DIR}"
echo "NUM_PROCS: ${NUM_PROCS}"
echo "BUILD_GUI: ${BUILD_GUI}"
echo "BUILD_CLEAN: ${BUILD_CLEAN}"
echo "BUILD_TYPE: ${BUILD_TYPE}"

#build dependencies
(time build_all 2>&1) 2>&1 | tee ${BUILD_LOG}
RC=( "${PIPESTATUS[@]}" )
exit ${RC[0]}
