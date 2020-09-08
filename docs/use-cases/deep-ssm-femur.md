# Deep-SSM: ShapeWorks Deep Learning on Femur Data

## What and Where is the Use Case? 

This use case demonstrates how to get shape models from unsegmented images using deep learning on the femur data. This includes performing data augmentation as well as building, training and testing a DeepSSM model. For a detailed description of these processes, please see [data-augmnetation.md](../deep-learning/data-augmentation.md) and [deep-ssm.md](../deep-learning/deep-ssm.md). The data used in this use case is the same as the femur use case ([femur.md](femur.md)) and the labels used in training are generated from running the femur use case. 

The use case is located at: [Examples/Python/deep-ssm.py](https://github.com/SCIInstitute/ShapeWorks/tree/master/Examples/Python/deep-ssm.py)

## Running the Use Case

To run the use case, run [RunUseCase.py](https://github.com/SCIInstitute/ShapeWorks/tree/master/Examples/Python/RunUseCase.py) with proper tags. 

```
$ cd /path/to/shapeworks/Examples/Python
$ python RunUseCase.py --use_case deep_ssm
```

See [Getting Started with Use Cases](../use-cases/use-cases.md#running-use-case) for the full list of tags. Note the following tags are not applicable to this use case:

* `--start_with_prepped_data`
* `--use_single_scale`
* `--interactive`
* `--start_with_image_and_segmentation_data`

**Important note:** This use case uses Pytorch and requires a GPU to run in a timely manner. When you run [conda_installs.sh](https://github.com/SCIInstitute/ShapeWorks/tree/master/conda_install.sh) it detects if you have a GPU and and installs the version of Pytorch compatible with your version of CUDA. Note we only support the three most recent versions of CUDA, if your GPU requires an older version of CUDA you will need to update the Pytorch install in your shapeworks conda environment to the correct CUDA version. For more information on doing so, see [pytorch.org](https://pytorch.org/). To do a quick check to see if Pytorch is running on your GPU, you can run the use case with the `--tiny-test` tag. This will run the use case quickly on a few examples and print an error if it is not running on the GPU.

The following is performed by [deep-ssm.py](https://github.com/SCIInstitute/ShapeWorks/tree/master/Examples/Python/deep-ssm.py):

* Loads the femur dataset using a local version if it exists (i.e., previously downloaded), otherwise the dataset is automatically downloaded from the [ShapeWorks Data Portal](http://cibc1.sci.utah.edu:8080/). This includes the particle files created from running the femur use case. 
* Performs data augmentation as described in [data-augmentation.md](../deep-learning/data-augmentation.md).
* Creates a DeepSSM model as described in [deep-ssm.md](../deep-learning/deep-ssm.md) and uses it to make predictions on unseen images.

## Use Case Pipeline

### Step 1: Getting the original data
The femur data is downloaded from Girder. This use case uses the original unsegmented images and the corresponding .particle shape model files. Of the 50 examples in the femur dataset, 40 are used to create training and validation sets while the remaining 10 are saved for a test set.

### Step 2: Running data augmentation
For a full description of the data augmentation process and how to use the ShapeWorks data augmentation Python package, please see [data-augmentation.md](../deep-learning/data-augmentation.md). The functions relevant to this step are `RunDataAugmentation` and `VisualizeAugmentation`.

Data augmentation is run using the images and particle files allocated for training and validation. 4960 augmented samples are created so that DeepSSM can be trained on 5000 total examples. The data is embedded to 6 dimensions using PCA, preserving 95% of the population variation. A KDE distribution is fit to the embedded data and used in sampling and the real and augmented results are visualized in a scatterplot matrix.

### Step 3: Creating torch loaders
For a full description of the DeepSSM process and how to use the ShapeWorks DeepSSM Python package, please see  [deep-ssm.md](../deep-learning/deep-ssm.md). The functions relevant to this step are `getTrainValLoaders` and `getTestLoader`.

The images and particle files are reformatted into tensors for training and testing the DeepSSM network. The 5000 original and augmented image/particle pairs are turned into train (80%) and validation (20%) loaders and the images held out for the test set are turned into a test loader. A batch size of 8 is used for optimal GPU capacity. Note if a CUDA memory error occurs when running the use case, this value may need to be decreased. The images in the train, validation, and test sets are downsampled to 75% of their original size to decrease training time.

### Step 4: Training DeepSSM 
This step uses function `trainDeepSSM` documented in [deep-ssm.md](../deep-learning/deep-ssm.md). A DeepSSM model is created and trained for 30 epochs. A learning rate of 0.0001 is used and the validation error is calculated and reported every epoch.

### Step 5: Testing DeepSSM
This step uses function `testDeepSSM` documented in [deep-ssm.md](../deep-learning/deep-ssm.md). The trained DeepSSM model is used to predict the PCA scores of the unseen images in the test loader. These scores are then mapped to the particle shape model using the PCA information from data augmentation and the predcited particles are saved.

### Step 6: Analyze DeepSSM Results
This step uses function `AnalyzeResults` documented in [deep-ssm.md](../deep-learning/deep-ssm.md). The DeepSSM predictions are analyzed by considering the surface-to-surface distance between the mesh generated from the original segmentation and the mesh generated from the predicted particles. Heat maps of these distances on the meshes are saved to visualize the results.






