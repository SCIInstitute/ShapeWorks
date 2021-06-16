# Ellipsoid: Cutting Planes

## What and Where is the Use Case? 

This example demonstrates using multiple cutting planes to constrain the distribution of particles on ellipsoids which are already aligned. This can be used in modeling scenarios where statistical modeling/analysis is needed for a region-of-interest on the anatomy/object-class at hand without having to affect the input data. 

*Ellipsoids with 2 cutting planes*
<p><video src="https://sci.utah.edu/~shapeworks/doc-resources/mp4s/ellipsoid_cut_trim.mp4" autoplay muted loop controls style="width:100%"></p>


The `ellipsoid_cut.py` (in `Examples/Python/`) use case represents the standard use version of a shape modeling workflow that entails one or more cutting planes using ShapeWorks. It includes the full pipeline for processed (i.e., groomed) as well as unprocessed data. 

The use case is located at: `Examples/Python/ellipsoid_cut.py`

## Running the Use Case

To run the use case, run `RunUseCase.py` (in `Examples/Python/`) with proper tags. The tags control the type of input data and the optimization method. See [Getting Started with Use Cases](../use-cases/use-cases.md#running-use-case) for the full list of tags.

* `--use_single_scale`: to use the single-scale optimization. Default is multi-scale optimization

To run the full pipeline with multi-scale:
            
```python
$ cd /path/to/shapeworks/Examples/Python
$python RunUseCase.py --use_case ellipsoid_cut 
```

This calls `ellipsoid_cut.py` (in `Examples/Python/`) to perform the following.

* Loads the ellipsoid dataset using a local version if it exists (i.e., previously downloaded); otherwise, the dataset is automatically downloaded from the [ShapeWorks Data Portal](http://cibc1.sci.utah.edu:8080/).
* Creates distance transforms from the aleardy aligned segmentations.
* Defines two cutting planes to be used to constrain the particle optimization on all ellipsoid. Note that this dataset contains a set of roughly aligned ellispoids; hence a common set of cutting planes can be used for all samples. 
```
    cutting_plane_points1 = [[10, 10, 0], [-10, -10, 0], [10, -10, 0]]
    cutting_plane_points2 = [[10, -3, 10], [-10, -3 ,10], [10, -3, -10]]
    cp = [cutting_plane_points1, cutting_plane_points2]
```
* Optimizes particle distribution (i.e., the shape/correspondence model) by calling optimization functions in `OptimizeUtils.py` (in `Examples/Python/`). See [Optimizing Shape Model](#optimizing-shape-model) for details about algorithmic parameters for optimizing the shape model. 
* Launches ShapeWorksStudio to visualize the use case results (i.e., optimized shape model and the groomed data) by calling functions in `AnalyzeUtils.py` (in `Examples/Python/`).

If you wish to start with the optimization step using a previously groomed data, add `--start_with_prepped_data` tag.

```
$ python RunUseCase.py --use_case ellipsoid_cut --start_with_prepped_data
```

## Grooming Data

The segmentations used in this use case are already aligned so the only grooming step neccesary is converting them to distance transforms. For a description of the grooming tools and parameters, see: [How to Groom Your Dataset?](../workflow/groom.md).

## Optimizing Shape Model

Below are the default optimization parameters for this use case. For a description of the optimize tool and its algorithmic parameters, see: [How to Optimize Your Shape Model](../workflow/optimize.md). Note that `use_shape_statistics_after` parameter is not used when `--use_single_scale` tag is given to the `RunUseCase.py` (in `Examples/Python/`). Also note the use of `adaptivity_mode`, `cutting_plane_counts`, and `cutting_planes` optimization parameters to trigger the constrained particles optimization.

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
        "verbosity": 2,
        "adaptivity_mode": 0,
        "cutting_plane_counts": cutting_plane_counts,
        "cutting_planes": cutting_planes
```

## Analyzing Shape Model

ShapeWorksStudio visualizes/analyzes the optimized particle-based shape model by visualizing the mean shape, individual shape samples, and the shape modes of variations. For more information, see: [How to Analyze Your Shape Model?](../workflow/analyze.md). 

<p><video src="https://sci.utah.edu/~shapeworks/doc-resources/mp4s/ellipsoid_cut_studio.mp4" autoplay muted loop controls style="width:100%"></p>


