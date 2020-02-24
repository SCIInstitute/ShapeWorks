
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

To use:

1. Download and install the "Microsoft Visual C++ Redistributable for Visual Studio 2015, 2017 and 2019." :

https://aka.ms/vs/16/release/vc_redist.x64.exe

2. Download and install Anaconda for Windows:

https://repo.anaconda.com/archive/Anaconda3-2019.10-Windows-x86_64.exe

3. Copy "C:\Program Files\ShapeWorks\Examples" to another location

4. Open "Anaconda Prompt"

5. CD to "C:\Program Files\ShapeWorks"

6. Run "conda_installs.bat"

7. CD to your copied Examples\Python location

8. Run "python3 RunUseCase.py --use_case ellipse"

