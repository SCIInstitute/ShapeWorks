#!/bin/bash

#set -v   # verbose execution for debugging
#set -x   # tracing execution for debugging (echos all commands from script)

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
BUILD_STUDIO=0
BUILD_SHAPEWORKS=1
BUILD_TYPE="Release"
BUILD_LOG="build_dependencies.log"
VTK_VER="v9.5.0"
VTK_VER_STR="9.5"
ITK_VER="v5.4.4"
ITK_VER_STR="5.4"
QT_MIN_VER="5.15.4"
XLNT_VER="538f80794c7d736afc0a452d21313606cc5538fc"
JKQTPLOTTER_VER="v2022.11.30-refix-rpath"
OpenVDB_VER="v9.1.0"
libigl_VER="v2.3.0"
geometry_central_VER="8b20898f6c7be1eab827a9f720c8fd45e58ae63c" # This library isn't using tagged versions
ACVD_VER="3cf3643851cdf261abbcb9a74e4df01d59154e23" # This library isn't using tagged version

#WIN_CFLAGS="-FS /Zi /GL /MD /O2 /Ob3 /Zm250 /DNDEBUG /EHsc"  # windows release compilation flags
WIN_CFLAGS="-FS /Zi /MD /O2 /Ob3 /Zm250 /DNDEBUG /EHsc"  # windows release compilation flags
#WIN_LFLAGS="-LTCG /DEBUG" # windows release compilation flags
WIN_LFLAGS="-DEBUG" # windows release compilation flags
#FLAGS="-g" # turn on symbols for all builds
FLAGS=""

# Normalize Windows OSTYPE flavors. The GHA windows-2022 runner image now
# reports "cygwin" (was "msys" before the recent bump). Both are git-bash-
# like environments and we want every Windows check below to fire for any
# of them. Without this, the script falls through to the macOS branch and
# adds clang-only flags that MSVC then rejects with D8021.
echo "OSTYPE=[$OSTYPE]"
case "$OSTYPE" in
  cygwin*) OSTYPE="msys" ;;
esac
echo "OSTYPE (normalized)=[$OSTYPE]"

# Defensive: clear compiler-flag env vars on Windows. CMake reads CXXFLAGS
# into CMAKE_CXX_FLAGS, and cl.exe also prepends $CL / appends $_CL_ to
# every invocation. Cheap insurance against future env leaks.
if [[ $OSTYPE == msys* ]]; then
  unset CXXFLAGS CFLAGS CL _CL_
fi

