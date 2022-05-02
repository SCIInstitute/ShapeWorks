# Shape Model for Multiple Domains directly from Mesh

## What is the Use Case?


The `ellipsoid_multiple_domain_mesh` use case demonstrates using ShapeWorks tools to perform shape modelings for anatomies with multiple structures (domains), e.g., joints to capture inter-domains correlations and interactions directly on triangular surface meshes
The dataset available for this use case:

* ellipsoid_joint_rotation - two stacked ellipsoids with the same radii, with the top ellipsoid rotating w.r.t the bottom ellipsoid. 


## Grooming Data
This is how the meshes in the dataset look before grooming.Here it can been seen that the ellipsoids have random centers andn orientations.![This is how the joints in the dataset look before grooming.](https://sci.utah.edu/~shapeworks/doc-resources/pngs/ellipsoid_mesh_multi_pre_groom.png)

1. [**Remeshing**](../../workflow/groom.md#remesh): Meshes are remeshed to ensure uniform vertices.
2. [**Reference Selection**](../../workflow/groom.md#aligning-meshes): The reference is selected by first computing the mean (average) mesh, then selecting the sample closest to that mean (i.e., medoid). The reference is selected based on domain 1 shapes. 
3. [**Rigid Alignment**](../../workflow/groom.md#aligning-meshes): For all the shapes, the transformation is calculated to factor out translation and rotation based on the reference shape.This transformation matrix will be sent to the optimizer as a 'prefix transform'

Here we show how the shapes would look like if the transforms are applied.
![Meshes obtained after grooming](https://sci.utah.edu/~shapeworks/doc-resources/pngs/ellipsoid_mesh_multi_post_groom.png)

## Relevant Arguments
[--use_subsample](../use-cases.md#-use_subsample)
[--num_subsample](../use-cases.md#-use_subsample)
[--use_single_scale](../use-cases.md#-use_single_scale)
[--tiny_test](../use-cases.md#-tiny_test)

## Optimization Parameters
The python code for the use case calls the `optimize` command of ShapeWorks which reads the project sheet with the shape filenames and optimization parameter values. See [Project excel file](../../workflow/parameters.md#project-excel-file) for details regarding creating the project sheet.
Below are the default optimization parameters for this use case.

```python
{
        "checkpointing_interval" : 200,
        "keep_checkpoints" : 0,
        "iterations_per_split" : 200,
        "optimization_iterations" : 200,
        "starting_regularization" :1000,
        "ending_regularization" : 0.1,
        "recompute_regularization_interval" : 1,
        "domains_per_shape" : 2,
        "relative_weighting" : 10, 
        "initial_relative_weighting" : 0.1,
        "procrustes_interval" : 0,
        "procrustes_scaling" : 0,
        "save_init_splits" : 0,
        "verbosity" : 0

}
    num_particles = [128,128]
```

## Analyzing Shape Model
Once the python code runs the optimization, the files are saved in the `Output` folder after which ShapeWorks Studio is launched from the python code to analyze the model. 
Here is the optimized shape.
![Ellipsoid Joint Shape Model](https://sci.utah.edu/~shapeworks/doc-resources/gifs/ellipsoid_md_mesh_model.gif)

Below are the particles for each sample, we can see they are uniformly distributed.
![Ellipsoid Joint Samples](https://sci.utah.edu/~shapeworks/doc-resources/pngs/ellipsoid_md_mesh_samples.png)