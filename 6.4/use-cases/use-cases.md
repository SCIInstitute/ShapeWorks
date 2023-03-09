# Getting Started with Use Cases

## What is a Use Case?
Use cases are Python examples that can help users get familiar with ShapeWorks and the general shape modeling workflow. The full dataset associated with each use case (input and output) is available on [ShapeWorks Data Portal](https://girder.shapeworks-cloud.org/) and downloads automatically when the use case runs.

Most use cases demomstrates the [Shape Modeling Workflow](../getting-started/workflow.md):

1. [Grooming](../workflow/groom.md) a dataset for optimization.
2. [Optimizing](../workflow/optimize.md) a correpsondence model on the groomed data.
3. [Analyzing](../workflow/analyze.md) the correspondence model.

However, some do not involve grooming, and some only demonstrate the functionality of ShapeWorks statistical tools. For an overview and brief description of the available use cases, see [Examples](../getting-started/examples.md).

## Running A Use Case

The use cases are located at: `Examples/Python/`. To run a use case, run the following commands with the `shapeworks` environment activated:
```
$ cd /path/to/shapeworks/Examples/Python
$ python RunUseCase.py [use case name]
```

To see the names currently supported use cases and the complete list of optional arguments, run:
```
$ python RunUseCase.py --help
```

!!! note "Using **bash** on Windows"
    Windows also has several Unix shells such as **git-bash** available. These also work with `conda activate shapeworks` in the same way. However, there is one crucial issue: running Python scripts requires prefixing with `winpty`. For example, `winpty python RunUseCase.py ...`.
When a use case is run, the dataset required for the use case is automatically downloaded. This requires registering for a *free* ShapeWorks account by visiting the [ShapeWorks Data Portal](https://girder.shapeworks-cloud.org/).

!!! danger
    Do not use the same password as for your bank account or email.

After registering a free account, you can log in from within the script. 
Note: You are only required to enter your credentials the first time you run a use case. 

### Uploading a Dataset

Uploading a dataset requires the following parameters:

1. dataset_name - The name of the dataset and the same name must be used while running the usecase.

2. licence_filename - File location which contains licence information of the dataset

3. ack_filename - File location which contains acknowledge information of the dataset

4. description - Description of the dataset

5. project_file - File location which contains the project file of dataset using by the usecase which needs to be uploaded. (must be in swproj extension)

6. overwrite - This is a boolean variable which controls any existing dataset with same name should be deleted or not. if True is passed it will delete the existing dataset. The default value is True.

<!-- --overwrite - which controls any existing dataset with same name should be deleted or not. if --overwrite is specified it will delete the existing dataset. -->

For example refer the following command
```
$ import shapeworks as sw
$ sw.upload_dataset(dataset_name, licence_filename, acknowledgement_filename, description, project_file, overwrite)
```

### Downloading a Dataset

Downloading a Dataset requires the following parameters:

1. datasetName - The name of the dataset which needs to be downloaded.

2. outputDirectory - The location where the dataset should be downloaded.

For example refer the following command
```
$ python portal.py [dataset name] [output directory]
```

### Use Case Data

When a use case runs, the appropriate zipped data downloads to `Examples/Python/Data/`. The data is then extracted to `Examples/Python/output/use_case_name/` where all the output from running the use case is also saved.

The downloaded data includes the raw input data (segmentations, meshes, and/or images) as well as the expected output from both the final grooming step of the use case (i.e., distance transforms) and the optimized shape model (particle files and project excel sheets). 

An "project.swproj" file is included so that the resulting shape model can be visualized in Studio without running the use case.
For example, to view the shape model downloaded for the ellipsoid use case run:
```
$ cd Examples/Python/Output/ellipsoid/ellipsoid_1mode/
$ cp shape_model ../
$ ShapeWorksStudio ellipsoid_multiscale.xlsx or
$ ShapeWorksStudio ellipsoid_multiscale.swproj
```

### Use Cases Arguments

When calling `RunUseCase.py`, various optional arguments can be used. 
To see the complete list of supported arguments from the command line, run:
```
$ python RunUseCase.py --help
```
A description of each optional argument and how to use it is provided below.

#### --use_subsample
Users can run a use case on a subset of the data by adding the `--use_subsample` argument followed by the `--num_subsample` argument with the number of samples you wish to use. This will select a representative subset of the specified sample size to run through the pipeline so that the use case runs faster and uses less memory. The subset is determined by running clustering, then picking one sample from each cluster so that the resulting subset is representative of all the data. If `--use_subsample` is used without `--num_subsample`, it will use the default number of subsamples which is 3. Note the entire dataset will still be downloaded to run the clustering.

```
$ python RunUseCase.py [use case name] --use_subsample --num_subsample 10
```
      
#### --use_single_scale
Use cases can be run with multi-scale or single-scale optimization. In both cases, particles on each shape sample are initialized using the particle splitting strategy starting from a single particle (or a given set of landmarks) until reaching the required number of particles. The optimized particles at each scale are used to initialize the next scale. At each scale, particles undergo *initialization* and *optimization* stages. The multi-scale argument triggers both the initialization and optimization stages. The single-scale mode uses the initialization stage at each scale and runs the optimization stage when the required number of particles is reached (i.e., at the last scale). 

The differences between *initialization* and *optimization* stages are: 

1. How important the correspondence (inter-surface) objective is compared to the surface sampling (intra-surface) term using a relative weighting parameter. Hence, the  `initial_relative_weighting` parameter is the weight used in the initialization stage, and the `relative_weighting` is the weight used for the optimization stage.

2. How the notion of correspondence (inter-surface) is quantified. In initialization, especially when we do not have enough particles to describe the geometry of each surface, we use mean energy (i.e., pushing all shapes in the shape space to the mean shape or, in other words, the covariance matrix is assumed to be identity). In optimization, we use the entropy of the distribution of the shapes (assuming Gaussian-distributed shapes), which is quantified based on the covariance matrix. In the multi-scale setting, we can use shape statistics at later scales using the `use_shape_statistics_after` parameter, where we have more particles that can reveal the covariance structure.The default mode is multi-scale. To run single-scale, use the `--use_single_scale` argument.

```
$ python RunUseCase.py [use case name] --use_single_scale
```

#### --mesh_mode
Use cases that optimize on distance transforms can be run in mesh mode to optimize on meshes instead. When the `--mesh_mode` argument is used, groomed distance transforms are converted to meshes (by extracting the isosurface at zero), and those meshes are passed to the optimizer. The `--mesh_mode` argument does not affect mesh-based use cases.

```
$ python RunUseCase.py [insert name of image-based use case here] --mesh_mode
```

#### --tiny_test
Users can run a fast version of the use case using the `--tiny_test` argument. This runs on a subset of the data for fewer optimization iterations to verify ShapeWorks has been properly installed. It is meant to quickly test that use cases can run, not to create a good correspondence model. Only a subset of the data will be downloaded directly in the `Output` folder for the tiny test, and no zip file will be downloaded in the `Data` folder. No new data will be downloaded if the data has already been unzipped in the `Output` folder.

```
$ python RunUseCase.py [use case name] --tiny_test
```

#### --verify


## Use Case Workflow Overview

The use cases which demomstrate the [Shape Modeling Workflow](../getting-started/workflow.md) follow this general outline:

### [Grooming](../workflow/groom.md)
Grooming involves pre-processing steps to prepare the data for optimization and calculate the alignment transforms which will be passed to the optimizer. This involves generating distance transforms if starting with binary segmentations or generating groomed meshes if starting with unaligned meshes and calculating the alignment transformation matrix for each shape. The grooming steps are unique to each use case, but common steps are explained here: [Common Grooming Steps](../workflow/groom.md#Common-Pre-Processing-Steps-for-Segmentations).

Note some use cases start with pre-aligned data that does not require grooming. 
!!! important "**Prefix Transforms**" 
    **Worflow Switched from Transforming the Data (e.g. centering) to Passing the Transform as the Prefix Transform.**

    Instead of passing groomed and aligned shapes to the optimizer, now, the use cases only perform the grooming steps that do not change the co-ordinates of the shape. For the grooming steps that alter the co-ordinates, the transformation matrices are calculated and sent to the optimizer to be applied(pre-multiplied) after optimization. See [How to Step Up Optimization?](../workflow/parameters.md#project-excel-file) page for details regarding setting up the project sheet.

### [Optimization](../workflow/optimize.md)
Optimization involves automatically computing a dense set of corresponding landmark positions from the groomed shape representations (distance transforms or meshes). Optimization can be run with different parameters in ShapeWorksStudio or via the command line. In the use cases, optimization parameters are defined in the optimize sheet of the project.xlsx or project.swproj and optimization is run from the command line. 

For a full decription of parameters, please see: [optimization parameters](../workflow/parameters.md). 

### [Analysis of Results](../workflow/analyze.md)
The resulting correspondence points from optimization can be viewed and analyzed in **ShapeWorks Studio**. This is a Qt and VTK-based graphical user interface (GUI), that allows visualizing the correspondence model for each shape sample where particle coloring is used to reflect correspondence among shapes. After optimization, the use cases create and open an `project.xlsx` or `project.swproj` file for viewing the results in Studio. Studio opens automatically when the use case finishes running. 

<p><video src="https://sci.utah.edu/~shapeworks/doc-resources/mp4s/studio_scroll.mp4" autoplay muted loop controls style="width:100%"></p>

This `project.xlsx` or `project.swproj` file can be reopened at any time after running the use case for subsequent analysis by calling `ShapeworksStudio project.xlsx` or `ShapeworksStudio project.swproj`. For a full description of how to analyze results in Studio, see [Analyzing Results](../workflow/analyze.md).