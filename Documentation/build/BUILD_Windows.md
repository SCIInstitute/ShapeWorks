# Building ShapeWorks from source on Windows

If you encounter problems, have questions, or need help, please contact shapeworks-dev-support@sci.utah.edu

## Minimum Requirements
* CMake 3.11 (provided by conda below if not already installed)
* MSVC 2019
* Qt 5.9.8 (optional for GUI components)

## Install dependencies

### CMake
Download and install [[CMake]](https://cmake.org/)  
Download and install [[Visual Studio 2019]](https://visualstudio.microsoft.com/)

### Anaconda
Download and install [[Anaconda]](https://www.anaconda.com/)  
It is recommended **not** to add Anaconda to your PATH and **not** to register Anaconda as your default Python.  
Using the *Anaconda Prompt*, run `conda_installs.bat`  

### Qt5  
Download and install the latest version of [[Qt5]](https://download.qt.io/archive/qt/), selecting the LGPL (free) license. (at least version 5.10 required)  
After installing Qt5, add the directory containing `qmake.exe` to your PATH. (See [Adding to PATH](GettingStarted.md#PATH-environment-variable) for help with this)  
Example qmake directory: `D:\Qt\5.14.0\winrt_x64_msvc2017\bin`  

### VXL, VTK, ITK, Eigen and OpenVDB
These dependencies can be installed using the **build_dependencies.sh** script.  
Use an msys shell (e.g. git bash) to do this on Windows.  

Use `$ ./build_dependencies.sh --help` for more details on the available build_dependencies options.  

**If you get an error** that says: `which: no qmake in (...)`  
Make sure you added Qt to your path as explained in the [Install dependencies/Qt5](#Qt5) step.  

If you decide to build ITK yourself and you would like to use the ShapeWorks GUI applications, make sure you build it with VTK  

## Configure and Build  
Use the cmake from the Anaconda Prompt with shapeworks env activated to configure and generate project files for your preferred build system. (e.g. Visual Studio 16 2019)  

### Options
Required:  
```
  -DCMAKE_PREFIX_PATH=<qt cmake path>  (This is different from qmake path in the Install Qt5 step
  -DVXL_DIR=<vxl cmake path>           (contains VXLConfig.cmake)
  -DVTK_DIR=<vtk cmake path>           (contains VTKConfig.cmake)
  -DITK_DIR=<itk cmake path>           (contains ITKConfig.cmake)
  -DEigen3_DIR=<eigen cmake path>      (contains Eigen3Config.cmake)
  -DOpenVDB_DIR=<openvdb cmake path>   (contains FindOpenVDB.cmake)
```
Optional:
```
  -D Build_Studio=[OFF|ON]             default: OFF
  -D Build_View2=[OFF|ON]              default: OFF
  -D Build_Post=[OFF|ON]               default: ON
  -D CMAKE_INSTALL_PREFIX=<path>       default: ./install
  -D CMAKE_BUILD_TYPE=[Debug|Release]  
```

### Examples
Example that builds dependencies separately, then generates a Visual Studio project for ShapeWorks:  
```
> conda activate shapeworks
> ./build_dependencies.sh --build-dir=../dependencies --install-dir=../dependencies
> mkdir build
> cd build
> cmake -G"Visual Studio 16 2019" -Ax64 -DVXL_DIR=../dependencies/vxl/build -DCMAKE_PREFIX_PATH=../dependencies -DBuild_Post:BOOL=ON -DBuild_View2:BOOL=ON -DBuild_Studio:BOOL=ON ..
```
