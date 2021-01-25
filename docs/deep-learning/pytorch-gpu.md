# PyTorch GPU Support for ShapeWorks

ShapeWorks deep learning tools, such as the `DeepSSMUtils` package, requires PyTorch with GPU support. 
This is installed to the `shapeworks` conda environment when `conda_installs.sh` is run. 

Currently `conda_installs.sh` installs the most recent stable release of PyTorch which can be found at [pytorch.org](https://pytorch.org/).  
When installing, `conda_installs.sh` checks which CUDA driver version is installed on the system and if it finds teh CUDA version is supported by the most recent 
PyTorch version, the PyTorch with GPU support is installed.
If an incompatible version of the CUDA driver is found, then `conda_installs.sh` installs CPU PyTorch instead. 

## Checking if PyTorch installation has GPU support
To check if your `shapeworks` environment has PyTorch with GPU support, run the following:
```
conda activate shapeworks
python
>>> import torch
>>> print(torch.cuda.is_available())
>>> exit()
```
If `torch.cuda.is_available()` is True then PyTorch has GPU support, otherwise the CPU version was installed. 
If `torch` cannot be imported than PyTorch was not installed to the `shapeworks` environment. 

## Reinstalling the Correct Pytorch Version
The following steps explain how to include a different PyTorch version in the `shapeworks` conda environment if you find that your system requires an older version 
of PyTorch or `conda_installs.sh` does not correcty find your CUDA version.

Deltailed instructions about the different ways to install PyTorch can be found here: [PyTorch Getting Started](https://pytorch.org/get-started/locally/).

1. If `conda_installs.sh` has already been run and the CPU version of PyTorch was installed, first that needs to be uninstalled. To uninstall run:
```
conda activate shapeworks
conda uninstall pytorch
```
2. Check which CUDA version is installed on your system using one of the methods explained here: [How to check CUDA version](https://varhowto.com/check-pytorch-cuda-version/)
3. Install the correct PyTorch to `shapeworks` environment using: 
```
conda install pytorch torchvision cudatoolkit=VERSION -c pytorch
```
  Where `VERSION` is your CUDA version (such as 11.0).

4. Restart your system and check if `shapeworks` now has PyTorch with GPU support using the instructions above. 
