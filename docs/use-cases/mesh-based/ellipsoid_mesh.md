# Ellipsoid Mesh: Basic Example

## What is the Use Case? 

This use case uses the same dataset as the `ellipsoid` use case, but optimization is done on meshes rather than distance transforms. The dataset comprises of axis-aligned ellipsoids with varying radii along the x-axis. The `ellipsoid_mesh` use case represents the standard use version of a shape modeling workflow from meshes using ShapeWorks. 


## Grooming Steps

This use case starts with pre-aligned meshes. 

## Supported Tags

``` 
        --use_subsample --num_subsample --use_single_scale --tiny_test
``` 
## Optimization Parameters

Below are the default optimization parameters for this use case.

```
        "number_of_particles": 128,
        "use_normals": 0,
        "normal_weight": 10.0,
        "checkpointing_interval": 200,
        "keep_checkpoints": 0,
        "iterations_per_split": 500,
        "optimization_iterations": 500,
        "starting_regularization": 100,
        "ending_regularization": 0.1,
        "recompute_regularization_interval": 2,
        "domains_per_shape": 1,
        "domain_type": 'mesh',
        "relative_weighting": 10,
        "initial_relative_weighting": 0.01,
        "procrustes_interval": 0,
        "procrustes_scaling": 0,
        "save_init_splits": 0,
        "verbosity": 1
```

## Analyzing Shape Model

Below is the mean shape reulting from optimization. Here we can see that there is only one major mode of variation.
![Ellipsoid Mesh Optimization](../img/use-cases/ellipsoid_mesh.png)

Animating along the first PCA mode we can see the variation in the radius along the x-axis.
![Ellipsoid Mesh Mode 1](https://sci.utah.edu/~shapeworks/doc-resources/gifs/ellipsoid_mesh_mode1.gif)

