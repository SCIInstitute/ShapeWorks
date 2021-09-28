# Shape Model for Multiple Domains directly from Mesh

## What is the Use Case?


This use case demonstrates using ShapeWorks tools to perform shape modelings for anatomies with multiple structures (domains), e.g., joints to capture inter-domains correlations and interactions directly on triangular surface meshes
There are three datasets available for this use case:

* ellipsoid_joint_rotation - two stacked ellipsoids with same radiis, with the top ellipsoid rotating w.r.t the bottom ellipsoid. 
* ellipsoid_joint_size - two stacked ellipsoids, with the top ellipsoid displaying varying radiis in all 3 directions
* ellipsoid_joint_size_rotation - two stacked ellipsoids, with the top ellipsoid displaying varying radii and rotating w.r.t the bottom ellipsoid

By defualt, the use case runs on the ellipsoid_joint_rotation dataset. The dataset can be changed by changing the name and output folder at the beginning of the Python script. 

## Grooming Data

In this use case, we download pre-groomed data. The use case will be updated soon to demonstrate the full mesh grooming process.

## Relevant Arguments
[--use_subsample](../use-cases.md#-use_subsample)
[--num_subsample](../use-cases.md#-use_subsample)
[--use_single_scale](../use-cases.md#-use_single_scale)
[--tiny_test](../use-cases.md#-tiny_test)

## Optimization Parameters
The python code for the use case calls the `optimize` command of ShapeWorks which requires the parameters of the optimization to be specified in a python dictionary. Please refer to [Parameter Dictionory in Python](../../workflow/optimize.md#parameter-dictionary-in-python) for more details.
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
Once the python code runs the optimization, the files are saved in the `Output` folder after which ShapeWorks Studio is launched from the python code to analyze the model. 
Here is the optimized shape.
![Ellipsoid Joint Shape Model](https://sci.utah.edu/~shapeworks/doc-resources/gifs/multiple_domain_segs_model.gif)

Below are the particles for each sample, we can see they are uniformly distributed.
![Ellipsoid Joint Samples](https://sci.utah.edu/~shapeworks/doc-resources/pngs/multi_domain_samples.png)