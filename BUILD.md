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

1. Create an environment for this build if using Anaconda:
```
wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh
bash ./Miniconda3-latest-MacOSX-x86_64.sh
conda create --yes --name shapeworks-foundation python=3.5
conda activate shapeworks
conda install --yes -c anaconda cmake
conda install --yes -c anaconda geotiff
conda install --yes -c anaconda libxrandr-devel-cos6-x86_64
conda install --yes -c conda-forge xorg-libx11
conda install --yes -c anaconda libxinerama-devel-cos6-x86_64
conda install --yes -c anaconda libxcursor-devel-cos6-x86_64
conda install --yes -c anaconda libxi-devel-cos6-x86_64
conda install --yes numpy
conda install --yes matplotlib
conda install --yes colorama
```

You can simply run [superbuild-mac.sh](superbuild-mac.sh) to build and install each dependency and the ShapeWorks tools themselves. Arguments can be passed to this script to build specific dependencies, set the number of processors to use, and choose whether or not to build certain modules. Refer to [the script itself](superbuild-mac.sh) for detailed instructions.

2. **VXL**
Follow example in [superbuild-mac.sh](superbuild-mac.sh).

3. _skip this step_ **Qt4** (failed but notes are included here anyway)
```
conda install -c conda-forge -c pkgw-forge qt4     (works! 4.8.7)
export PATH=/Users/cam/tools/miniconda3/envs/shapeworks-olddeps-foundation/qt4/bin:$PATH
...spoke too soon. This installation doesn't include QtScript. Other options:
 - try to fix this installation (unlikely since I don't have the source with which it was compiled)
 - install prebuilt (bad idea since it even tells you it might break your system and be uninstallable)
 - build it myself (tried and tried and eventually gave up since it doesn't work on this version OSX)
   /usr/bin/sed -i bak 's/fobjc-gc/fobjc-arc/' CMakeLists.txt
      NOTE: I ran this after the cmake and partial build had completed, so I used the following in the build directory:
      find .. -name "*" -type f -exec grep -i "\-fobjc-gc" {} \; -exec /usr/bin/sed -i bak 's/fobjc-gc/fobjc-arc/' {} \; -print
      ...and anyway, it doesn't work at all, so instead just comment out the -fobjc-arc line in CMakeLists.txt.
 - install macports version (not willing to corrupt my system to install very old software, assuming it works at all)
Ended up deciding not to install Qt at all, so no ShapeWorksView2.
```

4. **VTK** (w/o Qt)
If using the Anaconda sandbox, install tbb-devel (needed by vtk):
```
conda install -c intel tbb-devel
```
Continue following [superbuild-mac.sh](superbuild-mac.sh).

5. **ITK**
Follow example in [superbuild-mac.sh](superbuild-mac.sh).

6. **ShapeWorks**
If using the Anaconda sandbox, create a derived environment for this build:
```
conda create --name shapeworks_build --clone build-foundation
conda activate shapeworks_build
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

1. **First** install anaconda and create a sandbox environment. While this step is optional, we highly recommend it, as using a conda environment allows you to do simultaneous builds, which may each require different dependencies. Using a conda env allows you to cleanly separate those. Note that the dependencies installed with the following conda install commands can be done equivalently with apt install. The following lines are also found in [conda_installs.sh](conda_installs.sh), and will be run with the command `./conda_installs.sh`.  
```
wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh
bash ./Miniconda3-latest-Linux-x86_64.sh
source ~/miniconda3/bin/activate
conda create --yes --name shapeworks python=3.5
conda activate shapeworks
conda install --yes -c anaconda cmake
conda install --yes -c anaconda geotiff
conda install --yes -c anaconda libxrandr-devel-cos6-x86_64
conda install --yes -c conda-forge xorg-libx11
conda install --yes -c anaconda libxinerama-devel-cos6-x86_64
conda install --yes -c anaconda libxcursor-devel-cos6-x86_64
conda install --yes -c anaconda libxi-devel-cos6-x86_64
conda install --yes numpy
conda install --yes matplotlib
conda install --yes colorama
```

2. **Simply** run [superbuild.sh](superbuild.sh) to build and install each dependency and the ShapeWorks tools themselves. Arguments can be passed to this script to build specific dependencies, set the number of processors to use, and choose whether or not to build certain modules. These are the arguments the [superbuild script](superbuild.sh) accepts:
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
For instance:
```
BUILD_CLEAN=1 NUM_PROCS=8 VXL_DIR=/mylocation/ ./superbuild
```

The following dependencies are installed in superbuild. We show which lines they are found on for convenience:

**VXL** [65-77]

**VTK** [79-91]

**ITK** [93-105]

An additional, optional, dependency is Qt, which is not installed in the superbuild script.
**Qt4** (Required for using ShapeWorksView2 application)

**ShapeWorks** is installed in lines [113-124].  


### Windows

The current method for building ShapeWorks on Windows is to utilize its Linux shell.

1. Install the Linux Subsystem for Windows (details elsewhere online from Microsoft)

2. Install (using 'sudo apt install') packages:

g++-4.8 gcc-4.8 MesaGL libglu1-mesa-dev freeglut3-dev mesa-common-dev libosmesa6-dev cmake

3. See [superbuild-windows.sh](superbuild-windows.sh) for details on installing ShapeWorks and its dependencies.
