# ShapeWorks build and installation instructions

## Downloadable installation

_coming soon:_ instructions for user installation without needing to build ShapeWorks

### Clone source

First, clone the ShapeWorks source, (see [GettingStarted.md](GettingStarted.md#source-and-branches) for more details).  
`$ git clone https://github.com/SCIInstitute/ShapeWorks`  
<br>If you want to build a branch other than master, check that branch out next:  
`$ git checkout <branchname>`
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

The current method for building ShapeWorks on Windows is to utilize the Windows Subsystem for Linux.

1. Install the Linux Subsystem for Windows:

https://docs.microsoft.com/en-us/windows/wsl/install-win10

2. Install (using 'sudo apt install') packages:
```
build-essential cmake MesaGL libglu1-mesa-dev freeglut3-dev mesa-common-dev libosmesa6-dev libxrandr-dev libxinerama-dev libglu1-mesa-dev libxcursor-dev libi-dev qt-dev qttools5-dev libqt5x11extras5-dev libqt5x11extras5 libqt53dextras5 libxi-dev python3 python3-numpy python3-termcolor python3-matplotlib
```
Note that if you're using your linux subsystem for other things other than ShapeWorks, it might be beneficial to follow the linux build instructions above, creating a conda environment along the way.

3. To enable visualization, follow the instructions here to install and use VcXsrv

https://seanthegeek.net/234/graphical-linux-applications-bash-ubuntu-windows/

VcXsrv is available here:

https://sourceforge.net/projects/vcxsrv/

### Configuration
Make a build directory and use cmake (or ccmake for a gui version) to configure your build:  
``
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
  -DBuild_PrepTools=[OFF|ON]          default: ON
  -DBuild_Run=[OFF|ON]                default: ON
  -DCMAKE_INSTALL_PREFIX=<path>       default: ./install
  -DCMAKE_BUILD_TYPE=[Debug|Release]  default: Release
```
  Paths to dependencies can be set explicitly. Otherwise they will be downloaded and built automatically.
```
  -DVTK_DIR=<path to your own vtk>
  -DITK_DIR=<path to your own ITK>
  -DVXL_DIR=<path to your own vxl>
  -DSHAPEWORKS_EXTERNALS_CMAKE_BUILD_TYPE=[Debug|Release]  default: Release
```

### Build
Execute build using your generated project file:  
**Makefiles:** `make -j<num_procs>` where num_procs is the number of parallel processes, say 8.
- maybe need to build using `cmake --build . -j 16` in order to pass parallel flags to dependent projects (e.g., vtk)
**XCode project:** `open ShapeWorks.xcodeproj` and build from there  
**Windows Visual Studio:** open it and build.  
