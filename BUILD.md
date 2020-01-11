# ShapeWorks build and installation instructions

## Downloadable installation

Pre-built binary downloads are provided here:

https://github.com/SCIInstitute/ShapeWorks/releases

## Build from source

### Clone source

ShapeWorks uses git-lfs to store image data for testing.  Please install and setup git lfs before cloning:

git lfs install

If this command fails, you probably need to install git-lfs:

https://github.com/git-lfs/git-lfs/wiki/Installation

If you have already cloned ShapeWorks before installing git-lfs, you can get the test image data using:

git lfs fetch
git lfs checkout

First, clone the ShapeWorks source, (see [GettingStarted.md](GettingStarted.md#source-and-branches) for more details).  
`$ git clone https://github.com/SCIInstitute/ShapeWorks`  
<br>If you want to build a branch other than master, check that branch out next:  
`$ git checkout <branchname>`
<br>Available branches can be listed using:  
`$ git branch -a`
<br>If you want to create a new branch based off of the current branch:  
`$ git checkout -b <branchname>`

### Install dependencies (OSX/Linux)

We recommend using anaconda to create a sandbox environment. While optional, using a conda environment allows multiple builds with different dependencies. You can install Anaconda and the [ShapeWorks dependencies](deps.txt) using:  
`source ./conda_installs.sh`  
<br>Accept the cryptography license terms and default installation path.  
<br>These dependencies can be manually installed if preferred. [Here is the list](deps.txt).  
Note that conda may install a different version of Qt that is too old to use with ShapeWorks. Be sure to set the `CMAKE_PREFIX_PATH` as described below in [Configuration](#Configuration).

Install **Qt5**, required by ShapeWorks gui applications (at least version 5.10).  
Download and install the latest version for your OS, selecting the LGPL (free) license.  
[[OSX]](https://download.qt.io/archive/qt/5.13/5.13.0/qt-opensource-mac-x64-5.13.0.dmg) [[Linux]](https://download.qt.io/archive/qt/5.13/5.13.0/qt-opensource-linux-x64-5.13.0.run) [[Windows]](https://download.qt.io/archive/qt/5.13/5.13.0/qt-opensource-windows-x86-5.13.0.exe)  
<br>Ensure the bin directory for this Qt is in your path:  
**OSX/Linux:** `$ export PATH=<path to your installation>/bin:$PATH`  
**Windows:** `c:\shapeworks> set PATH=<path to your installation>\bin:%PATH%`     #todo: double check this

### Install dependencies (Windows)

Install anaconda and Qt5 as above.

### Superbuild

After the above dependencies installed via conda, ShapeWorks uses VXL, ITK, and VTK.  You can either provide your own (note: ITK must be built with VTK support), or use the provided superbuild.sh script to build all three (and optionally ShapeWorks itself).  On windows, please use an msys shell (e.g. git bash) and run the same superbuild.sh to build VXL, VTK, and ITK.

The **superbuild.sh** script accepts various options, such as specifying the locations of dependencies and whether or not to build the GUI. It can be used to build everything, or just the dependencies.  
Use `./superbuild.sh --help` for more details.

### Configuration
The following instructions describe how to configure and build ShapeWorks independently if the superbuild.sh script was only used to build dependencies.  
Make a build directory and use cmake (or ccmake for a gui version) to configure your build:  
```
mkdir build
cd build
cmake <options> ..
```
Note: using `ccmake` will present the user with a GUI that makes it easy to see and change any of the options.

Options include the following:
```
  -G <generator> (e.g., -GXCode)
  -DCMAKE_PREFIX_PATH=<lib Path(s)>   default: a ;-separated list of paths to Qt, vtk, ITK, etc.
                                               For Qt: it finds qmake in the system PATH; A prefix path must contain the lib/cmake directory.
  -DBuild_Studio=[OFF|ON]             default: OFF
  -DBuild_View2=[OFF|ON]              default: OFF
  -DBuild_Post=[OFF|ON]               default: ON
  -DCMAKE_INSTALL_PREFIX=<path>       default: ./install
  -DCMAKE_BUILD_TYPE=[Debug|Release]  default: Release
```
  Paths to dependencies can be set explicitly. Otherwise they will be downloaded and built automatically.
```
  -DVTK_DIR=<path to your own vtk>
  -DITK_DIR=<path to your own ITK> # Note that ITK must be build with VTK for GUI applications
  -DVXL_DIR=<path to your own vxl>
```

### Build
Execute build using your generated project file:  
**Makefiles:** `make -j<num_procs>` where num_procs is the number of parallel processes, say 8.
- maybe need to build using `cmake --build . -j 16` in order to pass parallel flags to dependent projects (e.g., vtk)
**XCode project:** `open ShapeWorks.xcodeproj` and build from there  
**Windows Visual Studio:** open it and build.  


**Example (using OSX) that builds dependencies separately, then generates an XCode project for ShapeWorks:**  
```
$ ./superbuild.sh  --dependencies_only --build-dir=../dependencies --install-dir=../dependencies
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=./install  -DCMAKE_PREFIX_PATH="${PWD}/../../dependencies" -DBuild_Post:BOOL=ON -DBuild_View2:BOOL=ON -DBuild_Studio:BOOL=ON -DUSE_OPENMP=OFF -Wno-dev -Wno-deprecated -GXcode ..
open ShapeWorks.xcodeproj
```
