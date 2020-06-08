# Building ShapeWorks from source

If you encounter problems, have questions, or need help, please contact shapeworks-dev-support@sci.utah.edu

## Minimum Requirements

### Linux
* CMake 3.11 (provided by conda below if not already installed)
* GCC 7.5.0
* Qt 5.9.8 (optional for GUI components)

### Mac
* CMake 3.11 (provided by conda below if not already installed)
* Clang 10.0.0
* Qt 5.9.8 (optional for GUI components)

### Windows
See **[BUILD_Windows.md](BUILD_Windows.md)**

## Clone source

To clone the ShapeWorks source:  
```
$ git clone https://github.com/SCIInstitute/ShapeWorks
```
See **[GIT.md](../Development/GIT.md)** for more details on git commands.  


## Install dependencies

### Anaconda
We use Anaconda (conda) to install many dependencies required for both building and running ShapeWorks. Conda [sub]environments do not affect a machine's global setup in any way, do not require sudo to install, and are only available at the user level when activated.

To install conda and the dependencies it provides (currently requires either bash or zsh shell), run:
```
$ source conda_installs.sh
```

ShapeWorks uses *[git-lfs](https://github.com/git-lfs/git-lfs/* to store image data for testing.  
If git-lfs was not already installed before cloning ShapeWorks, please use the following commands to get this data:  
```
$ git lfs fetch
$ git lfs checkout
```

_<b>Important</b>_  
Each time you build or use ShapeWorks you must first activate its environment:
```
$ conda activate shapeworks
```

### Qt5  
Download and install the latest version of [[Qt5]](https://download.qt.io/archive/qt/), selecting the LGPL (free) license. (at least version 5.10 required)  
After installing Qt5, add the directory containing `qmake.exe` to your PATH. (See [Adding to PATH](../Development/Paths.md#PATH-environment-variable) for help with this)  
Example qmake directory Linux: `/opt/Qt5.14.0/5.14.0/gcc_64/bin`  

### VXL, VTK, ITK, Eigen and OpenVDB
These dependencies can be installed using the **build_dependencies.sh** script.  

Use `$ ./build_dependencies.sh --help` for more details on the available options.  

**If you get an error** that looks like this:  
```
which: no qmake in (...)
## For GUI applications, please make sure at least version $QT_MIN_VER of Qt5 is installed and that its qmake is in the path.
## Download Qt5 from: https://download.qt.io/archive/qt/
```
Make sure you added Qt to your path as explained in the [Install dependencies/Qt5](#Qt5) step.  

If you decide to build ITK yourself and you would like to use the ShapeWorks GUI applications, __it must be built with VTK__  


## Configure and Build  
Make a build directory and use cmake to configure your build:  
```
mkdir build
cd build
cmake <options> ..
```
There is a CMake GUI to see and change any of the options:
- On OSX/Linux, you can use the GUI by running `ccmake` instead of `cmake`.  

### Options
If you used the `build_dependencies.sh` script above, the prefix for all dependencie is the same.
Otherwise, the specific paths to VTK, VXL, ITK, and Eigen3 are all required.

Required (if you used build_dependencies.sh):  
```
  -DCMAKE_PREFIX_PATH=<dependencies install path>
```

Required (otherwise):  
```
  -DVXL_DIR=<vxl cmake path>           (contains VXLConfig.cmake)
  -DVTK_DIR=<vtk cmake path>           (contains VTKConfig.cmake)
  -DITK_DIR=<itk cmake path>           (contains ITKConfig.cmake)
  -DEigen3_DIR=<eigen3 cmake path>     (contains Eigen3Config.cmake)
  -DOpenVDB_DIR=<openvdb cmake path>   (contains FindOpenVDB.cmake)
```

Optional:
```
  -G<generator>                       default: Unix Makefiles (ex: -GXCode or -G"Visual Studio 16 2019" -Ax64)  
  -DBuild_Studio=[OFF|ON]             default: OFF
  -DBuild_View2=[OFF|ON]              default: OFF
  -DBuild_Post=[OFF|ON]               default: OFF
  -DCMAKE_INSTALL_PREFIX=<path>       default: ./install
  -DCMAKE_BUILD_TYPE=[Debug|Release]  default: Release (only required is default generator is used)
```
**See [examples](#Examples) below for common values of the variables**  

### Building
- Makefiles: `make -j<num_procs>` where num_procs is the number of parallel processes, say 8.  
    - (maybe need to build using `cmake --build . -j 16` in order to pass parallel flags to dependent projects (e.g., vtk))  
- XCode project: `open ShapeWorks.xcodeproj` and build from there  

### Before running Example Python scripts
Add the ShapeWorks and dependency binaries to the path:  
- *OSX/Linux:* `$ export PATH=/path/to/shapeworks/build/bin:/path/to/dependencies/bin:$PATH`  

### Examples
*OSX* example that builds dependencies separately, then generates an XCode project for ShapeWorks:  
```
$ ./build_dependencies.sh
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH="${PWD}/../dependencies/install" -DBuild_Post:BOOL=ON -DBuild_View2:BOOL=ON -DBuild_Studio:BOOL=ON -DUSE_OPENMP=OFF -Wno-dev -Wno-deprecated -GXcode ..
open ShapeWorks.xcodeproj
```
