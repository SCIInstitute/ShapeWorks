# Ellipsoid: Basic Example

## What is the Use Case? 

The ellipsoid dataset comprises of axis-aligned ellipsoids with varying radii along all the x-axis. This example is a stepping stone for the user to get familiar with the workflow of ShapeWorks. 

The `ellipsoid` use case represents the standard use version of a shape modeling workflow using ShapeWorks. 


## Grooming Steps

This is how the segmentations in the dataset look before grooming.Here it can been seen that the ellipsoids have random centers andn orientations.![This is how the segmentations in the dataset look before grooming.](https://sci.utah.edu/~shapeworks/doc-resources/pngs/ellipsoid_pre_groom.png)

1. [**Isotropic Resampling**](../../workflow/groom.md#resampling-images-and-segmentations): Binary segmentations in `ellipsoid/segmentations/` are resampled to have an isotropic voxel spacing.
2. [**Center-of-Mass Alignment**](../../workflow/groom.md#aligning-segmentations): This translational alignment step is performed before rigidly aligning the samples to a shape reference. This factors out translations to reduce the risk of misalignment and allow for a medoid sample to be automatically selected as the reference for rigid alignment.
3. [**Reference Selection**](../../workflow/groom.md#aligning-segmentations): The reference is selected by first computing the mean (average) distance transform of the segmentations, then selecting the sample closest to that mean (i.e., medoid).
4. [**Rigid Alignment**](../../workflow/groom.md#aligning-segmentations): All of the segmentations are then aligned to the selected reference using rigid alignment, which factors out the rotation and remaining translation.
5. [**Bounding Box**](../../workflow/groom.md#cropping-and-padding-segmentations): The smallest region which fits all of the samples is found.
6. [**Cropping**](../../workflow/groom.md#cropping-and-padding-segmentations): The segmentations are cropped to the size of the bounding box.
7. [**Padding**](../../workflow/groom.md#cropping-and-padding-segmentations): The segmentations are padded with zeros on every side.
8. [**Distance Transform**](../../workflow/groom.md#converting-segmentations-to-smooth-signed-distance-transforms): Finally, the smooth signed distance transform is computed, and the dataset is now ready for the optimize phase.

Distance transform obtained after grooming.The ellipsoids are now centred and aligned ready to be sent to the optimizer![Distance transform obtained after grooming](https://sci.utah.edu/~shapeworks/doc-resources/pngs/ellipsoid_post_groom.png)

## Relevant Arguments
[--use_subsample](../use-cases.md#-use_subsample)
[--num_subsample](../use-cases.md#-use_subsample)
[--skip_grooming](../use-cases.md#-skip_grooming)
[--use_single_scale](../use-cases.md#-use_single_scale)
[--mesh_mode](../use-cases.md#-mesh_mode)
[--tiny_test](../use-cases.md#-tiny_test)

## Optimization Parameters
The python code for the use case calls the `optimize` command of ShapeWorks which requires the parameters of the optimization to be specified in a python dictionary. Please refer to [Parameter Dictionory in Python](../../workflow/optimize.md#parameter-dictionary-in-python) for more details. 
Below are the default optimization parameters for this use case.

```python
{
        "number_of_particles": 128,
        "use_normals": 0,
        "normal_weight": 10.0,
        "checkpointing_interval": 1000,
        "keep_checkpoints": 0,
        "iterations_per_split": 1000,
        "optimization_iterations": 1000,
        "starting_regularization": 10,
        "ending_regularization": 1,
        "recompute_regularization_interval": 1,
        "domains_per_shape": 1,
        "domain_type": 'image',
        "relative_weighting": 1,
        "initial_relative_weighting": 0.05,
        "procrustes_interval": 0,
        "procrustes_scaling": 0,
        "save_init_splits": 0,
        "verbosity": 0
}
```

## Analyzing Shape Model
Once the python code runs the optimization, the files are saved in the `Output` folder after which ShapeWorks Studio is launched from the python code to analyze the model. 

Below is the mean shape reulting from optimization. Here we can see that there is only one major mode of variation.
![Ellipsoid Optimization](../../img/use-cases/ellipsoid_mean.png)

Below are the particles for each sample, we can see they are uniformly distributed.
![Ellipsoid Samples](../../img/use-cases/ellipsoids_samples.png)

Animating along the first PCA mode we can see the variation in the radius along the x-axis.
![Ellipsoid Mode 1](https://sci.utah.edu/~shapeworks/doc-resources/gifs/ellipsoid_mode1.gif)
