# Ellipsoid: Basic Example

## What and Where is the Use Case? 

The ellipsoid dataset comprises of axis-aligned ellipsoids with varying radii along a single axis. This example is a stepping stone for the user to get familiar with the workflow of ShapeWorks. 

The `ellipsoid.py` (in `Examples/Python/`) use case represents the standard use version of a shape modeling workflow using ShapeWorks. It includes the full pipeline for processed (i.e., prepped/groomed) as well as unprocessed data. 

The use case is located at: `Examples/Python/ellipsoid.py`

## Running the Use Case

To run the use case, run `RunUseCase.py` (in `Examples/Python/`) with proper tags. The tags control the type of input data and the optimization method. See [Getting Started with Use Cases](../use-cases/use-cases.md#running-use-case) for the full list of tags.

* `--start_with_prepped_data`: to run the optimization on previously processed/groomed data
* `--use_single_scale`: to use the single-scale optimization. Default is multi-scale optimization

To run the full pipeline with multi-scale:
            
```
$ cd /path/to/shapeworks/Examples/Python
$python RunUseCase.py --use_case ellipsoid 
```

This calls `ellipsoid.py` (in `Examples/Python/`) to perform the following.

* Loads the ellipsoid dataset using a local version if it exists (i.e., previously downloaded); otherwise, the dataset is automatically downloaded from the [ShapeWorks Data Portal](http://cibc1.sci.utah.edu:8080/).
* Grooms the segmentations by calling data preprocessing functions in `GroomUtils.py` (in `Examples/Python/`). See [Grooming Data](#grooming-data) for details about these preprocessing steps.
* Optimizes particle distribution (i.e., the shape/correspondence model) by calling optimization functions in `OptimizeUtils.py` (in `Examples/Python/`). See [Optimizing Shape Model](#optimizing-shape-model) for details about algorithmic parameters for optimizing the shape model.
* Launches ShapeWorksStudio to visualize the use case results (i.e., optimized shape model and the groomed data) by calling functions in `AnalyzeUtils.py` (in `Examples/Python/`).

If you wish to start with the optimization step using a previously groomed data, add `--start_with_prepped_data` tag.

```
$ python RunUseCase.py --use_case ellipsoid --start_with_prepped_data
```

## Grooming Data

The following preprocessing steps are only performed when you start with *unprepped* data, i.e., the tag `--start_with_prepped_data` is not used. For a description of the grooming tools and parameters, see: [How to Groom Your Dataset?](../workflow/groom.md).

1. **Isotropic Resampling**: Binary segmentations in `ellipsoid/segmentations/` are resampled to have an isotropic voxel spacing using a user-defined spacing. This step could also be used to produce segmentations with smaller voxel spacing, and thereby reduce aliasing artifacts (i.e., staircase/jagged surface) due to binarization.
2. **Apply Padding**: Segmentations that touch the image boundary will have an artificial hole at that intersection. Segmentations are padded by adding a user-defined number of voxels along each image direction (rows, cols, and slices) to avoid introducing artificial holes.
3. **Center-of-Mass Alignment**: This translational alignment step is performed before rigidly aligning the samples to a shape reference. This factors out translations to reduce the risk of misalignment and allow for a medoid sample to be automatically selected as the reference for rigid alignment.
4. **Reference Selection**: The reference is selected by first computing the mean (average) distance transform of the segmentations, then selecting the sample closest to that mean (i.e., medoid).
5. **Rigid Alignment**: All of the segmentations are then aligned to the selected reference using rigid alignment, which factors out the rotation and remaining translation.
6. **Cropping**: The segmentations are cropped so that all of the samples are within the same bounding box.
7. **Distance Transform**: Finally, the signed distance transform is computed, and the dataset is now ready for the optimize phase.

## Optimizing Shape Model

Below are the default optimization parameters for this use case. For a description of the optimize tool and its algorithmic parameters, see: [How to Optimize Your Shape Model](../workflow/optimize.md). Note that `use_shape_statistics_after` parameter is not used when `--use_single_scale` tag is given to the `RunUseCase.py` (in `Examples/Python/`)

```
        "number_of_particles": 128,
        "use_normals": 1,
        "normal_weight": 10.0,
        "checkpointing_interval": 200,
        "keep_checkpoints": 0,
        "iterations_per_split": 2000,
        "optimization_iterations": 1000,
        "starting_regularization": 100,
        "ending_regularization": 10,
        "recompute_regularization_interval": 2,
        "domains_per_shape": 1,
        "domain_type": 'image',
        "relative_weighting": 10,
        "initial_relative_weighting": 0.01,
        "procrustes_interval": 0,
        "procrustes_scaling": 0,
        "save_init_splits": 0,
        "debug_projection": 0,
        "verbosity": 2
        "use_shape_statistics_after": 32
```

## Analyzing Shape Model

ShapeWorksStudio visualizes/analyzes the optimized particle-based shape model by visualizing the mean shape, individual shape samples, and the shape modes of variations. For more information, see: [How to Analyze Your Shape Model?](../workflow/analyze.md). 