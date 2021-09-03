# Ellipsoid: Basic Example

## What is the Use Case? 

The ellipsoid dataset comprises of axis-aligned ellipsoids with varying radii along all three axes. This example is a stepping stone for the user to get familiar with the workflow of ShapeWorks. 

The `ellipsoid.py` use case represents the standard use version of a shape modeling workflow using ShapeWorks. 


## Grooming Steps

1. **Isotropic Resampling**: Binary segmentations in `ellipsoid/segmentations/` are resampled to have an isotropic voxel spacing.
2. **Center-of-Mass Alignment**: This translational alignment step is performed before rigidly aligning the samples to a shape reference. This factors out translations to reduce the risk of misalignment and allow for a medoid sample to be automatically selected as the reference for rigid alignment.
3. **Reference Selection**: The reference is selected by first computing the mean (average) distance transform of the segmentations, then selecting the sample closest to that mean (i.e., medoid).
4. **Rigid Alignment**: All of the segmentations are then aligned to the selected reference using rigid alignment, which factors out the rotation and remaining translation.
5. **Bounding Box**: The smallest region which fits all of the samples is found.
6. **Cropping**: The segmentations are cropped to the size of the bounding box.
7. **Padding**: The segmentations are padded with zeros on every side.
8. **Distance Transform**: Finally, the smooth signed distance transform is computed, and the dataset is now ready for the optimize phase.

## Supported Tags

``` 
        --use_subsample --num_subsample --skip_grooming --use_single_scale --tiny_test
``` 
## Optimization Parameters

Below are the default optimization parameters for this use case.

```
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
        "verbosity": 1
```

## Analyzing Shape Model

ToDO:
1. Add GIF of the shape model : all dominant modes with the shape statistics plots
2. Add observations regarding the model like : mode of variations, uniform distribution
