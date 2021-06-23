# Adding Environment Variables for Development

!!! danger "This is unnecessary for users!"
    Users only need to activate their conda environments (`conda activate shapeworks`).
    In the past, setting PATH was necessary. Now it will only cause problems. 

!!! note "The simplest way for developers"
    A script called **devenv** is provided to setup developer environments on all platforms.

## Using **devenv** script to configure developer environment

The **devenv** script, short for **dev**eloper **env**ironment, only requires one parameter: the path to the build binaries. It infers the source path from its own location.  

It configures the environment of the terminal from which it is sourced such that:  

- PATH and PYTHONPATH point to $BUILD_BIN directory  
- PYTHONPATH points to each module in $SOURCE/Python  

This is very useful for development, since after sourcing this, processes in the
environment will use executables from the given build and import modules from
the developer's source--this includes the compiled portion of the Python bindings!

It dramatically reduces the probability of using the wrong paths for testing,
and eliminates the need to **pip install** any of the Python modules during
development.

As an example of using it with a make-based build directory inside source on Linux or OSX:
```
(shapeworks) ~/code/ShapeWorks$ source ./devenv.sh ./build/bin
```

On Windows, to point to the RelWithDebInfo binaries in a build directory inside source:
```
(shapeworks) ~/code/ShapeWorks> devenv .\build\bin\RelWithDebInfo
```

!!! note "Using **git-bash** on Windows"
    Windows may also have a **git-bash** command line available. This also works with `devenv`: just `source ./devenv.sh` like you would on linux/osx.  
    There is one important issue: running Python scripts requires prefixing with `winpty`. For example, `winpty python RunUseCase.py ...`.

Any path can be passed as the parameter for this script and it can be called from any location. Another example:
```
(shapeworks) ~/code/ShapeWorks/build_xcode/bin/Debug$ source ../../devenv.sh Debug
```

### Manually adding paths

While unnecessary for the use of ShapeWorks, these are the steps to manually add items to your paths.

#### OSX/Linux

`$ export PATH=path/to/add:$PATH`  

Verify the results with the command: `$ echo $PATH`  

#### Windows

`$ set PATH=path/to/add;%PATH%`  
This only modifies the path for the current command prompt.  

To permanently add to the path (or so you can remove what has previously been added):  

- Go to *Settings/Edit the system environment variables/Environment Variables*  
- Choose the *Path* variable and press *Edit...*  
- Add your path entry to the list  

Verify the results with the command: `$ echo %PATH%`  

