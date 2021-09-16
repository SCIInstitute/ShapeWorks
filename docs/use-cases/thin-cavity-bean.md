# Thin Cavity Bean: Shape Model with Geodesic Distances

## What is the Use Case?

The `thin_cavity_bean` use case demonstrates using ShapeWorks tools to perform the following.

- Geodesic distance based repulsion for mesh domains
- Build a shape model where shapes are given as triangular surface meshes

Here is an example of the data:
![Thin cavity bean](../img/use-cases/thin_cavity_bean.png)

## Grooming Steps

In this use case, we download pre-groomed data.

## Supported Tags

``` 
        --use_subsample --num_subsample --use_single_scale --tiny_test
``` 
## Optimization Parameters

Below are the default optimization parameters when running this use case. 

```python
{
        "number_of_particles": 1024,
        "use_normals": 1,
        "normal_weight": 10.0,
        "checkpointing_interval": 200,
        "keep_checkpoints": 0,
        "iterations_per_split": 150,
        "optimization_iterations": 2500,
        "starting_regularization": 3000,
        "ending_regularization": 0.1,
        "recompute_regularization_interval": 1,
        "domains_per_shape": 1,
        "domain_type": 'mesh',
        "relative_weighting": 15,
        "initial_relative_weighting": 0.01,
        "procrustes_interval": 0,
        "procrustes_scaling": 0,
        "save_init_splits": 0,
        "verbosity": 3,
        "use_shape_statistics_after": 32,
        "geodesics_enabled": 1
}
```

Note `geodesics_enabled` is set to on.

## Analyzing Shape Model

Here we can see the resulting shape model correctly captures the moving cavity as the only mode of variation.

![Thin cavity bean analyze](https://sci.utah.edu/~shapeworks/doc-resources/pngs/thin_cavity_bean.gif)
