# ShapeWorks build and installation instructions

## Downloadable installation

_coming soon:_ instructions for user installation without needing to build ShapeWorks

## Clone source and build

1. First, clone the ShapeWorks source, (see [GettingStarted.md](GettingStarted.md#source-and-branches) for more details).  
`$ git clone https://github.com/SCIInstitute/ShapeWorks`  
<br>If you want to build a branch other than master, check that branch out next:  
`$ git checkout -b <branchname>`

2. Install **Qt5**. ShapeWorks gui applications require **Qt5** or later. Download and install the latest version (5.13) for your OS, selecting the LGPL (free) license.  
[[OSX]](https://download.qt.io/archive/qt/5.13/5.13.0/qt-opensource-mac-x64-5.13.0.dmg) [[Linux]](https://download.qt.io/archive/qt/5.13/5.13.0/qt-opensource-linux-x64-5.13.0.run) [[Windows]](https://download.qt.io/archive/qt/5.13/5.13.0/qt-opensource-windows-x86-5.13.0.exe)  
<br>Ensure the bin directory for this Qt is in your path:  
`$ export PATH=<path to your installation>/bin:$PATH`.

3. Now follow the instructions for your specific platform:  
[[OSX]](#osx/linux) [[Linux]](#osx/linux) [[Windows]](#windows)


### OSX/Linux

1. For other requirements, we recommend using anaconda to create a sandbox environment. While optional, using a conda environment allows multiple builds with different dependencies. You can install Anaconda and the [ShapeWorks dependencies](deps.txt) using:  
`source ./conda_installs.sh`  
<br>These dependencies can be manually installed if preferred. [Here is the list](deps.txt).

2. The `superbuild.sh` script makes building ShapeWorks and its VXL, VTK, and ITK dependencies very easy.  
If using Anaconda, first activate the environment using `conda activate shapeworks`.  
The basic build script is executed with `./superbuild.sh`. Here are additional parameters accepted:
```
ShapeWorks superbuild
---------------------
usage: ./superbuild.sh [[-n=<num-procs>] [-i=<install_path>] [-b=<build_path>] [--clean] [--no-gui] [--qt-dir=<qt_path>] [--vxl-dir=<vxl_path>] [--vtk-dir=<vtk_path>] [--itk-dir=<itk_path>] | [-h | --help]]

If using Anaconda to install prerequisites please first run:
source ./conda_installs.sh

Arguments:
  -h,--help               : Show this screen
  --clean                 : Remove all build directories and clone implicit dependencies
                          : (note that user-specified paths such as --itk-dir=<path> will not be deleted)
  --no-gui                : Do not build the ShapeWorks gui applicaitons, which require Qt 5.x
                          : The GUI is built by default if qmake > 5.x is found in the path or user-specified QT_DIR.
  --with-studio           : Build ShapeWorksStudio (default off)
  -b,--build-dir=<path>   : Build directory for ShapeWorks and its implicit dependencieas (VXL, VTK, and ITK)
                          : By default uses a subdirectory of the current directory called 'build'.
  -i,--install-dir=<path> : Install directory for ShapeWorks and its implicit dependencieas (VXL, VTK, and ITK)
                          : By default uses a subdirectory of the current directory called 'install'.
  -n,--num_procs=<num>    : Number of processors to use for parallel builds (default is 8)
  --vxl-dir=<path>        : Path to existing VXL installation (version >= v2.0.2)
  --vtk-dir=<path>        : Path to existing VTK installation (version >= v8.2.0)
  --itk-dir=<path>        : Path to existing ITK installation (version >= v5.0.1)
  --qt-dir=<path>         : The path to Qt version 5.x that is installed on your system

Example: ./superbuild.sh --num-procs=8 --qt-dir=/path/to/qt
Build results are saved in shapeworks_superbuild.log.
```

### Windows

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

4. Run [superbuild.sh](superbuild.sh) to build.
