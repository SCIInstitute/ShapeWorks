# How to Install ShapeWorks?


## _**First, download ShapeWorks**_

!!! important "Release Builds"
    We provide the following [official user releases](https://github.com/SCIInstitute/ShapeWorks/releases/latest).  
    [**Windows**](https://github.com/SCIInstitute/ShapeWorks/releases/download/v6.0.0/ShapeWorks-v6.0.0-windows.exe)  
    [**Mac**](https://github.com/SCIInstitute/ShapeWorks/releases/download/v6.0.0/ShapeWorks-v6.0.0-mac.pkg)  
    [**Linux**](https://github.com/SCIInstitute/ShapeWorks/releases/download/v6.0.0/ShapeWorks-v6.0.0-linux.zip)  
  
     Their features can be seen at [Release Notes](../about/release-notes.md).

!!! danger "Development Builds"
    We also provide up-to-date development builds from the **master** branch  
    [Windows Dev Build](https://github.com/SCIInstitute/ShapeWorks/releases/tag/dev-windows)  
    [Mac Dev Build](https://github.com/SCIInstitute/ShapeWorks/releases/tag/dev-mac)  
    [Linux Dev Build](https://github.com/SCIInstitute/ShapeWorks/releases/tag/dev-linux)
    
    Please understand that these are in-progress development builds, not official releases.

## _**Next, follow the instructions for your platform.**_
[Windows](#installing-shapeworks-on-windows)  
[Mac](#installing-shapeworks-on-mac)  
[Linux](#installing-shapeworks-on-linux)



## Installing ShapeWorks on Windows

Please make sure that you download the [latest ShapeWorks binary release](https://github.com/SCIInstitute/ShapeWorks/releases/latest), or up-to-date development builds from the master branch for [Windows](https://github.com/SCIInstitute/ShapeWorks/releases/tag/dev-windows) *(remember these are in-progress development builds, not official releases).*

### Installation instructions.

1. Download and install the "Microsoft Visual C++ Redistributable for Visual Studio 2015, 2017 and 2019."  
[https://aka.ms/vs/16/release/vc_redist.x64.exe](https://aka.ms/vs/16/release/vc_redist.x64.exe)

1. Download and install Miniconda for Windows.  
  [https://repo.anaconda.com/miniconda/Miniconda3-latest-Windows-x86_64.exe](https://repo.anaconda.com/miniconda/Miniconda3-latest-Windows-x86_64.exe)

1. Double-click on the installer that you downloaded and follow the instructions.  
   Please choose whatever installation directory you want.

1. Open an Anaconda terminal and change directory to the chosen installation path.  
`cd "C:\Program Files\ShapeWorks"`

1. Create the **shapeworks** conda environment, installing everything necessary to run.  
`install_shapeworks`  
**Note:** You can pass a different name for the environment, enabling multiple installations.  
`install_shapeworks shapeworks_61`

!!! danger "Warning"
    If you already have anaconda/miniconda installed, this step may hang.  If so please uninstall anaconda/miniconda, re-install it, then run install_shapeworks again. This seems to be an occasional problem with Anaconda on Windows.


#### ShapeWorks comes with Python examples to get you started.

1. Open an Anaconda terminal and activate the shapeworks conda environment (use the environment name passed to **install_shapeworks** above).  
`conda activate shapeworks`

2. Copy the Examples folder to another location of your choosing (you can also use Explorer).  
`xcopy /E/H Examples %HOMEPATH%\ShapeWorks-Examples\`

3. Change to the Python folder of the Examples directory you copied.  
`cd %HOMEPATH%\ShapeWorks-Examples\Python`

4. Run one of the included use cases. To list them all, run: `python RunUseCase.py --help`.  
`python RunUseCase.py <insert name of use case here>`

- More information about running use cases can be found [here](../use-cases/use-cases.md#running-the-use-cases).


#### ShapeWorks also includes interactive Jupyter Python notebook examples.

1. Open an Anaconda terminal and activate the shapeworks conda environment (use the environment name passed to **install_shapeworks** above).  
`conda activate shapeworks`

2. Change to the notebook tutorials folder of the Examples directory you copied.  
`cd %HOMEPATH%\ShapeWorks-Examples\Python\notebooks\tutorials`

3. Start the Jupyter notebook server. This will open a new tab in your web brower.  
`jupyter notebook`

4. Click on a notebook to get started.

!!! note "Important"
    Your shapeworks conda environment must always be activated before using ShapeWorks.

!!! note "Using **git-bash** on Windows"
    Windows may also have a **git-bash** command line available. This also works with conda, and environments are activated in the same way. However, there is one important issue: running Python scripts requires prefixing with `winpty`. For example, `winpty 
    ...`.

## Installing ShapeWorks on Mac

Please make sure that you download the [latest ShapeWorks binary release](https://github.com/SCIInstitute/ShapeWorks/releases/latest), or up-to-date development builds from the master branch for [Mac](https://github.com/SCIInstitute/ShapeWorks/releases/tag/dev-mac) *(remember these are in-progress development builds, not official releases).*

#### Installation instructions.

1. Install the ShapeWorks .pkg file by double-clicking on the file.

1. Open a terminal and change directory to the installation path.  
`cd /Applications/ShapeWorks`

1. Create a protected conda environment that installs everything necessary to run.  
`source install_shapeworks.sh`  
  **Note:** You can pass a different name for the environment, enabling multiple installations.  
  `source install_shapeworks.sh shapeworks_61`


#### ShapeWorks comes with Python examples to get you started.

1. Open a terminal and activate the shapeworks conda environment (use the environment name passed to **install_shapeworks** above).  
`conda activate shapeworks`

2. Copy the Examples folder to another location of your choosing.  
`cp -r Examples $HOME/ShapeWorks-Examples`

3. Change to the Python folder of the Examples directory you copied.  
`cd $HOME/ShapeWorks-Examples/Python`

4. Run one of the included use cases. To list them all, run: `python RunUseCase.py --help`.  
`python RunUseCase.py <insert name of use case here>`

- More information about running use cases can be found [here](../use-cases/use-cases.md#running-the-use-cases).


#### ShapeWorks also includes interactive Jupyter Python notebook examples.

1. Open a terminal and activate the shapeworks conda environment (use the environment name passed to **install_shapeworks** above).  
`conda activate shapeworks`

2. Change to the notebook tutorials folder of the Examples directory you copied.  
`cd $HOME/ShapeWorks-Examples/Python/notebooks/tutorials`

3. Start the Jupyter notebook server. This will open a new tab in your web brower.  
`jupyter notebook`

4. Click on a notebook to get started.

!!! note "Important"
    Your shapeworks conda environment must always be activated before using ShapeWorks.



## Installing ShapeWorks on Linux

Please make sure that you download the [latest ShapeWorks binary release](https://github.com/SCIInstitute/ShapeWorks/releases/latest), or up-to-date development builds from the master branch for [Linux](https://github.com/SCIInstitute/ShapeWorks/releases/tag/dev-linux) *(remember these are in-progress development builds, not official releases).*

#### Installation instructions.

1. Open a terminal and change directory to the installation path (where you unzipped the downloaded file).  
`cd /path/to/shapeworks`

2. Create a protected conda environment that installs everything necessary to run.  
`source install_shapeworks.sh`  
  **Note:** You can pass a different name for the environment, enabling multiple installations.  
  `source install_shapeworks.sh shapeworks_61`


#### ShapeWorks comes with Python examples to get you started.

1. Open a terminal and activate the shapeworks conda environment (use the environment name passed to **install_shapeworks** above).  
`conda activate shapeworks`

2. Copy the Examples folder to another location of your choosing.  
`cp -r Examples $HOME/ShapeWorks-Examples`

3. Change to the Python folder of the Examples directory you copied.  
`cd $HOME/ShapeWorks-Examples/Python`

4. Run one of the included use cases. To list them all, run: `python RunUseCase.py --help`.  
`python RunUseCase.py <insert name of use case here>`

- More information about running use cases can be found [here](../use-cases/use-cases.md#running-the-use-cases).


#### ShapeWorks also includes interactive Jupyter Python notebook examples.

1. Open a terminal and activate the shapeworks conda environment (use the environment name passed to **install_shapeworks** above).  
`conda activate shapeworks`

2. Change to the notebook tutorials folder of the Examples directory you copied.  
`cd $HOME/ShapeWorks-Examples/Python/notebooks/tutorials`

3. Start the Jupyter notebook server. This will open a new tab in your web brower.  
`jupyter notebook`

4. Click on a notebook to get started.

!!! note "Important"
    Your shapeworks conda environment must always be activated before using ShapeWorks.