usage()
{
  echo "usage: ./build_dependencies.sh [[-n=<num-procs>] [-i=<install_path>] [-b=<build_path>] [--clean] | [-h | --help]]"
  echo ""
  echo "If using Anaconda to install prerequisites please first run:"
  echo "source ./install_shapeworks.sh"
  echo ""
  echo "Please make sure the ShapeWorks conda environment is activated."
  echo ""
  echo "Arguments:"
  echo "  -h,--help               : Show this screen."
  echo "  --clean                 : Remove all build directories and clone implicit dependencies."
  echo "                          : (note that user-specified paths such as --itk-dir=<path> will not be deleted)."
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

# Retry a git clone up to 5 times to ride out transient GHA-runner DNS / TLS
# hiccups (we've hit "Could not resolve host: github.com" on the mac and
# mac-arm64 runners). Exits the script if every attempt fails so the build
# stops at the clone instead of stumbling forward into a missing source tree.
git_clone_retry()
{
  local attempt
  for attempt in 1 2 3 4 5; do
    if git clone "$@"; then
      return 0
    fi
    echo "## git clone attempt ${attempt}/5 failed; retrying in 10s..."
    sleep 10
  done
  echo "ERROR: git clone failed after 5 attempts: $*"
  exit 1
}

build_vtk()
{
  echo ""
  echo "## Building vtk..."
  cd ${BUILD_DIR}
  git_clone_retry https://gitlab.kitware.com/vtk/vtk.git
  cd vtk
  git checkout -f tags/${VTK_VER}
  patch -p1 < ${SCRIPT_DIR}/Support/vtk-9.5.patch
  if [[ $BUILD_CLEAN = 1 ]]; then rm -rf build; fi
  mkdir -p build && cd build
  if [[ $OSTYPE == msys* ]]; then
      cmake -DCMAKE_CXX_FLAGS="" -DCMAKE_C_FLAGS="" -DCMAKE_CXX_FLAGS_RELEASE="$WIN_CFLAGS" -DCMAKE_C_FLAGS_RELEASE="$WIN_CFLAGS" -DCMAKE_SHARED_LINKER_FLAGS_RELEASE="$WIN_LFLAGS" -DCMAKE_EXE_LINKER_FLAGS_RELEASE="$WIN_LFLAGS" -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" -DBUILD_SHARED_LIBS:BOOL=ON -DVTK_WRAP_PYTHON:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DVTK_Group_Qt:BOOL=ON -DVTK_QT_VERSION=5 -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DVTK_PYTHON_VERSION=3 -DVTK_GROUP_ENABLE_Qt=YES -DVTK_MODULE_ENABLE_VTK_GUISupportQtQuick:STRING=DONT_WANT -DBUILD_EXAMPLES:BOOL=OFF -DVTK_SMP_ENABLE_TBB=ON -DVTK_SMP_IMPLEMENTATION_TYPE=TBB -Wno-dev ..
      cmake --build . --config ${BUILD_TYPE} --parallel || exit 1
      cmake --build . --config ${BUILD_TYPE} --target install
      VTK_DIR="${INSTALL_DIR}/lib/cmake/vtk-${VTK_VER_STR}"
      VTK_DIR=$(echo $VTK_DIR | sed s/\\\\/\\//g)
  else
      # Fix for newer libtbb requiring updated libstdc++ from conda environment
      if [[ -n "${CONDA_PREFIX}" ]]; then
          export LD_LIBRARY_PATH="${CONDA_PREFIX}/lib:${LD_LIBRARY_PATH}"
          CONDA_LINK_FLAGS="-L${CONDA_PREFIX}/lib -Wl,-rpath,${CONDA_PREFIX}/lib"
      else
          CONDA_LINK_FLAGS=""
      fi
      
      cmake -DCMAKE_CXX_FLAGS="$FLAGS -Wno-incompatible-function-pointer-types" -DCMAKE_C_FLAGS="$FLAGS -Wno-incompatible-function-pointer-types" -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DBUILD_SHARED_LIBS:BOOL=ON -DVTK_WRAP_PYTHON:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DVTK_Group_Qt:BOOL=ON -DVTK_QT_VERSION=5 -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DVTK_PYTHON_VERSION=3 -DVTK_GROUP_ENABLE_Qt=YES -DVTK_MODULE_ENABLE_VTK_GUISupportQtQuick:STRING=DONT_WANT -DBUILD_EXAMPLES:BOOL=OFF ${VTK_EXTRA_OPTIONS} -DVTK_SMP_ENABLE_TBB=ON -DVTK_SMP_IMPLEMENTATION_TYPE=TBB -DCMAKE_EXE_LINKER_FLAGS="${CONDA_LINK_FLAGS}" -DCMAKE_SHARED_LINKER_FLAGS="${CONDA_LINK_FLAGS}" -DCMAKE_MODULE_LINKER_FLAGS="${CONDA_LINK_FLAGS}" -Wno-dev ..
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
  git_clone_retry https://github.com/InsightSoftwareConsortium/ITK.git
  cd ITK
  git checkout -f tags/${ITK_VER}

  # replace <fp.h> with <math.h> in Modules/ThirdParty/PNG/src/itkpng/pngpriv.h
  if [ "$(uname)" == "Darwin" ]; then
    # macOS requires an extension for -i
    sed -i '' 's|<fp.h>|<math.h>|g' Modules/ThirdParty/PNG/src/itkpng/pngpriv.h
  else
    # Linux/GitHub Actions doesn't need an extension
    sed -i 's|<fp.h>|<math.h>|g' Modules/ThirdParty/PNG/src/itkpng/pngpriv.h
  fi

  # Always emit the _ITKCommonPython._C_API capsule, even with shared libs, so
  # the pip itk-elastix wheel (SABI .abi3.so) can load against our shared
  # libITKCommon. See Support/itk-5.4-shared-cAPI.patch for the full story.
  patch -p1 < ${SCRIPT_DIR}/Support/itk-5.4-shared-cAPI.patch

  # Keep ITK_LEGACY_REMOVE=OFF settable when ITK_WRAPPING=ON. Upstream forces
  # LEGACY_REMOVE on when wrapping is on; ShapeWorks' C++ uses legacy APIs
  # (Region.h Point arithmetic, itk::SpatialOrientation::ITK_COORDINATE_*)
  # that disappear under LEGACY_REMOVE. See the patch for details.
  patch -p1 < ${SCRIPT_DIR}/Support/itk-5.4-legacy-remove.patch

  # Fix an upstream bug in the LEGACY-only Approximate3DNormals() wrapper that
  # fails to compile once we keep LEGACY_REMOVE off while wrapping is on
  # (the LEGACY+WRAPPING combination upstream's cmake_dependent_option normally
  # prevents and therefore never tested).
  patch -p1 < ${SCRIPT_DIR}/Support/itk-5.4-legacy-computenormals.patch

  # Honor -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=OFF in the Python wrap step.
  # Upstream itk_end_wrap_module.cmake forces per-target IPO regardless of the
  # cache var; gcc-toolset-13's lto1 ICEs trying to link the wrap modules.
  patch -p1 < ${SCRIPT_DIR}/Support/itk-5.4-respect-ipo-off.patch

  # Download a newer castxml than ITK 5.4 pins (0.6.5). The bundled 0.6.5
  # collides with glibc's _Float64/_Float32x/_Float64x typedefs whenever it
  # simulates GCC >=13 (CastXML#251 / CastXML#272 fixed in 0.6.9 / 0.6.10).
  # The CI Linux container is manylinux_2_28_x86_64 (AlmaLinux 8) with
  # gcc-toolset-13, so castxml-0.6.5 fails to parse <bits/floatn-common.h>
  # with "typedef redefinition with different types". We feed ITK a
  # 2026.01.30 binary via CASTXML_EXECUTABLE and let it skip the download.
  # ITK 6.0b02 already uses this castxml; we backport for the 5.4.4 deps.
  CASTXML_VER="2026.01.30"
  CASTXML_DIR="${BUILD_DIR}/castxml"
  CASTXML_EXECUTABLE=""
  if [[ $OSTYPE == msys* || $OSTYPE == cygwin* ]]; then
    # win11-amd64 is MSVC-built (depends on VCRUNTIME140.dll which the GHA
    # windows-2022 runner already has via the bundled Visual C++ Redist).
    # The windows-2022-amd64 asset is MinGW-built and the zip ships only
    # castxml.exe without the libgcc_s_seh-1 / libstdc++-6 / libwinpthread-1
    # DLLs it links against — every castxml invocation dies with Windows
    # exit code 0xC0000139 (STATUS_ENTRYPOINT_NOT_FOUND) on the runner.
    castxml_asset="castxml-windows-win11-amd64.zip"
    castxml_exe_suffix=".exe"
  elif [[ "$(uname)" == "Darwin" ]]; then
    if [[ "$(uname -m)" == "arm64" ]]; then
      castxml_asset="castxml-macos-15-arm64.tar.gz"
    else
      castxml_asset="castxml-macos-15-intel-x86_64.tar.gz"
    fi
    castxml_exe_suffix=""
  else
    if [[ "$(uname -m)" == "aarch64" ]]; then
      castxml_asset="castxml-alma-linux-8-arm-aarch64.tar.gz"
    else
      castxml_asset="castxml-alma-linux-8-x86_64.tar.gz"
    fi
    castxml_exe_suffix=""
  fi
  if [[ ! -x "${CASTXML_DIR}/bin/castxml${castxml_exe_suffix}" ]]; then
    mkdir -p "${CASTXML_DIR}"
    pushd "${CASTXML_DIR}" >/dev/null
    # --retry covers GitHub's transient 5xx CDN hiccups (HTTP 5xx is in
    # curl's default retry set); || exit 1 stops the script instead of
    # silently continuing without a castxml binary and surfacing the failure
    # 200 log lines later in the ITK wrap step. (Avoid --retry-all-errors:
    # that's curl 7.71+ and the manylinux_2_28 container ships an older curl.)
    curl -fsSL --retry 5 --retry-delay 10 \
      -o "${castxml_asset}" \
      "https://github.com/CastXML/CastXMLSuperbuild/releases/download/v${CASTXML_VER}/${castxml_asset}" \
      || { echo "ERROR: castxml download failed"; exit 1; }
    if [[ "${castxml_asset}" == *.zip ]]; then
      unzip -q "${castxml_asset}" || exit 1
    else
      tar xzf "${castxml_asset}" || exit 1
    fi
    rm "${castxml_asset}"
    # All assets extract to a top-level `castxml/` dir — flatten one level so
    # the binary lands at ${CASTXML_DIR}/bin/castxml.
    if [[ -d "${CASTXML_DIR}/castxml" ]]; then
      mv "${CASTXML_DIR}/castxml"/* "${CASTXML_DIR}/"
      rmdir "${CASTXML_DIR}/castxml"
    fi
    popd >/dev/null
  fi
  CASTXML_EXECUTABLE="${CASTXML_DIR}/bin/castxml${castxml_exe_suffix}"
  echo "## using castxml at ${CASTXML_EXECUTABLE}"

  # The macOS castxml binary is built on a Homebrew Mac and links against
  # /opt/homebrew/opt/zstd/lib/libzstd.1.dylib. On non-Homebrew Macs
  # (MacPorts, conda-only dev envs) that path doesn't exist and the next
  # make step dies with `dyld: Library not loaded: ...libzstd.1.dylib`.
  # Retarget at whichever libzstd we have (conda env first, MacPorts second)
  # and re-sign ad-hoc so the kernel will still exec the patched binary.
  # No-op when castxml already runs (Homebrew is installed).
  if [[ "$(uname)" == "Darwin" ]] && [[ -x "${CASTXML_EXECUTABLE}" ]] \
       && ! "${CASTXML_EXECUTABLE}" --version >/dev/null 2>&1; then
    for cand in "${CONDA_PREFIX:-}/lib/libzstd.1.dylib" /opt/local/lib/libzstd.1.dylib; do
      if [[ -f "${cand}" ]]; then
        install_name_tool -change /opt/homebrew/opt/zstd/lib/libzstd.1.dylib "${cand}" "${CASTXML_EXECUTABLE}"
        codesign --force --sign - "${CASTXML_EXECUTABLE}"
        echo "## Patched castxml libzstd reference -> ${cand}"
        break
      fi
    done
  fi

  if [[ $BUILD_CLEAN = 1 ]]; then rm -rf build; fi
  mkdir -p build && cd build

  # ITK is built shared + Python-wrapped so the bundled Python can reuse the same
  # C++ ITK that ShapeWorks links — replacing the ~1.5 GB of pip itk-core /
  # itk-numerics / itk-io / itk-filtering / itk-registration / itk-segmentation
  # wheels with one shared install.
  #
  # itk-elastix stays as a pip wheel (Module_Elastix is a remote module not
  # shipped by ITK upstream, so vendoring it would add a maintenance pin and
  # extra build time for a ~90 MB delta). Its statically-linked .abi3.so files
  # don't expose ITK symbols, so they coexist with our shared libitk* in one
  # process without the loader-resolution problem the VTK pip wheel had.
  #
  # ITK_WRAP_DIMS=2;3 + a minimal scalar-type set keeps the wrap step from
  # blowing up — the upstream itk-* wheels wrap 11 types x 5 dims and take
  # hours. Expand if downstream code asks for a missing type.
  ITK_WRAP_FLAGS=(
    -DITK_WRAP_PYTHON:BOOL=ON
    # Dim 4 is needed by pip itk-elastix: its itkTransformixFilter and
    # itkElastixRegistrationMethod classes reference itkImageSource{D,F,SS,UC,US}4
    # and importing them fails with AttributeError if dim 4 isn't wrapped.
    -DITK_WRAP_DIMS:STRING="2;3;4"
    -DITK_WRAP_unsigned_char:BOOL=ON
    -DITK_WRAP_unsigned_short:BOOL=ON
    -DITK_WRAP_signed_short:BOOL=ON
    -DITK_WRAP_float:BOOL=ON
    -DITK_WRAP_double:BOOL=ON
    # SABI defaults ON for Python>=3.11 — required so our dep itk-core .abi3.so
    # exposes the `_C_API` capsule that the pip itk-elastix wheel imports. Pip
    # elastix is also SABI; without SABI parity, loading elastix against our
    # core raises AttributeError: '_C_API' on first call. (Needs CMake>=3.26
    # for Development.SABIModule, hence the pip cmake==3.31.6 install in
    # install_shapeworks.sh.)
    # ITK's Python wrap install path defaults to sysconfig's `platlib` for the
    # build-time Python — that's conda's site-packages. We want the wrap to
    # land alongside the dep install so cmake/provision_bundled_itk.py can
    # find it via ${ITK_DIR}/../../python3.12/site-packages.
    -DPY_SITE_PACKAGES_PATH=${INSTALL_DIR}/lib/python3.12/site-packages
    # Disable LTO/IPO. The CI Linux container's gcc-toolset-13 lto1 ICEs
    # ("internal compiler error: in get_token, at opts-common.cc:2123") while
    # linking _ITKPyBasePython.abi3.so. We don't need IPO on a dep build, and
    # disabling it sidesteps the toolset-13 LTO bug without changing toolchain.
    -DCMAKE_INTERPROCEDURAL_OPTIMIZATION:BOOL=OFF
  )

  if [[ $OSTYPE == msys* ]]; then
      cmake -DCMAKE_CXX_FLAGS="" -DCMAKE_C_FLAGS="" -DCMAKE_CXX_FLAGS_RELEASE="$WIN_CFLAGS" -DCMAKE_C_FLAGS_RELEASE="$WIN_CFLAGS" -DCMAKE_SHARED_LINKER_FLAGS_RELEASE="$WIN_LFLAGS" -DCMAKE_EXE_LINKER_FLAGS_RELEASE="$WIN_LFLAGS" -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DBUILD_EXAMPLES:BOOL=OFF -DVTK_DIR="${VTK_DIR}" -DITK_USE_SYSTEM_EIGEN=on -DEigen3_DIR=${EIGEN_DIR} -DModule_ITKVtkGlue:BOOL=ON -DModule_ITKDeprecated:BOOL=ON -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DModule_ITKIOHDF5=OFF -DITK_USE_SYSTEM_HDF5=ON -DZLIB_SYMBOL_PREFIX="itkzlib_" -DITK_USE_SYSTEM_CASTXML:BOOL=ON -DCASTXML_EXECUTABLE="${CASTXML_EXECUTABLE}" "${ITK_WRAP_FLAGS[@]}" -Wno-dev ..

      cmake --build . --config ${BUILD_TYPE} --parallel || exit 1
      cmake --build . --config ${BUILD_TYPE} --target install
  else
      # Same conda-libstdc++ trick as build_vtk: the linker has to pick the
      # conda env's libstdc++, not the system one, or shared libitk* loads a
      # different libstdc++ than the ShapeWorks binaries (compiled by conda's
      # gcc) and RTTI lookups crash when objects cross the boundary
      # (typeinfo for std::__future_base::_Result<void*> SIGSEGVs in
      # itk::ImageSource::GenerateData).
      if [[ -n "${CONDA_PREFIX}" ]]; then
          export LD_LIBRARY_PATH="${CONDA_PREFIX}/lib:${LD_LIBRARY_PATH}"
          CONDA_LINK_FLAGS="-L${CONDA_PREFIX}/lib -Wl,-rpath,${CONDA_PREFIX}/lib"
      else
          CONDA_LINK_FLAGS=""
      fi

      cmake -DCMAKE_CXX_FLAGS="$FLAGS" -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DBUILD_EXAMPLES:BOOL=OFF -DModule_ITKVtkGlue:BOOL=ON -DModule_ITKDeprecated:BOOL=ON -DITK_USE_SYSTEM_EIGEN=on -DEigen3_DIR=${EIGEN_DIR} -DVTK_DIR="${VTK_DIR}" -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCMAKE_EXE_LINKER_FLAGS="${CONDA_LINK_FLAGS}" -DCMAKE_SHARED_LINKER_FLAGS="${CONDA_LINK_FLAGS}" -DCMAKE_MODULE_LINKER_FLAGS="${CONDA_LINK_FLAGS}" -DITK_USE_SYSTEM_CASTXML:BOOL=ON -DCASTXML_EXECUTABLE="${CASTXML_EXECUTABLE}" "${ITK_WRAP_FLAGS[@]}" -Wno-dev ..
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
  git_clone_retry https://github.com/tfussell/xlnt.git
  cd xlnt
  git checkout -f ${XLNT_VER}
  git submodule init
  git submodule update

  # move conflicting file out of the way so it builds on osx
  mv third-party/libstudxml/version third-party/libstudxml/version.bak

  # Fix for GCC 13
  echo '#include <cstdint>' >> include/xlnt/xlnt_config.hpp

  # fix rpath
  sed -i'.original' -e 's/INSTALL_NAME_DIR.*/)/' source/CMakeLists.txt

  if [[ $BUILD_CLEAN = 1 ]]; then rm -rf build; fi
  mkdir -p build && cd build

  if [[ $OSTYPE == msys* ]]; then
      cmake -DCMAKE_CXX_FLAGS="" -DCMAKE_C_FLAGS="" -DCMAKE_CXX_FLAGS_RELEASE="$WIN_CFLAGS" -DCMAKE_C_FLAGS_RELEASE="$WIN_CFLAGS" -DCMAKE_SHARED_LINKER_FLAGS_RELEASE="$WIN_LFLAGS" -DCMAKE_EXE_LINKER_FLAGS_RELEASE="$WIN_LFLAGS" -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" -DSTATIC=OFF ..
      cmake --build . --config ${BUILD_TYPE} --parallel || exit 1
      cmake --build . --config ${BUILD_TYPE} --target install
  else
      cmake -DCMAKE_CXX_FLAGS="$FLAGS" -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DSTATIC=OFF -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCMAKE_DEBUG_POSTFIX="" -DCMAKE_POLICY_VERSION_MINIMUM=3.5 ..
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
  git_clone_retry --depth 1 --branch ${JKQTPLOTTER_VER} https://github.com/akenmorris/JKQtPlotter.git jkqtplotter
  cd jkqtplotter
  # fix compile on windows
  sed -i '1s/^/#include <stdexcept>\n/' lib/jkqtcommon/jkqtpdebuggingtools.h

  
  if [[ $BUILD_CLEAN = 1 ]]; then rm -rf build; fi
  mkdir -p build && cd build

  if [[ $OSTYPE == msys* ]]; then
      cmake -DCMAKE_CXX_FLAGS="-FS" -DCMAKE_C_FLAGS="-FS" -DJKQtPlotter_BUILD_EXAMPLES=OFF -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" ..
      cmake --build . --config ${BUILD_TYPE} --parallel || exit 1
      cmake --build . --config ${BUILD_TYPE} --target install
  else
      cmake -DCMAKE_CXX_FLAGS="$FLAGS" -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DJKQtPlotter_BUILD_EXAMPLES=OFF -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCMAKE_POLICY_VERSION_MINIMUM=3.5 ..
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
  git_clone_retry https://github.com/AcademySoftwareFoundation/openvdb.git
  cd openvdb
  git checkout ${OpenVDB_VER}

  # patch for Clang 17
  patch -p1 < ${SCRIPT_DIR}/Support/openvdb-9.1.0.patch
  
  if [[ $BUILD_CLEAN = 1 ]]; then rm -rf build; fi
  mkdir -p build && cd build

  CONCURRENT_FLAG="-DCONCURRENT_MALLOC=None"
      
  if [[ $OSTYPE == msys* ]]; then
      cmake -DCMAKE_CXX_FLAGS="" -DCMAKE_C_FLAGS="" -DCMAKE_CXX_FLAGS_RELEASE="$WIN_CFLAGS" -DCMAKE_C_FLAGS_RELEASE="$WIN_CFLAGS" -DCMAKE_SHARED_LINKER_FLAGS_RELEASE="$WIN_LFLAGS" -DCMAKE_EXE_LINKER_FLAGS_RELEASE="$WIN_LFLAGS" -DCMAKE_SHARED_LINKER_FLAGS_RELEASE="$WIN_LFLAGS" -DUSE_BLOSC=OFF -DCMAKE_PREFIX_PATH=${CONDA_PREFIX} -DOPENVDB_CORE_STATIC=OFF -DUSE_EXPLICIT_INSTANTIATION=OFF -DOPENVDB_BUILD_BINARIES=OFF -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} ..
      cmake --build . --config ${BUILD_TYPE} || exit 1
      cmake --build . --config ${BUILD_TYPE} --target install
  elif [[ "$OSTYPE" == "linux"* ]]; then
      cmake -DCMAKE_CXX_FLAGS="$FLAGS" -DUSE_BLOSC=OFF ${CONCURRENT_FLAG} -DCMAKE_PREFIX_PATH=${CONDA_PREFIX} -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DCMAKE_INSTALL_LIBDIR=lib -DOPENVDB_CORE_STATIC=OFF -DUSE_EXPLICIT_INSTANTIATION=OFF -DOPENVDB_BUILD_BINARIES=OFF -DCMAKE_BUILD_TYPE=${BUILD_TYPE} ..
      make -j${NUM_PROCS} install || exit 1
  else
      cmake -DCMAKE_CXX_FLAGS="$FLAGS -Wno-enum-constexpr-conversion" -DUSE_BLOSC=OFF ${CONCURRENT_FLAG} -DCMAKE_PREFIX_PATH=${CONDA_PREFIX} -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DCMAKE_INSTALL_LIBDIR=lib -DOPENVDB_CORE_STATIC=OFF -DOPENVDB_BUILD_BINARIES=OFF -DCMAKE_BUILD_TYPE=${BUILD_TYPE} ..
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
  git_clone_retry https://github.com/libigl/libigl.git
  cd libigl
  git checkout -f tags/${libigl_VER}

  LIBIGL_DIR=${INSTALL_DIR}/libigl/cmake
}

