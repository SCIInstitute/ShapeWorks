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

ShapeWorks 6.8 ships with a complete Python environment built in. There is no
conda environment to set up.


### Windows 11 users: Smart App Control affects Python features

On Windows 11, a security feature called Smart App Control will block the
Python libraries that ShapeWorks ships.

ShapeWorks Studio itself will still run, and you can use its grooming,
optimization, and analysis features normally. However, the following will not
work until Smart App Control is turned off:

  - Python use cases (swpython RunUseCase.py ...)
  - Jupyter notebook examples
  - DeepSSM and other Python-based features inside Studio

If affected, you will see errors like:

  "ImportError: DLL load failed while importing vtkWebCore:
   An Application Control policy has blocked this file."

ShapeWorks bundles common scientific Python libraries (VTK, PyTorch, etc.) that
are not digitally signed by Microsoft, so Smart App Control blocks them. Most
other scientific Python tools have the same issue.

To turn Smart App Control off:

  1. Click the Start button and type "Smart App Control".
  2. Click "Smart App Control settings".
  3. Select "Off".

Note: Turning Smart App Control off is a one-way change. The only way to turn
it back on later is to reset Windows.


### To complete your installation:

1. Double-click on the ShapeWorks installer that you downloaded and follow the
   instructions.

2. The installer creates a "ShapeWorks Prompt" shortcut in the Start Menu. Open
   it whenever you want to run ShapeWorks from the command line — it opens a
   terminal with shapeworks, swpython, and swpip already on PATH.


### ShapeWorks comes with examples to get you started.

1. Copy the Examples folder to another location of your choosing (you can also
   use Explorer):

     xcopy /E/H "C:\Program Files\ShapeWorks\Examples" %HOMEPATH%\ShapeWorks-Examples\


### Studio ###

1. Open ShapeWorks Studio from the Start Menu.

2. Select "Open Existing Project".

3. Select Examples/Studio/Ellipsoid/ellipsoid.xlsx.


### Python ###

1. Open the ShapeWorks Prompt from the Start Menu.

2. Change to the Python folder of the Examples directory you copied.

     cd %HOMEPATH%\ShapeWorks-Examples\Python

3. Run one of the included use cases. To list them all, run:
   `swpython RunUseCase.py --help`.

     swpython RunUseCase.py ellipsoid

The first time you run a DeepSSM use case, ShapeWorks downloads and installs
PyTorch into a per-user directory (~3 GB, one-time). Subsequent runs use the
cached install.


### ShapeWorks also includes interactive Jupyter Python notebook examples.

1. Open the ShapeWorks Prompt from the Start Menu.

2. Install Jupyter the first time you use it:

     swpip install jupyter

   This installs to your per-user ShapeWorks site-packages and persists across
   sessions.

3. Change to the notebook tutorials folder of the Examples directory you copied.

     cd %HOMEPATH%\ShapeWorks-Examples\Python\notebooks\tutorials

4. Start the Jupyter notebook server. This will open a new tab in your web browser.

     swpython -m jupyter notebook

5. Click on a notebook to get started.


=====================

Please contact us with any questions or ideas.

Website    sciinstitute.github.io/ShapeWorks
Email      shapeworks-users@sci.utah.edu
Discourse  shapeworks.discourse.group
Twitter    @ShapeWorksSuite

=====================
