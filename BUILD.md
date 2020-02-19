---

# Building ShapeWorks from source

---

This document contains detailed instructions on building ShapeWorks from scratch after cloning the repository ([Instructions on cloning](#How-To-Clone)). This guide will consist of two main steps. The steps will be summarized, but you can click on the links to access further information.

1. [Installing dependencies](#Install-Dependencies)
2. [Configuring and building](#Configure-and-Build)

## Install Dependencies

Follow these instructions to install the dependencies required to build shapeworks. Please make sure they are installed correctly before attempting to build it.

1. **Setting up the conda environment**: Using the *Anaconda Prompt*, run `source ./conda_installs.sh` on OSX or Linux and run `conda_installs.bat` on Windows. 

    * **On Windows,** it is recommended **not** to add Anaconda to your PATH and **not** to register Anaconda as your default Python. 
    * You can also install these [dependencies](deps.txt) manually if you prefer not to use Anaconda.

2. **Qt5**: Download and install the latest version of [Qt5](https://download.qt.io/archive/qt/), selecting the LGPL (free) license. (at least version 5.10 required)

   * [[OSX]](https://download.qt.io/archive/qt/5.13/5.13.0/qt-opensource-mac-x64-5.13.0.dmg) [[Linux]](https://download.qt.io/archive/qt/5.13/5.13.0/qt-opensource-linux-x64-5.13.0.run) [[Windows]](https://download.qt.io/archive/qt/5.13/5.13.0/qt-opensource-windows-x86-5.13.0.exe) 

3. **Add Qt5 to path**: After installing Qt5, add the directory containing `qmake.exe` to your PATH. ([How to add to PATH](GettingStarted.md#PATH-environment-variable))  
Example qmake directory Linux: `/opt/Qt5.14.0/5.14.0/gcc_64/bin`  
Example qmake directory Windows: `D:\Qt\5.14.0\winrt_x64_msvc2017\bin`

4. **VXL, VTK, and ITK**: These three dependencies can be installed by running the `$ ./dependencies.sh` script. 
   * **On Windows**, use an msys shell (e.g. git bash) to do this. 
   * If you are getting an error that starts with  **`which: no qmake in (...)`** make sure to execute the last step correctly. If you are still having no luck, [click here](#No-qmake-Error). 
   * If you decide to build ITK yourself and you would like to use the ShapeWorks GUI applications, make sure you build it with VTK. 


## Configure and Build

### Configure and Generate

Make a build directory and use cmake to configure your build:  
```
mkdir build
cd build
cmake <options> ..
```

### Cmake Options

Edit the cmake options to specify the paths for qmake, VXL, VTK, and ITK explicitly.

- **On OSX/Linux**, you can use a GUI by running `ccmake` instead of `cmake`.  
- **On Windows**, you can use the CMake application. You might need to specify the paths.

Required:  
```
  -G<generator> (For example: -GXCode or -G"Visual Studio 16 2019" -Ax64)
  -DCMAKE_PREFIX_PATH=<qt cmake path>  (This is different from qmake path in the Install Qt5 step
  -DVXL_DIR=<vxl cmake path>           (contains VXLConfig.cmake)
  -DVTK_DIR=<vtk cmake path>           (contains VTKConfig.cmake)
  -DITK_DIR=<itk cmake path>           (contains ITKConfig.cmake)
  -DEIGEN3_DIR=<eigen3 cmake path>       (contains Eigen3Config.cmake)
```
Optional:
```
  -DBuild_Studio=[OFF|ON]             default: OFF
  -DBuild_View2=[OFF|ON]              default: OFF
  -DBuild_Post=[OFF|ON]               default: ON
  -DCMAKE_INSTALL_PREFIX=<path>       default: ./install
  -DCMAKE_BUILD_TYPE=[Debug|Release]  
```
**See [examples](#Examples) below for common values of the variables**  

### Building
- Makefiles: `make -j<num_procs>` where num_procs is the number of parallel processes, say 8.  
    - (maybe need to build using `cmake --build . -j 16` in order to pass parallel flags to dependent projects (e.g., vtk))  
- XCode project: `open ShapeWorks.xcodeproj` and build from there  
- Microsoft Visual Studio: Open ShapeWorks.sln and build from there 
- Qt Creator: Open project and select the CMakeLists.txt file in the cloned ShapeWorks directory. Continue through the steps, and Qt Creator will recognize the project automatically. Edit the options in the *projects* tab. 

### Adding ShapeWorks to Path
Before running any example python scripts, add ShapeWorks' dependency binaries to the path:  
- *OSX/Linux:* `$ export PATH=/path/to/shapeworks/build/bin;/path/to/dependencies/bin:$PATH`  
- *Windows*: `> set PATH=\path\to\shapeworks\build\bin;\path\to\dependency\bin;%PATH%`  

### Examples
*OSX* example that builds dependencies separately, then generates an XCode project for ShapeWorks:  
```
$ ./superbuild.sh  --dependencies_only --build-dir=../dependencies --install-dir=../dependencies
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=./install  -DCMAKE_PREFIX_PATH="${PWD}/../dependencies" -DBuild_Post:BOOL=ON -DBuild_View2:BOOL=ON -DBuild_Studio:BOOL=ON -DUSE_OPENMP=OFF -Wno-dev -Wno-deprecated -GXcode ..
open ShapeWorks.xcodeproj
```

*Windows* example that builds dependencies separately, then generates a Visual Studio project for ShapeWorks:  
```
$ ./superbuild.sh  --dependencies_only --build-dir=dependencies --install-dir=dependencies
mkdir build
cd build
cmake -G"Visual Studio 16 2019" -Ax64 -DCMAKE_PREFIX_PATH=D:/ProgramFiles/Qt5.14.0/5.14.0/msvc2017_64/lib/cmake -DVXL_DIR=dependencies/vxl/build -DVTK_DIR=dependencies/lib/cmake/vtk-8.2 -DITK_DIR=dependencies/lib/cmake/ITK-5.0 -DEIGEN3_DIR=dependencies/lib/cmake/eigen-3.3.6/cmake/Eigen3 -DBuild_Post:BOOL=ON -DBuild_View2:BOOL=ON -DBuild_Studio:BOOL=ON ..
```

## Additional Help

### How To Clone

To clone the ShapeWorks source:  
`$ git clone https://github.com/SCIInstitute/ShapeWorks`  
See [GettingStarted.md](GettingStarted.md#source-and-branches) for more details on git commands. 

## Clone Source

ShapeWorks uses *git-lfs* to store image data for testing. Please install and setup **[git-lfs](https://github.com/git-lfs/git-lfs/wiki/Installation)** before cloning. If you have already cloned ShapeWorks before installing *git-lfs*, you can get the test image data using:  
```
$ git lfs fetch
$ git lfs checkout
```

### No qmake Error

If you get an error that looks like this 
```
which: no qmake in (...)
## For GUI applications, please make sure at least version $QT_MIN_VER of Qt5 is installed and that its qmake is in the path.
## Download Qt5 from: https://download.qt.io/archive/qt/
```
it means that you your qmake installed with Qt5 has not been added to path, meaning your machine does not know where to find the program.

If the instructions on **Add Qt5 to path** in [Installing dependencies](#Install-Dependencies) do not work, make sure of the following

* You have installed a compatible version of Qt5.
* You have exported the correct path that links to qmake in your machine. Check the directory of the path you added to confirm qmake is there.
* Make sure to export the path without qmake itself at the end.
* On linux, make sure to start the path with a backslash, which makes it originate from the root directory.
