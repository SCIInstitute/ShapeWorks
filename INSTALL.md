# ShapeWorks build and installation instructions

## Installation

_coming soon:_ instructions for user installation without needing to build ShapeWorks

## Build

### OSX version < 10.14

TODO: For now, please try the instructions for [Mojave](#build-mojave)

<a id="user-content-build-mojave"></a>
### OSX 10.14 (Mojave)

Notes for building consolidated ShapeWorks for OSX 10.14.

#### Install miniconda3 for a sandboxed environment.

This first step is optional but recommended. Anaconda enables simultaneous builds that require different versions of dependencies (e.g., ViSUS, ShapeWorks, and VisIt).

1. Download Miniconda from https://docs.conda.io/en/latest/miniconda.html
2. Install Miniconda
 - `bash ./Miniconda3-latest-MacOSX-x86_64.sh`  
 - accept license agreement and default installation location (or change it).
 - Maybe `conda config --set auto_activate_base false` if you don't want conda available at startup.
3. Update to latest version and install helper utilities
 - `conda update -n base -c defaults conda`
 - `conda install -c conda-forge ctags` (the version included with osx isn't as good)

#### Two builds are described next, but only the first yet works:
- [Build with the existing dependencies](#build-mojave-existing)  
- [Build with updated dependencies (Python, VXL, VTK, ITK, and QT)](#build-mojave-updated)

<a id="user-content-build-mojave-existing"></a>
#### (A) Build with existing (very old) dependencies

Starting with an older version of Python (3.5), these instructions are based on how the dependencies were acquired and built by the `superbuild-mac.sh` script, with necessary modifications for compatibility with Mojave.

1. **Create base environment**
 ```
 export PYVER="3.5"
 conda create --yes --name shapeworks-olddeps-foundation python=$PYVER
 conda activate shapeworks-olddeps-foundation
 conda install -c anaconda -c conda-forge cmake openmp ccache
 ```

2. **VXL**
 ```
 #SKIP conda install -c hcc vxl  (1.17.0) ...not working because it's only available for linux
 pushd ~/code
 git clone git@github.com:vxl/vxl.git
 cd vxl
 git checkout tags/github-migration
 /usr/bin/sed -i bak '1,10 s/^/#/' CMakeLists.txt
 /usr/bin/sed -i bak 's/JPEG_FOUND/JPEG_FOUND_DISABLE/' core/vil/CMakeLists.txt
 /usr/bin/sed -i bak 's/JPEG_FOUND/JPEG_FOUND_DISABLE/' core/vil1/CMakeLists.txt
 mkdir build_oldver
 cd build_oldver
 cmake -DCMAKE_INSTALL_PREFIX=${CONDA_PREFIX} -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DBUILD_CORE_VIDEO:BOOL=OFF -DBUILD_BRL:BOOL=OFF -DBUILD_CONTRIB:BOOL=OFF -DVNL_CONFIG_LEGACY_METHODS=ON -DVCL_STATIC_CONST_INIT_FLOAT=0 -Wno-dev ..
 make -j16 install
 ```
 
3. _skip this step_ **Qt4** (failed but notes are included here anyway)
 ```
 conda install -c conda-forge -c pkgw-forge qt4     (works! 4.8.7)
 export PATH=/Users/cam/tools/miniconda3/envs/shapeworks-olddeps-foundation/qt4/bin:$PATH
 ...spoke too soon. This installation doesn't include QtScript. Other options:
  - try to fix this installation (unlikely since I don't have the source with which it was compiled)
  - install prebuilt (bad idea since it even tells you it might break your system and be uninstallable)
  - build it myself (tried and tried and eventually gave up since it doesn't work on this version OSX)
    /usr/bin/sed -i bak 's/fobjc-gc/fobjc-arc/' CMakeLists.txt
       NOTE: I ran this after the cmake and partial build had completed, so I used the following in the build directory:
       find .. -name "*" -type f -exec grep -i "\-fobjc-gc" {} \; -exec /usr/bin/sed -i bak 's/fobjc-gc/fobjc-arc/' {} \; -print
       ...and anyway, it doesn't work at all, so instead just comment out the -fobjc-arc line in CMakeLists.txt.
  - install macports version (not willing to corrupt my system to install very old software, assuming it works at all)
 Ended up deciding not to install Qt at all, so no ShapeWorksView2.
 ```
 
4. **VTK** (w/o Qt)
 ```
 conda install -c intel tbb-devel   (needed by vtk)
 pushd ~/code
 git clone https://gitlab.kitware.com/vtk/vtk.git
 cd vtk
 git checkout tags/v5.10.1 -b v5.10.1
 mkdir build_5.10.1
 cd build_5.10.1
 cmake -DCMAKE_INSTALL_PREFIX=${CONDA_PREFIX} -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DVTK_USE_QT:BOOL=OFF -DCMAKE_C_FLAGS=-DGLX_GLXEXT_LEGACY -DCMAKE_CXX_FLAGS=-DGLX_GLXEXT_LEGACY -DVTK_USE_SYSTEM_TIFF=ON -DVTK_REQUIRED_OBJCXX_FLAGS="" -Wno-dev ..
 make -j16 install
 ```
 
5. **ITK**
 ```
 pushd ~/code
 git clone git@github.com:InsightSoftwareConsortium/ITK
 git checkout tags/v4.7.2 -b v4.7.2
 mkdir build-4.7.2
 cd build-4.7.2
 /usr/bin/sed -i bak 's/nout > 0/this->nout/' Modules/ThirdParty/VNL/src/vxl/v3p/netlib/linalg/lsqrBase.cxx
 cmake -DCMAKE_INSTALL_PREFIX=${CONDA_PREFIX} -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DBUILD_EXAMPLES:BOOL=OFF -DITK_USE_SYSTEM_VXL=on -Wno-dev ..
 make -j16 install
 ```
 
6. **ShapeWorks**
 ```
 conda create --name shapeworks_build --clone build-foundation
 conda activate shapeworks_build
 cd ~/code/ShapeWorks
 mkdir build_oldver
 cd build_oldver
 cmake -DCMAKE_INSTALL_PREFIX=${CONDA_PREFIX} -DUSE_OPENMP=OFF ..
 make -j16 install
 ```

<a id="user-content-build-mojave-updated"></a>
#### (B) Build with latest versions of VXL, ITK, Python, Qt, and VTK

_TODO_

### Linux(es)

TODO

### Windows

The current method for building ShapeWorks on Windows is to utilize its Linux shell.

1. Install the Linux Subsystem for Windows (details elsewhere online from Microsoft)

2. Install (using 'sudo apt install') packages:

g++-4.8 gcc-4.8 MesaGL libglu1-mesa-dev freeglut3-dev mesa-common-dev libosmesa6-dev cmake

3. See shapeworks_windows_linux_subsystem.sh for details on installing dependencies and shapeworks.

