# Shape Model for Multiple Domains directly from Mesh

## What and Where is the Use Case?

This use case demonstrates using ShapeWorks tools to perform the following.

- Generate shape models for anatomies with multiple structures (domains), e.g., joints.
- Optimization is carried out in the given domains' shared/joint shape spaces to capture inter-domains correlations and interactions
- Build a shape model where shapes are given as triangular surface meshes


The use case is located at: `Examples/Python/ellipsoid_multiple_domain_mesh.py`


There are three datasets available for this use case:
- `ellipsoid_joint_rotation`
- `ellipsoid_joint_size`
- `ellipsoid_joint_size_rotation`
By defualt, the use case runs on the `ellipsoid_joint_rotation` dataset. The name of the dataset can be changed in the `ellipsoid_multiple_domain_mesh.py` file if required.
 
## Running the Use Case

To run the use case, run `RunUseCase.py` (in `Examples/Python/`).

```
$ cd /path/to/shapeworks/Examples/Python
$ python RunUseCase.py --use_case ellipsoid_multiple_domain_mesh
```

This calls `ellipsoid_multiple_domain_mesh.py` (in `Examples/Python/`) to perform the following.
 

* Loads the `ellipsoid joint` dataset using a local version if it exists (i.e., previously downloaded); otherwise, the dataset is automatically downloaded from the [ShapeWorks Data Portal](http://cibc1.sci.utah.edu:8080/).
* Optimizes particle distribution (i.e., the shape/correspondence model) by calling optimization functions in `OptimizeUtils.py` (in `Examples/Python/`). See [Optimizing Shape Model](#optimizing-shape-model) for details about algorithmic parameters for optimizing the shape model.
* Launches ShapeWorks Studio to visualize the use case results (i.e., the optimized shape model and the groomed data) by calling functions in `AnalyzeUtils.py` (in `Examples/Python/`).


## Grooming Data

In this use case, we download pre-groomed data. The use case will be updated soon to demonstrate the full mesh grooming process.

## Optimizing Shape Model

Below are the default optimization parameters when running this use case. For a description of the optimize tool and its algorithmic parameters, see: [How to Optimize Your Shape Model](../workflow/optimize.md).

```bash
$ python RunUseCase.py --use_case ellipsoid_multiple_domain_mesh
```
The list of `<inputs>` (surface mesh ) should be ordered consistently for each shape.(e.g., shape1-domain1, shape1-domain2, shape2-domain1, shape2-domain2 ... etc.).

```python
{
        "number_of_particles" : [512,512],
        "use_normals": [0,0],
        "normal_weight": [1.0,1.0],
        "checkpointing_interval" : 200,
        "keep_checkpoints" : 0,
        "iterations_per_split" : 500,
        "optimization_iterations" : 500,
        "starting_regularization" :100,
        "ending_regularization" : 0.5,
        "recompute_regularization_interval" : 2,
        "domains_per_shape" : 2,
        "domain_type" : 'mesh',
        "relative_weighting" : 1, 
        "initial_relative_weighting" : 0.1,
        "procrustes_interval" : 0,
        "procrustes_scaling" : 0,
        "save_init_splits" : 0,
        "verbosity" : 3

}
```

## Analyzing Shape Model

ShapeWorks Studio visualizes/analyzes the optimized particle-based shape model by visualizing the mean shape, individual shape samples, and the shape modes of variations. For more information, see: [How to Analyze Your Shape Model?](../workflow/analyze.md).            

