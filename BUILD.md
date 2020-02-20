# Building ShapeWorks from source

## Minimum Requirements

### Linux
* CMake 3.11
* GCC 7.5.0
* Qt 5.9.8 (optional for GUI components)

### Mac
* CMake 3.11
* Clang 10.0.0
* Qt 5.9.8 (optional for GUI components)

### Windows
* CMake 3.11
* MSVC 2019
* Qt 5.9.8 (optional for GUI components)

## Clone source

ShapeWorks uses *git-lfs* to store image data for testing.  
Please install and setup **[git-lfs](https://github.com/git-lfs/git-lfs/wiki/Installation)** before cloning.  
If you have already cloned ShapeWorks before installing *git-lfs*, you can get the test image data using:  
```
$ git lfs fetch
$ git lfs checkout
```

To clone the ShapeWorks source:  
`$ git clone https://github.com/SCIInstitute/ShapeWorks`  
See [GettingStarted.md](GettingStarted.md#source-and-branches) for more details on git commands.  


## Install dependencies

### CMake
Download and install [[CMake]](https://cmake.org/)  
Download and install [[Visual Studio]](https://visualstudio.microsoft.com/) or another CMake-compatible compiler  

### Anaconda
Download and install [[Anaconda]](https://www.anaconda.com/)  
It is recommended **not** to add Anaconda to your PATH and **not** to register Anaconda as your default Python.  
Using the *Anaconda Prompt*, run `conda_installs.sh` on OSX or Linux and run `conda_installs.bat` on Windows  

You can also install these [dependencies](deps.txt) manually if you prefer not to use Anaconda  

### Qt5  
Download and install the latest version of [[Qt5]](https://download.qt.io/archive/qt/), selecting the LGPL (free) license. (at least version 5.10 required)  
After installing Qt5, add the directory containing `qmake.exe` to your PATH. (See [Adding to PATH](GettingStarted.md#PATH-environment-variable) for help with this)  
Example qmake directory Linux: `/opt/Qt5.14.0/5.14.0/gcc_64/bin`  
Example qmake directory Windows: `C:\Qt\5.14.0\winrt_x64_msvc2017\bin`  

### VXL, VTK, ITK, and Eigen
These dependencies can be installed using the **build_dependencies.sh** script.  
On Windows, use an msys shell (e.g. git bash) to do this.  

Use `$ ./build_dependencies.sh --help` for more details on the available options.  

**If you get an error** that looks like this:  
```
which: no qmake in (...)
## For GUI applications, please make sure at least version $QT_MIN_VER of Qt5 is installed and that its qmake is in the path.
## Download Qt5 from: https://download.qt.io/archive/qt/
```
Make sure you added Qt to your path as explained in the [Install dependencies/Qt5](#Qt5) step.  

If you decide to build ITK yourself and you would like to use the ShapeWorks GUI applications, make sure you build it with VTK  


## Configure and Build  
Make a build directory and use cmake to configure your build:  
```
mkdir build
cd build
cmake <options> ..
```
CMake GUI to see and change any of the options:
- On OSX/Linux, you can use a GUI by running `ccmake` instead of `cmake`.  
- On Windows, you can use the CMake application.  

### Options
Required:  
```
  -G<generator> (For example: -GXCode or -G"Visual Studio 16 2019" -Ax64)
  -DCMAKE_PREFIX_PATH=<qt cmake path>  (This is different from qmake path in the Install Qt5 step
  -DVXL_DIR=<vxl cmake path>           (contains VXLConfig.cmake)
  -DVTK_DIR=<vtk cmake path>           (contains VTKConfig.cmake)
  -DITK_DIR=<itk cmake path>           (contains ITKConfig.cmake)
```
Optional:
```
  -DBuild_Studio=[OFF|ON]             default: OFF
  -DBuild_View2=[OFF|ON]              default: OFF
  -DBuild_Post=[OFF|ON]               default: OFF
  -DCMAKE_INSTALL_PREFIX=<path>       default: ./install
  -DCMAKE_BUILD_TYPE=[Debug|Release]  
```
**See [examples](#Examples) below for common values of the variables**  

### Building
- Makefiles: `make -j<num_procs>` where num_procs is the number of parallel processes, say 8.  
    - (maybe need to build using `cmake --build . -j 16` in order to pass parallel flags to dependent projects (e.g., vtk))  
- XCode project: `open ShapeWorks.xcodeproj` and build from there  
- Microsoft Visual Studio: Open ShapeWorks.sln and build from there  

### Before running Example Python scripts
Add the ShapeWorks and dependency binaries to the path:  
- *OSX/Linux:* `$ export PATH=/path/to/shapeworks/build/bin;/path/to/dependencies/bin:$PATH`  
- *Windows*: `> set PATH=\path\to\shapeworks\build\bin;\path\to\dependency\bin;%PATH%`  

### Examples
*OSX* example that builds dependencies separately, then generates an XCode project for ShapeWorks:  
```
$ ./build_dependencies.sh --build-dir=../dependencies --install-dir=../dependencies
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=./install  -DCMAKE_PREFIX_PATH="${PWD}/../../dependencies" -DBuild_Post:BOOL=ON -DBuild_View2:BOOL=ON -DBuild_Studio:BOOL=ON -DUSE_OPENMP=OFF -Wno-dev -Wno-deprecated -GXcode ..
open ShapeWorks.xcodeproj
```

*Windows* example that builds dependencies separately, then generates a Visual Studio project for ShapeWorks:  
```
$ ./build_dependencies.sh --build-dir=../dependencies --install-dir=../dependencies
mkdir build
cd build
cmake -G"Visual Studio 16 2019" -Ax64 -DCMAKE_PREFIX_PATH=D:/ProgramFiles/Qt5.14.0/5.14.0/msvc2017_64/lib/cmake -DVXL_DIR=../dependencies/vxl/build -DVTK_DIR=../dependencies/lib/cmake/vtk-8.2 -DITK_DIR=../dependencies/lib/cmake/ITK-5.0 -DBuild_Post:BOOL=ON -DBuild_View2:BOOL=ON -DBuild_Studio:BOOL=ON ..
```
