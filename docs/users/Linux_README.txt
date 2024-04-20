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

## Installing ShapeWorks on Linux

1. Open a terminal and change directory to the installation path (where you unzipped the downloaded file).
`cd /path/to/shapeworks`

2. Create a protected conda environment that installs everything necessary to run.
`source install_shapeworks.sh`
  **Note:** You can pass a different name for the environment, enabling multiple installations.
  `source install_shapeworks.sh shapeworks_61`


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



=====================

Please contact us with any questions or ideas.

Website    sciinstitute.github.io/ShapeWorks
Email      shapeworks-users@sci.utah.edu
Discourse  shapeworks.discourse.group
Twitter    @ShapeWorksSuite

=====================
