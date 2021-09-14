# Shape Model for Multiple Domains directly from Mesh

## What is the Use Case?


This use case demonstrates using ShapeWorks tools to perform shape modelings for anatomies with multiple structures (domains), e.g., joints to capture inter-domains correlations and interactions directly on triangular surface meshes
There are three datasets available for this use case:

* ellipsoid_joint_rotation - two stacked ellipsoids with same radiis, with the top ellipsoid rotating w.r.t the bottom ellipsoid. 
* ellipsoid_joint_size - two stacked ellipsoids, with the top ellipsoid displaying varying radiis in all 3 directions
* ellipsoid_joint_size_rotation - two stacked ellipsoids, with the top ellipsoid displaying varying radii and rotating w.r.t the bottom ellipsoid

By defualt, the use case runs on the ellipsoid_joint_rotation dataset.

## Grooming Data

In this use case, we download pre-groomed data. The use case will be updated soon to demonstrate the full mesh grooming process.

## Supported Tags

``` 
        --use_subsample --num_subsample --use_single_scale --tiny_test
``` 
## Optimizing Shape Model

NOTE:The list of `<inputs>` (surface mesh ) should be ordered consistently for each shape.(e.g., shape1-domain1, shape1-domain2, shape2-domain1, shape2-domain2 ... etc.).

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


