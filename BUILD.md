# ShapeWorks build and installation instructions

## Installation

_coming soon:_ instructions for user installation without needing to build ShapeWorks

## Build

First, clone the ShapeWorks source, (see [GettingStarted.md](GettingStarted.md#source-and-branches) for more details).
```
$ git clone https://github.com/SCIInstitute/ShapeWorks
```

Then follow the instructions for your specific platform:  

[OSX/Linux](#osx/linux)

[Windows](#windows)  

### OSX/Linux

1. **First** install anaconda and create a sandbox environment.  While this step is optional, we highly recommend it, as using a conda environment allows you to do simultaneous builds, which may each require different dependencies.  Using a conda env allows you to cleanly separate those.  This can be done by running [conda_installs.sh](conda_installs.sh) with the command `./conda_installs.sh`.  Note that the dependencies installed with conda can be done equivalently with apt install [apt_installs.sh](apt_installs.sh) `./apt_installs.sh`.  

2. **Simply** run [superbuild.sh](superbuild.sh) to build and install each dependency and the ShapeWorks tools themselves.  Arguments can be passed to this script to build specific dependencies, set the number of processors to use, and choose whether or not to build certain modules.  These are the arguments the [superbuild script](superbuild.sh) accepts:
```
# Call this script by specifying arguments in the same command.
# Ex:
#                 NUM_PROCS=16 HAVE_QT=1 ./superbuild.sh
#
# Arguments:
#
#   BUILD_CLEAN:   whether or not to remove all build directories and clone new dependencies
#
#   VXL_DIR:       if you already have VXL its install path can be specified
#   VTK_DIR:       if you already have VTK its install path can be specified
#   ITK_DIR:       if you already have ITK its install path can be specified
#
#   BUILD_DIR:     by default creates a subdirectory of the current directory
#                  called 'build' where ShapeWorks and all its external
#                  dependencies will be built
#   INSTALL_DIR:   by default creates a subdirectory of the current directory
#                  called 'install' where ShapeWorks and all external
#                  dependencies are installed
#   NUM_PROCS:     number of processors to use for parallel builds (default is 8)
#
#   BUILD_POST:    whether or not to build any applications in Post
#                  (default is to build them all, see
#                  https://github.com/SCIInstitute/ShapeWorks/issues/58
#                  to understand why this is even an option here)
#   HAVE_QT:       whether or not qt version 5.x is installed in your system,
#                  set 0 if not and it will skip building shapeworksview2
```
For instance:
```
BUILD_CLEAN=1 NUM_PROCS=8 VXL_DIR=/mylocation/ ./superbuild
```

The following dependencies are installed in superbuild. We show which lines they are found on for convenience:

**Qt5** Is an optional dependency which is not installed in the superbuild script.  However, it is required for using ShapeWorksView2 (our visualization application).  If you are planning on using the visualization tools, you can install Qt before running the superbuild script.  This is done easily in conda: `conda install --yes -c anaconda qt`.  Without conda you can install it from their [site](https://doc.qt.io/qt-5/gettingstarted.html#online-installation).

**VXL** [65-77]

**VTK** [79-91]

**ITK** [93-105]

**ShapeWorks** [113-124]  


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
