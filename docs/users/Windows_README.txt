
=====================
Shapeworks
=====================

The ShapeWorks software is an open-source distribution of a new method for constructing compact statistical point-based models of ensembles of similar shapes that does not rely on any specific surface parameterization. The method requires very little preprocessing or parameter tuning and applies to a wide range of shape analysis problems, including nonmanifold surfaces and objects of arbitrary topology. The proposed correspondence point optimization uses an entropy-based 
minimization that balances the simplicity of the model (compactness) with the accuracy of the surface representations. The ShapeWorks software includes tools for preprocessing data, computing point-based shape models, and visualizing the results.

=====================

You have downloaded a binary distribution of ShapeWorks.

Installation instructions:

1. Download and install the "Microsoft Visual C++ Redistributable for
Visual Studio 2015, 2017 and 2019." :

https://aka.ms/vs/16/release/vc_redist.x64.exe

2. Download and install Miniconda for Windows:

https://repo.anaconda.com/miniconda/Miniconda3-latest-Windows-x86_64.exe


ShapeWorks comes with Python examples to get you started. To run them:

1. Copy "C:\Program Files\ShapeWorks\Examples" to another location

2. Open "Anaconda Prompt"

3. CD to "C:\Program Files\ShapeWorks"

4. Run "conda_installs.bat"

   **Note:** If you already have anaconda/miniconda installed, this step may hang.  If this step hangs, please uninstall anaconda/miniconda and re-install it and then run conda_installs.bat.

5. Activate the shapeworks conda environment: "conda activate shapeworks"

6. In Anaconda Prompt with the shapeworks environment activated, cd to your copied Examples\Python location

7. Run "python RunUseCase.py --use_case <insert name of use case here>". For names for the use cases that are currently released, run: "python RunUseCase.py --help"

Note: If you have installed ShapeWorks in a location different than
"C:\Program Files\ShapeWorks", please add the path to the "bin"
directory when calling RunUseCase.py so that the path will be set correctly.
Example: "python RunUseCase.py --use_case ellipsoid --tiny_test --shapeworks_path D:\ShapeWorks\bin"

**Note:** For subsequent usage, the shapeworks conda environment must be activated using "conda activate shapeworks"


To see the interactive Jupyter notebook examples:

1. In Anaconda Promt with shapeworks environment activated, cd to your copied Examples\Python\notebooks\tutorials location

2. Run "jupyter notebook" to open the notebooks in your web brower.

