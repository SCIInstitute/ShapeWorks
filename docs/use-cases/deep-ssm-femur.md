# Deep-SSM: Demonstrating ShapeWorks Deep Learning on Femur Data

## What and Where is the Use Case? 

This use case demonstrates how to get shape models from unsegmented images using deep learning on the femur data. This includes performing data augmentation as well as building, training and testing a DeepSSM model. For a detailed description of these processes, please see [data-augmnetation.md](../deep-learning/data-augmentation.md)) and [deep-ssm.md](../deep-learning/deep-ssm.md)). The data used in this use case is the same as the femur use case ([femur.md](femur.md)) and the labels used in training are generated from running the femur use case. 

The use case is located at: [Examples/Python/deep-ssm.py](https://github.com/SCIInstitute/ShapeWorks/tree/master/Examples/Python/deep-ssm.py)

## Running the Use Case

To run the use case, run [RunUseCase.py](https://github.com/SCIInstitute/ShapeWorks/tree/master/Examples/Python/RunUseCase.py) with proper tags. The tags control the type of input data and the optimization method. See [Getting Started with Use Cases](../use-cases/use-cases.md#running-use-case) for the full list of tags. Note the following tags are not applicable to this use case:

* `--start_with_prepped_data`
* `--use_single_scale`
* `--interactive`
* `--start_with_image_and_segmentation_data`

This use case uses Pytorch and requires a GPU to run in a timely manner. When you run [conda_installs.sh](https://github.com/SCIInstitute/ShapeWorks/tree/master/conda_install.sh) it detects if you have a GPU and and installs the version of Pytorch compatible with your version of CUDA. Note we only support the three most recent versions of CUDA, if your GPU requires an older version of CUDA you will need to update the Pytorch install in your shapeworks conda environment to the correct CUDA version. For more information on doing so, see [pytorch.org](https://pytorch.org/).

To do a quick check to see if Pytorch is running on your GPU, you can run the use case with the `--tiny-test` tag. This will run the use case quickly on a few examples and print an error if it is not running on the GPU.

The following is performed by [deep-ssm.py](https://github.com/SCIInstitute/ShapeWorks/tree/master/Examples/Python/deep-ssm.py):

* Loads the femur dataset using a local version if it exists (i.e., previously downloaded), otherwise the dataset is automatically downloaded from the [ShapeWorks Data Portal](http://cibc1.sci.utah.edu:8080/). This includes the particle files created from running the femur use case. 
* Performs data augmentation as described in [data-augmnetation.md](../deep-learning/data-augmentation.md))
* Creates a DeepSSM model as described in [deep-ssm.md](../deep-learning/deep-ssm.md)) and uses it to make predictions on unseen images.
