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


1. Install the ShapeWorks .pkg file by double-clicking on the file.
   It installs into /Applications/ShapeWorks/.

2. (Optional, for convenience) Put the ShapeWorks command-line tools on your PATH.
   Add this to your ~/.zshrc or ~/.bash_profile:

     export PATH="/Applications/ShapeWorks/bin:$PATH"

   Without this step you can still run the tools using their full paths, e.g.
   /Applications/ShapeWorks/bin/swpython RunUseCase.py ...


### ShapeWorks comes with examples to get you started.

#### Studio

1. Open /Applications/ShapeWorks/ShapeWorksStudio.app (double-click in Finder,
   or `open /Applications/ShapeWorks/ShapeWorksStudio.app` from a terminal).

2. Select "Open Existing Project".

3. Select Examples/Studio/Ellipsoid/ellipsoid.xlsx.

4. Experiment with the Studio interface.


#### Python

1. Copy the Examples folder to a writable location.

     cp -r /Applications/ShapeWorks/Examples $HOME/ShapeWorks-Examples

2. Change to the Python folder of the Examples directory you copied.

     cd $HOME/ShapeWorks-Examples/Python

3. Run one of the included use cases. To list them all, run:
   `swpython RunUseCase.py --help`.

     swpython RunUseCase.py <insert name of use case here>

The first time you run a DeepSSM use case, ShapeWorks downloads and installs
PyTorch into ~/Library/Application Support/ShapeWorks/<version>/site-packages/
(~3 GB, one-time). Subsequent runs use the cached install.


#### ShapeWorks also includes interactive Jupyter Python notebook examples.

1. Install Jupyter the first time you use it:

     swpip install jupyter

   This installs to your per-user ShapeWorks site-packages and persists across
   sessions.

2. Change to the notebook tutorials folder of the Examples directory you copied.

     cd $HOME/ShapeWorks-Examples/Python/notebooks/tutorials

3. Start the Jupyter notebook server. This will open a new tab in your web browser.

     swpython -m jupyter notebook

4. Click on a notebook to get started.


=====================

Please contact us with any questions or ideas.

Website    sciinstitute.github.io/ShapeWorks
Email      shapeworks-users@sci.utah.edu
Discourse  shapeworks.discourse.group
Twitter    @ShapeWorksSuite

=====================
