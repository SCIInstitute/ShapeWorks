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

#### Two builds are described next, but only the first yet works:
- [Build with the existing dependencies](#build-mojave-existing)  
- [Build with updated dependencies (Python, VXL, VTK, ITK, and QT)](#build-mojave-updated)

<a id="user-content-build-mojave-existing"></a>
#### (A) Build with existing (very old) dependencies

Starting with an older version of Python (3.5), these instructions are based on how the dependencies were acquired and built by the `superbuild-mac.sh` script, with necessary modifications for compatibility with Mojave.

1. **Create base environment** (if using the Anaconda sandbox)
```
export PYVER="3.5"
conda create --yes --name shapeworks-olddeps-foundation python=$PYVER
conda activate shapeworks-olddeps-foundation
conda install --yes -c anaconda cmake
conda install --yes -c anaconda geotiff
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

Then configure, build, and install just as in [superbuild-mac.sh](superbuild-mac.sh):
```
cmake -DCMAKE_INSTALL_PREFIX=${CONDA_PREFIX} -DBuild_Post:BOOL=1 -DBuild_View2=0 -DUSE_OPENMP=OFF -Wno-dev ..
make -j8 install
```

<a id="user-content-build-mojave-updated"></a>
#### (B) Build with latest versions of VXL, ITK, Python, Qt, and VTK

_TODO_

<a id="user-content-build-osx-older"></a>
### OSX versions < 10.14

TODO: For now, please try the instructions for [Mojave](#build-mojave)


### Linux

Notes for building ShapeWorks on Linux using the existing (very old) dependencies.

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

You can simply run [superbuild-linux.sh](superbuild-linux.sh) to build and install each dependency and the ShapeWorks tools themselves. Arguments can be passed to this script to build specific dependencies, set the number of processors to use, and choose whether or not to build certain modules. [Refer to the script itself](superbuild-linux.sh) for detailed instructions.

2. **VXL**
Follow example in [superbuild-linux.sh](superbuild-linux.sh).

3. **Qt4**
You must install Qt4 if you want to use the ShapeWorksView2 application.

4. **VTK**
If using the Anaconda sandbox, install tbb-devel (needed by vtk):
```
conda install -c intel tbb-devel
```
Continue following [superbuild-linux.sh](superbuild-linux.sh).

5. **ITK**
Follow example in [superbuild-linux.sh](superbuild-linux.sh).

6. **ShapeWorks**
If using the Anaconda sandbox, create a derived environment for this build:
```
conda create --name shapeworks_build --clone build-foundation
conda activate shapeworks_build
```
Then simply continue with the example in [superbuild-linux.sh](superbuild-linux.sh).

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

The current method for building ShapeWorks on Windows is to utilize its Linux shell.

1. Install the Linux Subsystem for Windows (details elsewhere online from Microsoft)

2. Install (using 'sudo apt install') packages:

g++-4.8 gcc-4.8 MesaGL libglu1-mesa-dev freeglut3-dev mesa-common-dev libosmesa6-dev cmake

3. See [superbuild-windows.sh](superbuild-windows.sh) for details on installing ShapeWorks and its dependencies.