build_geometry_central()
{
  echo " "
  echo "## Building Geometry central..."
  cd ${BUILD_DIR}
  git_clone_retry --recursive https://github.com/nmwsharp/geometry-central.git
  cd geometry-central
  git checkout -f ${geometry_central_VER}
  mkdir build
  
  if [[ $BUILD_CLEAN = 1 ]]; then rm -rf build; fi
  mkdir -p build && cd build

  if [[ $OSTYPE == msys* ]]; then
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
  #git clone https://github.com/valette/ACVD.git acvd
  git_clone_retry https://github.com/akenmorris/ACVD.git acvd
  cd acvd
  git checkout -f ${ACVD_VER}

  # silence a bunch of output
  sed -i'.original' -e 's/cout/if(0)cout/' Common/vtkCurvatureMeasure.cxx
  sed -i'.original' -e 's/cout/if(0)cout/' DiscreteRemeshing/vtkDiscreteRemeshing.h
  sed -i'.original' -e 's/cout/if(0)cout/' DiscreteRemeshing/vtkQEMetricForClustering.h
  sed -i'.original' -e 's/cout/if(0)cout/' Common/vtkUniformClustering.h

  if [[ $BUILD_CLEAN = 1 ]]; then rm -rf build; fi
  mkdir -p build && cd build

  if [[ $OSTYPE == msys* ]]; then
      cmake -DCMAKE_CXX_FLAGS="-FS" -DCMAKE_C_FLAGS="-FS" -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" -DVTK_DIR="${VTK_DIR}" -DBUILD_EXAMPLES:BOOL=OFF -DBUILD_SHARED_LIBS:BOOL=OFF ..
      cmake --build . --config ${BUILD_TYPE} --parallel || exit 1
      cmake --build . --config ${BUILD_TYPE} --target install
  else
      cmake -DCMAKE_CXX_FLAGS="$FLAGS" -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DVTK_DIR="${VTK_DIR}" -DBUILD_EXAMPLES:BOOL=OFF -DBUILD_SHARED_LIBS:BOOL=OFF -DCMAKE_POLICY_VERSION_MINIMUM=3.5 ..
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

  FINDQT="-DQt5_DIR=`qmake -query QT_INSTALL_PREFIX`"

  echo "cmake -DCMAKE_CXX_FLAGS="$FLAGS" -DCMAKE_PREFIX_PATH=${INSTALL_DIR} ${OPENMP_FLAG} ${FIND_QT} -Wno-dev -Wno-deprecated -DCMAKE_BUILD_TYPE=${BUILD_TYPE} ${SRC}"
}

