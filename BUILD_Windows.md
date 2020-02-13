# Building ShapeWorks from source

This document contains detailed instructions on building ShapeWorks from scratch after cloning the repository ([Instructions on cloning](#How-To-Clone)). This guide will consist of two main steps. The steps will be summarized, but you can click on the links to access further information.

1. [Installing dependencies](#Install-Dependencies)
2. [Configuring and building](#Configure-and-Build)

## Install dependencies

### CMake
Download and install [[CMake]](https://cmake.org/)  
Download and install [[Visual Studio]](https://visualstudio.microsoft.com/) or another CMake-compatible compiler  

### Anaconda
Download and install [[Anaconda]](https://www.anaconda.com/)  
It is recommended **not** to add Anaconda to your PATH and **not** to register Anaconda as your default Python.  
Using the *Anaconda Prompt*, run `conda_installs.bat`  

### Qt5  
Download and install the latest version of [[Qt5]](https://download.qt.io/archive/qt/), selecting the LGPL (free) license. (at least version 5.10 required)  
After installing Qt5, add the directory containing `qmake.exe` to your PATH. (See [Adding to PATH](GettingStarted.md#PATH-environment-variable) for help with this)  
Example qmake directory: `D:\Qt\5.14.0\winrt_x64_msvc2017\bin`  

### VXL, VTK, and ITK
These three dependencies can be installed using the **superbuild.sh** script.  
Use an msys shell (e.g. git bash) to do this on Windows.  
It is recommended to use `$ ./superbuild.sh --dependencies-only` to build VXL, VTK, and ITK.  

Use `$ ./superbuild.sh --help` for more details on the available superbuild options.  

**If you get an error** that looks like this:  
```
which: no qmake in (...)
```
Make sure you added Qt to your path as explained in the [Install dependencies/Qt5](#Qt5) step.  

## Configure and Build  
Use the CMake-gui to configure and generate project files for your preferred build system. (e.g. Visual Studio 16 2019)  

### Options
Required:  
```
  -D VXL_DIR=<vxl cmake path>           (contains VXLConfig.cmake)
  -D VTK_DIR=<vtk cmake path>           (contains VTKConfig.cmake)
  -D ITK_DIR=<itk cmake path>           (contains ITKConfig.cmake)
```
Optional:
```
  -D Build_Studio=[OFF|ON]             default: OFF
  -D Build_View2=[OFF|ON]              default: OFF
  -D Build_Post=[OFF|ON]               default: ON
  -D CMAKE_INSTALL_PREFIX=<path>       default: ./install
  -D CMAKE_BUILD_TYPE=[Debug|Release]  
```
**See [examples](#Examples) below for common values of the variables**  

### Before running Example Python scripts
Add the ShapeWorks and dependency binaries to the path:  
`> set PATH=\path\to\shapeworks\build\bin;\path\to\dependency\bin;%PATH%`  

### Examples
*Windows* example that builds dependencies separately, then generates a Visual Studio project for ShapeWorks:  
```
> ./superbuild.sh  --dependencies_only --build-dir=../dependencies --install-dir=../dependencies
> mkdir build
> cd build
> cmake -G"Visual Studio 16 2019" -Ax64 -DVXL_DIR=../dependencies/vxl/build -DVTK_DIR=../dependencies/lib/cmake/vtk-8.2 -DITK_DIR=../dependencies/lib/cmake/ITK-5.0 -DBuild_Post:BOOL=ON -DBuild_View2:BOOL=ON -DBuild_Studio:BOOL=ON ..
```
