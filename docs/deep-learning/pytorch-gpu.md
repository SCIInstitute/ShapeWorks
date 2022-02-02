# PyTorch GPU Support for ShapeWorks

ShapeWorks deep learning tools, such as the `DeepSSMUtils` package, requires PyTorch with GPU support. 
This is installed with the rest of the ShapeWorks Anaconda environment using `install_shapeworks`. 
It selects the most recent stable release of PyTorch which can be found at [pytorch.org](https://pytorch.org/).  

When the Anaconda enironment is created using `install_shapeworks`, PyTorch with GPU support is installed if the system's current CUDA driver version is supported. Otherwise it selects the CPU version of PyTorch.

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

If you find that your system requires an older version of PyTorch or `install_shapeworks` did not correcty find your CUDA version, 
the following steps explain how to install a different PyTorch version in the `shapeworks` conda environment.

Deltailed instructions about the different ways to install PyTorch can be found here: [PyTorch Getting Started](https://pytorch.org/get-started/locally/).

1. If the CPU version of PyTorch was installed, that first needs to be uninstalled. To uninstall run:
```
conda activate shapeworks
pip uninstall torch torchvision torchaudio
```
2. Check which CUDA version is installed on your system using one of the methods explained here: [How to check CUDA version](https://varhowto.com/check-pytorch-cuda-version/)
3. Install the correct PyTorch to `shapeworks` environment using: 
```
pip install torch===1.7.1+cu<VERSION> torchvision===0.8.2+cu<VERSION> torchaudio===0.7.2 -f https://download.pytorch.org/whl/torch_stable.html
```
  Where `VERSION` is your CUDA version with no dot (such as 92 for 9.2 or 110 for 11.0).

4. Restart your system and check if `shapeworks` now has PyTorch with GPU support using the instructions above. 
