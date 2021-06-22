==========================================
 ShapeWorks   v6.1.0 - Released June 2021
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

You have downloaded a binary distribution of ShapeWorks.

### Installation instructions.

1. Download and install the "Microsoft Visual C++ Redistributable for Visual Studio 2015, 2017 and 2019."

  https://aka.ms/vs/16/release/vc_redist.x64.exe

2. Download and install Miniconda for Windows.

  https://repo.anaconda.com/miniconda/Miniconda3-latest-Windows-x86_64.exe

3. Double-click on the installer that you downloaded and follow the instructions.

  **Note:** Choose whatever installation directory you want.

4. Open an Anaconda terminal and change directory to the chosen installation path.
`cd "C:\Program Files\ShapeWorks"`

5. Create a protected conda environment that installs everything necessary to run.
`install_shapeworks`

  **Note:** You can pass a different name for the environment, enabling multiple installations.
  `install_shapeworks shapeworks_61`

  **Warning:** If you already have anaconda/miniconda installed, this step may
    hang.  If so please uninstall anaconda/miniconda, re-install it, then run
    install_shapeworks again. This seems to be an occasional problem with
    Anaconda on Windows.

**Important:** Your shapeworks conda environment must always be activated before using ShapeWorks.


### ShapeWorks comes with Python examples to get you started.

1. Open an Anaconda terminal and activate the shapeworks conda environment (use the environment name passed to `install_shapeworks` above).
`conda activate shapeworks`

2. Copy the Examples folder to another location of your choosing (you can also use Explorer).
`xcopy /E/H Examples %HOMEPATH%\ShapeWorks-Examples\`

3. Change to the Python folder of the Examples directory you copied.
`cd %HOMEPATH%\ShapeWorks-Examples\Python`

4. Run one of the included use cases. To list them all, run: `python RunUseCase.py --help`.
`python RunUseCase.py --use_case ellipsoid_fd`


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