# determine if we can build using the specified or discovered version of Qt
verify_qt()
{
  # Verify that a sufficient version of Qt is installed using the qmake that's in the path.

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
    echo "Please make sure the ShapeWorks conda environment bin dir is at the head of the path." >&2
    exit 1
  fi
}

provision_conda_vtk()
{
  # Delegate to Support/dedup_conda_vtk.sh — kept as a separate script so CI
  # can call it unconditionally after pip install (this function only runs on
  # cache-miss builds; the conda env state isn't part of the cached deps).
  INSTALL_DIR="${INSTALL_DIR}" bash "${SRC}/Support/dedup_conda_vtk.sh"
}

# Note: no conda-env ITK dedup. The pip itk-* family is statically linked, so
# it can coexist with our shared libitk* in one process without the
# loader-resolution problem that forced the VTK dedup. The bundled-Python
# tree gets the dep ITK via cmake/provision_bundled_itk.py instead.

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
    build_jkqtplotter
  fi

  if [[ -z $ACVD_DIR ]]; then
    build_acvd
  fi

  # Point the active conda env's Python at the VTK we just built (dedup; avoids a
  # second VTK being loaded into ShapeWorks' embedded Python interpreter).
  provision_conda_vtk

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

if [[ $OSTYPE != msys* ]]; then
    verify_qt
fi

echo "INSTALL_DIR: ${INSTALL_DIR}"
echo "BUILD_DIR: ${BUILD_DIR}"
echo "NUM_PROCS: ${NUM_PROCS}"
echo "BUILD_CLEAN: ${BUILD_CLEAN}"
echo "BUILD_TYPE: ${BUILD_TYPE}"

#build dependencies
(time build_all 2>&1) 2>&1 | tee ${BUILD_LOG}
RC=( "${PIPESTATUS[@]}" )
exit ${RC[0]}
