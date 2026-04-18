# How to Install ShapeWorks?


## _**First, download ShapeWorks**_

!!! important "Release Builds"
    We provide [official user releases](https://github.com/SCIInstitute/ShapeWorks/releases/latest) for Windows, MacOS, and Linux.  
  
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

!!! warning "Windows 11: Smart App Control affects Python features"
    On Windows 11, a security feature called **Smart App Control** will block
    the Python libraries that ShapeWorks installs. **ShapeWorks Studio itself
    will still run**, and you can use its grooming, optimization, and analysis
    features normally. However, the following will not work until Smart App
    Control is turned off:

    - Python use cases (`python RunUseCase.py ...`)
    - Jupyter notebook examples
    - DeepSSM and other Python-based features inside Studio

    If affected, you will see errors like:

    *"ImportError: DLL load failed while importing vtkWebCore: An Application Control policy has blocked this file."*

    This happens because ShapeWorks relies on common scientific Python
    libraries (VTK, PyTorch, etc.) that are not digitally signed by Microsoft.
    Smart App Control blocks unsigned files, even though they are safe.
    Most other scientific Python tools (PyTorch, SciPy, OpenCV, ...) have the
    same issue.

    **To turn Smart App Control off:**

    1. Click the **Start** button and type **Smart App Control**.
    2. Click **Smart App Control settings**.
    3. Select **Off**.

    **Important:** Turning Smart App Control off is a one-way change. The only
    way to turn it back on later is to reset Windows. If you are unsure
    whether your computer has Smart App Control enabled, the
    `install_shapeworks.bat` script will detect it and warn you.


1. Download and install Miniconda for Windows.  
[https://repo.anaconda.com/miniconda/Miniconda3-latest-Windows-x86_64.exe](https://repo.anaconda.com/miniconda/Miniconda3-latest-Windows-x86_64.exe)

2. Double-click on the ShapeWorks installer that you downloaded and follow the installation steps.

3. Open an Anaconda terminal and change directory to the chosen installation path.  
`cd "C:\Program Files\ShapeWorks"`

4. Create the **shapeworks** conda environment, installing everything necessary to run.  
`install_shapeworks.bat`  

**Note:** You can pass a different name for the environment, enabling multiple installations.  
`install_shapeworks shapeworks_61`

!!! danger "Warning"
    If you already have anaconda/miniconda installed, this step may hang.  If so please uninstall anaconda/miniconda, re-install it, then run install_shapeworks again. This seems to be an occasional problem with Anaconda on Windows.

!!! note "Important"
    Your shapeworks conda environment must always be activated before using ShapeWorks python packages.


### ShapeWorks comes with examples to get you started.

1. Copy the Examples folder to another location of your choosing (you can also use Explorer).
`xcopy /E/H Examples %HOMEPATH%\ShapeWorks-Examples\`

#### Studio

1. Open ShapeWorks Studio

2. Select "Open Exising Project"

3. Select Examples/Studio/Ellipsoid/ellipsoid.xlsx

4. Experiment with the Studio interface.

#### Python

1. Open an Anaconda terminal and activate the shapeworks conda environment (use the environment name passed to `install_shapeworks` above).  
  `conda activate shapeworks`

2. Change to the Python folder of the Examples directory you copied.  
`cd %HOMEPATH%\ShapeWorks-Examples\Python`

3. Run one of the included use cases. To list them all, run: `python RunUseCase.py --help`.  
`python RunUseCase.py ellipsoid`

#### ShapeWorks also includes interactive Jupyter Python notebook examples.

1. Open an Anaconda terminal and activate the shapeworks conda environment (use the environment name passed to `install_shapeworks` above).  
`conda activate shapeworks`

2. Change to the notebook tutorials folder of the Examples directory you copied.  
`cd %HOMEPATH%\ShapeWorks-Examples\Python\notebooks\tutorials`

3. Start the Jupyter notebook server. This will open a new tab in your web browser.  
`jupyter notebook`

4. Click on a notebook to get started.

## Installing ShapeWorks on Mac

Please make sure that you download the [latest ShapeWorks binary release](https://github.com/SCIInstitute/ShapeWorks/releases/latest), or up-to-date development builds from the master branch for [Mac](https://github.com/SCIInstitute/ShapeWorks/releases/tag/dev-mac) *(remember these are in-progress development builds, not official releases).*

1. Install the ShapeWorks .pkg file by double-clicking on the file.

2. Open a terminal and change directory to the installation path.  
`cd /Applications/ShapeWorks`

3. Create a protected conda environment that installs everything necessary to run.  
`source install_shapeworks.sh`  
  **Note:** You can pass a different name for the environment, enabling multiple installations.  
  `source install_shapeworks.sh shapeworks_61`

!!! note "Important"
    Your shapeworks conda environment must always be activated before using ShapeWorks python packages.

### ShapeWorks comes with examples to get you started.

#### Studio

1. Copy the /Applications/ShapeWorks/Examples folder to another location of your choosing. (You can also use Finder.)

2. Open ShapeWorks Studio

3. Select "Open Exising Project"

4. Select Examples/Studio/Ellipsoid/ellipsoid.xlsx

5. Experiment with the Studio interface.


#### Python

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

3. Start the Jupyter notebook server. This will open a new tab in your web broswer.  
`jupyter notebook`

4. Click on a notebook to get started.



## Installing ShapeWorks on Linux

Please make sure that you download the [latest ShapeWorks binary release](https://github.com/SCIInstitute/ShapeWorks/releases/latest), or up-to-date development builds from the master branch for [Linux](https://github.com/SCIInstitute/ShapeWorks/releases/tag/dev-linux) *(remember these are in-progress development builds, not official releases).*

1. Open a terminal and change directory to the installation path (where you unzipped the downloaded file).  
`cd /path/to/shapeworks`

2. Create a protected conda environment that installs everything necessary to run.  
`source install_shapeworks.sh`  
  **Note:** You can pass a different name for the environment, enabling multiple installations.  
  `source install_shapeworks.sh shapeworks_61`

!!! note "Important"
    Your shapeworks conda environment must always be activated before using ShapeWorks python packages.

### ShapeWorks comes with examples to get you started.

#### Studio

1. Open ShapeWorks Studio

2. Select "Open Exising Project"

3. Select Examples/Studio/Ellipsoid/ellipsoid.xlsx

4. Experiment with the Studio interface.

#### Python

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

3. Start the Jupyter notebook server. This will open a new tab in your web broswer.  
`jupyter notebook`

4. Click on a notebook to get started.
