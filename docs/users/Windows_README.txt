==========================================
 ShapeWorks
==========================================

The ShapeWorks software is an open-source distribution of a new method for
constructing compact statistical point-based models of ensembles of similar
shapes that does not rely on any specific surface parameterization. The method
requires very little preprocessing or parameter tuning and applies to a wide
range of shape analysis problems, including nonmanifold surfaces and objects of
arbitrary topology. The proposed correspondence point optimization uses an
entropy-based minimization that balances the simplicity of the model
(compactness) with the accuracy of the surface representations. The ShapeWorks
software includes tools for preprocessing data, computing point-based shape
models, and visualizing the results.

=====================

Welcome to ShapeWorks!

### To complete your installation:

1. Download and install Miniconda for Windows.

  https://repo.anaconda.com/miniconda/Miniconda3-latest-Windows-x86_64.exe

2. Double-click on the installer that you downloaded and follow the instructions.

3. Open an Anaconda terminal and change directory to the chosen installation path.

`cd "C:\Program Files\ShapeWorks"`

4. Run install_shapeworks.bat to create a conda environment and install python packages

`install_shapeworks.bat`

  **Note:** You can pass a different name for the environment, enabling multiple installations.
  `install_shapeworks shapeworks_61`

  **Warning:** If you already have anaconda/miniconda installed, this step may
    hang.  If so please uninstall anaconda/miniconda, re-install it, then run
    install_shapeworks again. This seems to be an occasional problem with
    Anaconda on Windows.

  **Important:** Your shapeworks conda environment must always be activated before using ShapeWorks python packages.


### ShapeWorks comes with examples to get you started.

1. Copy the Examples folder to another location of your choosing (you can also use Explorer).
`xcopy /E/H Examples %HOMEPATH%\ShapeWorks-Examples\`

### Studio ###

1. Open ShapeWorks Studio

2. Select "Open Exising Project"

3. Select Examples/Studio/Ellipsoid/ellipsoid.xlsx

### Python ###

1. Open an Anaconda terminal and activate the shapeworks conda environment (use the environment name passed to `install_shapeworks` above).

`conda activate shapeworks`

2. Change to the Python folder of the Examples directory you copied.
`cd %HOMEPATH%\ShapeWorks-Examples\Python`

3. Run one of the included use cases. To list them all, run: `python RunUseCase.py --help`.
`python RunUseCase.py ellipsoid`

### ShapeWorks also includes interactive Jupyter Python notebook examples.

1. Open an Anaconda terminal and activate the shapeworks conda environment (use the environment name passed to `install_shapeworks` above).
`conda activate shapeworks`

2. Change to the notebook tutorials folder of the Examples directory you copied.

`cd %HOMEPATH%\ShapeWorks-Examples\Python\notebooks\tutorials`

3. Start the Jupyter notebook server. This will open a new tab in your web brower.

`jupyter notebook`

4. Click on a notebook to get started.


=====================

Please contact us with any questions or ideas.

Website    sciinstitute.github.io/ShapeWorks
Email      shapeworks-users@sci.utah.edu
Discourse  shapeworks.discourse.group
Twitter    @ShapeWorksSuite

=====================
