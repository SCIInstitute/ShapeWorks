# Adding Environment Variables for Development

!!! danger "This is unnecessary for users!"
    **Be careful** doing this! You are responsible for messing up your own PATH.  
    Users only need to activate their conda environments (`conda activate shapeworks`).
    In the past, setting PATH was necessary. Now it will only cause problems. 

!!! note "The simplest way for developers"
    A script called **devenv** is provider to setup developer environments on all platforms.

### Using **devenv** script to configure developer environment

This handy script takes two arguments: the path to your ShapeWorks source directory and the path to your build directory.  
It then configures the environment of the terminal from which it is sourced such that:
- PATH and PYTHONPATH point to $BUILD/bin
- PYTHONPATH points to each module in $SOURCE/Python

This is very useful for development, since after sourcing this, processes in the
environment will use executables from the given build and import modules from
the developer's source--this includes the compiled portion of the Python bindings!

It dramatically reduces the probability of using the wrong paths for testing,
and eliminates the need to **pip install** any of the Python modules during
development.

As an example of using it, when a build directory is inside source:  
On Linux or OSX:
```
(shapeworks) ~/code/ShapeWorks$ source ./devenv.sh `pwd` `pwd`/build_debug
```

On Windows:
```
(shapeworks) ~/code/ShapeWorks> devenv  %cd% %cd%\build
```

Any path can be passed as the parameters for this script. These examples are just demonstrating simple versions that require less typing.


## Manually adding to the PATH on OSX/Linux
`$ export PATH=path/to/add:$PATH`  

Verify the results with the command: `$ echo $PATH`  

## Manually adding to the PATH on Windows
`$ set PATH=path/to/add;%PATH%`  
This only modifies the path for the current command prompt.  

To permanently add to the path:     

- Go to *Settings/Edit the system environment variables/Environment Variables*  
- Choose the *Path* variable and press *Edit...*  
- Add your path entry to the list  

Verify the results with the command: `$ echo %PATH%`  

