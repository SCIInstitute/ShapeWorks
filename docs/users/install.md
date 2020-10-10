# How to Install ShapeWorks?


ShapeWorks provides official user releases, the features of which can be seen at [Release Notes](../about/release-notes.md).

First, download the [latest ShapeWorks binary release](https://github.com/SCIInstitute/ShapeWorks/releases/latest).    

We also provide up-to-date development builds from the master branch for:   
[Windows](https://github.com/SCIInstitute/ShapeWorks/releases/tag/dev-windows)  
[Mac](https://github.com/SCIInstitute/ShapeWorks/releases/tag/dev-mac)  
[Linux](https://github.com/SCIInstitute/ShapeWorks/releases/tag/dev-linux)  

!!! important
    Please understand that these are in-progress development builds, not official releases.

Then follow the appropriate instructions for their platform:  
[Windows](#installing-shapeworks-on-windows)   
[Mac](#installing-shapeworks-on-mac)   
[Linux](#installing-shapeworks-on-linux)   

!!! danger "Activate shapeworks environment"
    Each time you use ShapeWorks from the command line, you must first activate its environment using the `conda activate shapeworks` command on the terminal.


## Installing ShapeWorks on Windows

Please make sure that you download the [latest ShapeWorks binary release](https://github.com/SCIInstitute/ShapeWorks/releases/latest), or up-to-date development builds from the master branch for [Windows](https://github.com/SCIInstitute/ShapeWorks/releases/tag/dev-windows). *Please understand that these are in-progress development builds, not official releases.*

To install:

- Download and install the "Microsoft Visual C++ Redistributable for Visual Studio 2015, 2017 and 2019." :

```
https://aka.ms/vs/16/release/vc_redist.x64.exe
```

- Download and install Anaconda for Windows:

```
https://repo.anaconda.com/archive/Anaconda3-2019.10-Windows-x86_64.exe
```

- Copy "C:\Program Files\ShapeWorks\Examples" to another location

- Open "Anaconda Prompt"

- CD to "C:\Program Files\ShapeWorks"

- Run:

```
$ conda_installs.bat
```

- CD to your copied Examples\Python location

- To run an example:

```
$ python RunUseCase.py --use_case [insert name of use case here]
```

- For names for the use cases that are currently released, run:

```
$ python RunUseCase.py --help
```

- More information about running use cases can be found [here](../use-cases/use-cases.md#running-the-use-cases).


Running conda_installs.bat will activate the conda shapeworks environment
into your current anaconda prompt. 

!!! danger "Activate shapeworks environment"
    For subsequent usage, the shapeworks conda environment must be activated using the `conda activate shapeworks` command on the terminal.


!!! note 
    If you have installed ShapeWorks in a different location than "C:\Program Files\ShapeWorks", please add the path to the "bin" directory as the final argument to RunUseCase.py so that the path will be set correctly.



## Installing ShapeWorks on Mac

Please make sure that you download the [latest ShapeWorks binary release](https://github.com/SCIInstitute/ShapeWorks/releases/latest), or up-to-date development builds from the master branch for [Mac](https://github.com/SCIInstitute/ShapeWorks/releases/tag/dev-mac). *Please understand that these are in-progress development builds, not official releases.*


To use the installer version of ShapeWorks (.pkg):

- Install the ShapeWorks pkg file.

- Open a terminal, change directory to /Applications/ShapeWorks

- Type:

```
$ source conda_installs.sh
```

- Copy "/Applications/ShapeWorks/Examples to another location (e.g., $HOME/ShapeWorks-Examples)

- CD to your copied Examples/Python location

- To run an example:

```
$ python RunUseCase.py --use_case [insert name of use case here]
```

- For names for the use cases that are currently released, run:

```
$ python RunUseCase.py --help
```

- More information about running use cases can be found [here](../use-cases/use-cases.md#running-the-use-cases).



!!! danger "Activate shapeworks environment"
    For subsequent usage, the shapeworks conda environment must be activated using the `conda activate shapeworks` command on the terminal.


## Installing ShapeWorks on Linux

Please make sure that you download the [latest ShapeWorks binary release](https://github.com/SCIInstitute/ShapeWorks/releases/latest), or up-to-date development builds from the master branch for [Linux](https://github.com/SCIInstitute/ShapeWorks/releases/tag/dev-linux). *Please understand that these are in-progress development builds, not official releases.*


ShapeWorks comes with [python examples](../use-cases/use-cases.md) to get you started. To run them, you will
need python with a few packages.  The easiest way to **install** them is to run:

```
$ source ./conda_installs.sh
```

Then, to run the example:

```
$ cd Examples/Python
$ python RunUseCase.py --use_case [insert name of use case here]
```

For names for the use cases that are currently released, run:

```
$ python RunUseCase.py --help
```

More information about running use cases can be found [here](../use-cases/use-cases.md#running-the-use-cases).


!!! danger "Activate shapeworks environment"
    For subsequent usage, the shapeworks conda environment must be activated using the `conda activate shapeworks` command on the terminal.
