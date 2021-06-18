
=====================
Shapeworks
=====================

The ShapeWorks software is an open-source distribution of a new method for constructing compact statistical point-based models of ensembles of similar shapes that does not rely on any specific surface parameterization. The method requires very little preprocessing or parameter tuning and applies to a wide range of shape analysis problems, including nonmanifold surfaces and objects of arbitrary topology. The proposed correspondence point optimization uses an entropy-based 
minimization that balances the simplicity of the model (compactness) with the accuracy of the surface representations. The ShapeWorks software includes tools for preprocessing data, computing point-based shape models, and visualizing the results.

=====================

You have downloaded a binary distribution of ShapeWorks.

1. Install the ShapeWorks .pkg file by double-clicking on the file.


ShapeWorks comes with Python examples to get you started. To run them:

1. Open a terminal and change directory to the installation path: `cd /Applications/ShapeWorks`

2. type `source conda_installs.sh` to install necessary Python packages into a protected environment.

**Note:** You can also pass a different name for the installation environment: `source conda_installs.sh shapeworks_new`

3. Copy the Examples folder to another location (ex: `cp -r Examples $HOME/ShapeWorks-Examples`).

4. Activate the shapeworks conda environment: `conda activate shapeworks`

5. With the shapeworks conda environment activated, `cd` to your copied Examples\Python location.

6. Run `python RunUseCase.py --use_case <insert name of use case here>`. To list the use cases that are currently released, run: `python RunUseCase.py --help`.

**Note:** For subsequent usage, the shapeworks conda environment must be activated using `conda activate shapeworks`.


To see the interactive Jupyter notebook examples:

1. Open a terminal and change directory to your copied Examples\Python\notebooks\tutorials location.

2. Activate the shapeworks conda environment: `conda activate shapeworks`

3. Run `jupyter notebook` to open the notebooks in your web brower. Click on a notebook to get started.
