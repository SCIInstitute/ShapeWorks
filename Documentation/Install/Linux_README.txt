
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

There is currently one python example included.  To run it you will
need python with a few packages.  The easist way to install them is
to run

source ./conda_installs.sh

Then, to run the example:

cd Examples/Python
python RunUseCase.py --use_case ellipse

For subsequent usage, the shapeworks conda environment must be activated using:

conda activate shapeworks
