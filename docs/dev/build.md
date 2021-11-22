# How to Build ShapeWorks from Source?

If you encounter problems, have questions, or need help, please contact `<shapeworks-dev-support@sci.utah.edu>`.

!!! note "After you finish building..."
    While user installations have everything you need in the PATH, developer builds like these need those additions. Please See [Adding Environment Variables for Development](../dev/paths.md) for instructions on the simple **devenv** script that takes care of everything.

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
<!--See **[BUILD_Windows.md](BUILD_Windows.md)**-->

* CMake 3.11 (provided by conda below if not already installed)
* MSVC 2019
* Qt 5.9.8 (optional for GUI components)


## Clone source

To clone the ShapeWorks source:  
```
$ git clone https://github.com/SCIInstitute/ShapeWorks
```
See [How to Contribute?](contribute.md) for more details on git commands.  


## Install dependencies

### Linux and Mac

#### Anaconda
We use Anaconda (conda) to install many dependencies required for both building and running ShapeWorks. Conda [sub]environments do not affect a machine's global setup in any way, do not require sudo to install, and are only available at the user level when activated.

To install conda and the dependencies it provides (currently requires either bash or zsh shell), run:
```
$ source install_shapeworks.sh [environment name]`  
```
**Note:** By default this creates an environment named **shapeworks**, but you can specify a different name and it's okay to have multiple environments.  

ShapeWorks uses *[git-lfs](https://github.com/git-lfs/git-lfs/)* to store image data for testing.  If git-lfs was not already installed before cloning ShapeWorks, please use the following commands to get this data:  
```
$ git lfs fetch
$ git lfs install
$ git lfs checkout
```

!!! danger "Activate shapeworks environment"
    Each time you build or use [ShapeWorks from the command line](../tools/ShapeWorksCommands.md), you must first activate its environment using the `conda activate shapeworks` command on the terminal.


#### Qt5  
Download and install the latest version of [Qt5](https://download.qt.io/archive/qt/), selecting the LGPL (free) license (at least version 5.10 required).  

After installing Qt5, add the directory containing `qmake.exe` to your PATH. See [Adding to PATH Environment Variable](paths.md) for help with this.  

Example qmake directory Linux: `/opt/Qt5.14.0/5.14.0/gcc_64/bin`  

#### VXL, VTK, ITK, Eigen and OpenVDB
These dependencies can be installed using the `build_dependencies.sh` script.  

Use `$ ./build_dependencies.sh --help` for more details on the available options.  


!!! note 
    **If you get an error** that looks like this:  
    ```
    which: no qmake in (...) 
    For GUI applications, please make sure at least version $QT_MIN_VER of Qt5 is installed and that its qmake is in the path.
    Download Qt5 from: https://download.qt.io/archive/qt/
    ```
    
    Make sure you added Qt to your path as explained in the Qt5 installation step.  

!!! important
    If you decide to build ITK yourself and you would like to use the ShapeWorks GUI applications, __ITK must be built with VTK__.  

### Windows

#### CMake
Download and install [[CMake]](https://cmake.org/)  
Download and install [[Visual Studio 2019]](https://visualstudio.microsoft.com/)

#### Anaconda
Download and install [[Anaconda]](https://www.anaconda.com/). 

!!! important
    It is recommended **not** to add Anaconda to your PATH and **not** to register Anaconda as your default Python.  

Using the *Anaconda Prompt*, run `install_shapeworks.bat [environment name]`  
**Note:** By default this creates an environment named **shapeworks**, but you can specify a different name and it's okay to have multiple environments.  


#### Qt5  
Download and install the latest version of [Qt5](https://download.qt.io/archive/qt/), selecting the LGPL (free) license (at least version 5.10 required).   
   
After installing Qt5, add the directory containing `qmake.exe` to your PATH. See [Adding to PATH Environment Variable](paths.md) for help with this.  

Example qmake directory: `D:\Qt\5.14.0\winrt_x64_msvc2017\bin`  

#### VXL, VTK, ITK, Eigen and OpenVDB
These dependencies can be installed using the `build_dependencies.sh` script. Use an msys shell (e.g., git bash) to do this on Windows.  

Use `$ ./build_dependencies.sh --help` for more details on the available build_dependencies options.  

!!! important 
    **If you get an error** that says: `which: no qmake in (...)`  
    Make sure you added Qt to your path as explained in the Qt5 installation step.

!!! important 
    If you decide to build ITK yourself and you would like to use the ShapeWorks GUI applications, __ITK must be built with VTK__.  


## Configure and Build  

### Linux & Mac 
Make a build directory and use cmake to configure your build:  
```
mkdir build
cd build
cmake <options> ..
```
There is a CMake GUI to see and change any of the options:
- On OSX/Linux, you can use the GUI by running `ccmake` instead of `cmake`.  

#### Options
If you used the `build_dependencies.sh` script above, the prefix for all dependencies is the same.
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
:fa-info-circle: See **Examples** below for common values of the variables.

#### Building
- **Makefiles:** 
    - `make -j<num_procs>` where num_procs is the number of parallel processes, say 8.  
    - You might need to build using `cmake --build . -j 16` to pass parallel flags to dependent projects (e.g., vtk) 
- **XCode project:** `open ShapeWorks.xcodeproj` and build from there.  

#### Before running Examples/Python scripts and Examples/Python/notebooks
Developer environment should be set by using the `devenv.sh` script by running this:

```
$ source devenv.sh [build_directory]
```

#### Examples
*OSX* example that builds dependencies separately, then generates an XCode project for ShapeWorks:  

```
$ ./build_dependencies.sh
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH="${PWD}/../dependencies/install" -DBuild_Post:BOOL=ON -DBuild_View2:BOOL=ON -DBuild_Studio:BOOL=ON -DUSE_OPENMP=OFF -Wno-dev -Wno-deprecated -GXcode ..
open ShapeWorks.xcodeproj
```


### Windows

Use the cmake from the Anaconda Prompt with shapeworks env activated to configure and generate project files for your preferred build system (e.g., Visual Studio 16 2019). Like with all the other platforms, after running `build_dependencies.sh` a suggested cmake command is printed. Create a build directory and use it.  

#### Examples
An example that builds dependencies separately then generates a Visual Studio project for ShapeWorks (note that by default a Visual Studio project will be created):  
```
> conda activate shapeworks
> ./build_dependencies.sh --build-dir=../dependencies --install-dir=../dependencies
> mkdir build
> cd build
> cmake -G"Visual Studio 16 2019" -Ax64 -DVXL_DIR=../dependencies/vxl/build -DCMAKE_PREFIX_PATH=../dependencies -DBuild_Post:BOOL=ON -DBuild_View2:BOOL=ON -DBuild_Studio:BOOL=ON ..
```

#### Options
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

After cmake the Visual Studio solution can be opened with `start ShapeWorks.sln` from the build directory.

!!! important "RelWithDebInfo only"
    Currently it's only possible to build **RelWithDebInfo** on Windows.
    
