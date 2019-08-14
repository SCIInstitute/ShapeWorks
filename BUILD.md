# ShapeWorks build and installation instructions

## Installation

_coming soon:_ instructions for user installation without needing to build ShapeWorks

## Build

First, [clone the ShapeWorks source](GettingStarted.md#source-and-branches).  
Then follow the instructions for your specific platform:  

[OSX](#osx)  
[Linux](#linux)  
[Windows](#windows)  

### OSX

Notes for building ShapeWorks for OSX.

[First, create a build sandbox using Anaconda](#build-osx-sandbox)  

Next, select the OS version being used:

[Current version (_Mojave_ 10.14)](#build-osx-mojave)  
[Older versions (<10.14)](#build-osx-older)  

<a id="user-content-build-osx-sandbox"></a>
### Install miniconda3 for a sandboxed environment.

This first step is optional but recommended. Anaconda enables simultaneous builds that require different versions of dependencies (e.g., ViSUS, ShapeWorks, and VisIt).

1. Download Miniconda from https://docs.conda.io/en/latest/miniconda.html
2. Install Miniconda
 - `bash ./Miniconda3-latest-MacOSX-x86_64.sh`  
 - accept license agreement and default installation location (or change it).
 - Maybe `conda config --set auto_activate_base false` if you don't want conda available at startup.
3. Update to latest version and install helper utilities
 - `conda update -n base -c defaults conda`
 - `conda install -c conda-forge ctags` (the version included with osx isn't as good)

<a id="user-content-build-osx-mojave"></a>
### OSX 10.14 (Mojave)

To build the current version of ShapeWorks and its dependencies, start with an older version of Python (3.5), and use superbuild.sh.

1. **Create an environment for this build if using Anaconda:
```
export PYVER="3.5"
conda create --yes --name shapeworks-foundation python=$PYVER
conda activate shapeworks-foundation
conda install --yes -c anaconda -c conda-forge cmake openmp ccache
conda install -c intel tbb-devel
```

Next run [superbuild.sh](superbuild.sh) to build and install each dependency and the ShapeWorks tools themselves. Arguments can be passed to this script to build specific dependencies, set the number of processors to use, and choose whether or not to build certain modules.

These are the arguments the [superbuild script](superbuild.sh) accepts:
```
# Call this script by specifying arguments in the same command.
# Ex:
#   NUM_PROCS=16 HAVE_QT=1 ./superbuild-linux.sh
#
# Arguments:
#
#  BUILD_CLEAN:   whether or not to remove all build directories and clone new dependencies
#
#  VXL_DIR:       if you already have VXL its install path can be specified
#  VTK_DIR:       if you already have VTK its install path can be specified
#  ITK_DIR:       if you already have ITK its install path can be specified
#
#  BUILD_DIR:     by default creates a subdirectory of the current directory called 'build' where ShapeWorks and all its external dependencies will be built
#  INSTALL_DIR:   by default creates a subdirectory of the current directory called 'install' where ShapeWorks and all external dependencies are installed
#  NUM_PROCS:     number of processors to use for parallel builds (default is 4)
#
#  BUILD_POST:    whether or not to build any applications in Post (default is to build them all, see https://github.com/SCIInstitute/ShapeWorks/issues/58 to understand why this is even an option here)
#  HAVE_QT:       whether or not qt version 4.x is installed in your system, set 0 if not and it will skip building shapeworksview2
#
```

<a id="user-content-build-osx-older"></a>
### OSX versions < 10.14

TODO: For now, please try the instructions for [Mojave](#build-mojave)


### Linux

Notes for building ShapeWorks on Linux.

[First, create a build sandbox using Anaconda](#build-linux-sandbox)  

Next, select the OS and version being used:

[Generic (manylinux if using Docker)](#generic-linux)  
[SuSE](#suse)  
[Ubuntu](#ubuntu)  

<a id="user-content-build-linux-sandbox"></a>
### Install miniconda3 for a sandboxed environment.

This first step is optional but recommended. Anaconda enables simultaneous builds that require different versions of dependencies (e.g., ViSUS, ShapeWorks, and VisIt).

1. Download Miniconda:  
`wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh`
2. Install Miniconda:  
`bash ./Miniconda3-latest-MacOSX-x86_64.sh`  
accept license agreement and default installation location (or change it).  
3. Update to latest version and install helper utilities:  
 `conda update -n base -c defaults conda`  
 `conda install -c conda-forge ctags` (the version included with most OSes isn't as good)

Starting with an older version of Python (3.5), the following instructions are based on the `superbuild-linux.sh` script.

1. **Create base environment** (if using the Anaconda sandbox)
```
export PYVER="3.5"
conda create --yes --name shapeworks-olddeps-foundation python=$PYVER
conda activate shapeworks-olddeps-foundation
conda install --yes -c anaconda -c conda-forge cmake openmp ccache
```

### Generic Linux

If using Docker, the most generic linux image available is:  
`FROM quay.io/pypa/manylinux1_x86_64`

1. **Create base environment** (if using the Anaconda sandbox)
```
export PYVER="3.5"
conda create --yes --name shapeworks-olddeps-foundation python=$PYVER
conda activate shapeworks-olddeps-foundation
conda install --yes -c anaconda -c conda-forge cmake openmp ccache
```

You can simply run [superbuild.sh](superbuild.sh) to build and install each dependency and the ShapeWorks tools themselves. Arguments can be passed to this script to build specific dependencies, set the number of processors to use, and choose whether or not to build certain modules. [Refer to the script itself](superbuild-linux.sh) for detailed instructions.

2. **VXL**
Follow example in [superbuild.sh](superbuild.sh).

3. **Qt4**
You must install Qt4 if you want to use the ShapeWorksView2 application.
 
4. **VTK**
If using the Anaconda sandbox, install tbb-devel (needed by vtk):
```
conda install -c intel tbb-devel
```
Continue following [superbuild.sh](superbuild.sh).
 
5. **ITK**
Follow example in [superbuild.sh](superbuild.sh).
 
6. **ShapeWorks**

Note: In the superbuild script the shapeworks is set up such that you must run the script from its parent directory i.e. from inside shapeworks repo cloned on your local machine. 
If using the Anaconda sandbox, create a derived environment for this build:
```
conda create --name shapeworks_build --clone build-foundation
conda activate shapeworks_build
```
Then simply continue with the example in [superbuild.sh](superbuild.sh).

#### Running

Some shared libraries are not specified at link time and therefore, if using the Anaconda sandbox, their paths must be made available using the following:  
```
$ export LD_LIBRARY_PATH=$CONDA_PREFIX/lib:$CONDA_PREFIX/lib/vtk-5.10
```
_NOTE:_ This issue may related to a variety of different configuration options, such as our explicit disablement of JPEG in the VXL compilation, or possibly they are problems with old libraries that have since been fixed. We will address these as we upgrade to the latest versions of all dependencies. 

### SuSE

See [Generic Linux][] to get started, and use the following details for OS-specific issues.

_(SuSE-specific instructions)_

### Ubuntu

See [Generic Linux][] to get started, and use the following details for OS-specific issues.

_(Ubuntu-specific instructions)_



### Windows

The current method for building ShapeWorks on Windows is to utilize the Windows Subsystem for Linux.

1. Install the Linux Subsystem for Windows:

https://docs.microsoft.com/en-us/windows/wsl/install-win10

2. Install (using 'sudo apt install') packages:

build-essential cmake MesaGL libglu1-mesa-dev freeglut3-dev mesa-common-dev libosmesa6-dev libxrandr-dev libxinerama-dev libglu1-mesa-dev libxcursor-dev libi-dev qt-dev qttools5-dev libqt5x11extras5-dev libqt5x11extras5 libqt53dextras5 libxi-dev python3 python3-numpy python3-termcolor python3-matplotlib

3. Run [superbuild.sh](superbuild.sh) to build.

