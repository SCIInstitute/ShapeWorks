# ShapeWorks build and installation instructions

## Downloadable installation

_coming soon:_ instructions for user installation without needing to build ShapeWorks

### Clone source

First, clone the ShapeWorks source, (see [GettingStarted.md](GettingStarted.md#source-and-branches) for more details).  
`$ git clone https://github.com/SCIInstitute/ShapeWorks`  
<br>If you want to build a branch other than master, check that branch out next:  
`$ git checkout -b <branchname>`

### Install dependencies (OSX/Linux)

We recommend using anaconda to create a sandbox environment. While optional, using a conda environment allows multiple builds with different dependencies. You can install Anaconda and the [ShapeWorks dependencies](deps.txt) using:  
`source ./conda_installs.sh`  
<br>Accept the cryptography license terms and default installation path.  
<br>These dependencies can be manually installed if preferred. [Here is the list](deps.txt).

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

### CMake
Make a build directory and use cmake (or ccmake for a gui version) to configure your build: `cd build; cmake <options> ..`   
Options include the following:
```
  -G <generator> (e.g., -GXCode)
```
  Paths to dependencies can be set explicitly. Otherwise they will be downloaded and built automatically, which guarantees the correct version.
```
  -DVTK_DIR=<path to your own vtk>
  -DITK_DIR=<path to your own ITK>
  -DVXL_DIR=<path to your own vxl>
  ...etc
```

### Build
Execute build using your generated project file:  
**Makefiles:** `make -j<num_procs>` where num_procs is the number of parallel processes, say 8.
- maybe need to build using `cmake --build . -j 16` in order to pass parallel flags to dependent projects (e.g., vtk)
**XCode project:** `open ShapeWorks.xcodeproj` and build from there  
**Windows Visual Studio:** open it and build.  





-------------------------------------------------------------------------------
...no more superbuild, so dump this.
2. The `superbuild.sh` script makes building ShapeWorks and its VXL, VTK, and ITK dependencies very easy.  
If using Anaconda, first activate the environment using `conda activate shapeworks`.  
The basic build script is executed with `./superbuild.sh`. Here are additional parameters accepted (also shown by passing `--help` to the command):
```
usage: ./superbuild.sh [[-n=<num-procs>] [-i=<install_path>] [-b=<build_path>] [--clean] [--no-gui] [--vxl-dir=<vxl_path>] [--vtk-dir=<vtk_path>] [--itk-dir=<itk_path>] | [-h | --help]]

If using Anaconda to install prerequisites please first run:
source ./conda_installs.sh

For GUI applications, please make sure at least version 5.10 of Qt5 is installed and that its qmake is in the path.
Download Qt5 from: https://download.qt.io/archive/qt/

Arguments:
  -h,--help               : Show this screen.
  --clean                 : Remove all build directories and clone implicit dependencies.
                          : (note that user-specified paths such as --itk-dir=<path> will not be deleted).
  --no-gui                : Do not build the ShapeWorks gui applicaitons, which require Qt.
                          : The GUI is built by default if qmake >= 5.10 is found in the path.
  --with-studio           : Build ShapeWorksStudio (default off).
  -b,--build-dir=<path>   : Build directory for ShapeWorks and its implicit dependencieas (VXL, VTK, and ITK).
                          : By default uses a subdirectory of the current directory called 'build'.
  -i,--install-dir=<path> : Install directory for ShapeWorks and its implicit dependencieas (VXL, VTK, and ITK).
                          : By default uses a subdirectory of the current directory called 'install'.
  -n,--num-procs=<num>    : Number of processors to use for parallel builds (default is 8).
  --vxl-dir=<path>        : Path to existing VXL installation (version >= v2.0.2).
  --vtk-dir=<path>        : Path to existing VTK installation (version >= v8.2.0).
  --itk-dir=<path>        : Path to existing ITK installation (version >= v5.0.1).

Example: ./superbuild.sh --num-procs=8 --install-dir=/path/to/desired/installation
Build results are saved in shapeworks_superbuild.log.
```
