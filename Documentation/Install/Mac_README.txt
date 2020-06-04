
=====================
Shapeworks
=====================

The ShapeWorks software is an open-source distribution of a new method for constructing compact 
statistical point-based models of ensembles of similar shapes that does not rely on any specific 
surface parameterization. The method requires very little preprocessing or parameter tuning, and 
is applicable to a wide range of shape analysis problems, including nonmanifold surfaces and 
objects of arbitrary topology. The proposed correspondence point optimization uses an entropy-based 
minimization that balances the simplicity of the model (compactness) with the accuracy of the 
surface representations. The ShapeWorks software includes tools for preprocessing data, computing 
point-based shape models, and visualizing the results.

=====================

You have downloaded a binary distribution of ShapeWorks.

To use the installer version of ShapeWorks (.pkg):

1. Install the ShapeWorks pkg file.

2. Open a terminal, change directory to /Applications/ShapeWorks

3. type "source conda_installs.sh"

4. Copy "/Applications/ShapeWorks/Examples to another location (e.g. $HOME/ShapeWorks-Examples)

5. CD to your copied Examples\Python location

6. Run "python3 RunUseCase.py --use_case ellipse"


Note: For subsequent usage, the shapeworks conda environment must be activated using:

conda activate shapeworks
