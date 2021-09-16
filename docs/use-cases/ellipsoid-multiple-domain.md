# Shape Model for Multiple Domains 

## What is the Use Case?

This use case demonstrates using ShapeWorks tools to perform shape modelings for anatomies with multiple structures (domains), e.g., joints, to capture inter-domains correlations and interactions.
There are three datasets available for this use case:

* ellipsoid_joint_rotation - two stacked ellipsoids with the same radii, with the top ellipsoid rotating w.r.t the bottom ellipsoid. 
* ellipsoid_joint_size - two stacked ellipsoids, with the top ellipsoid displaying varying radii in all three directions
* ellipsoid_joint_size_rotation - two stacked ellipsoids, with the top ellipsoid displaying varying radii and rotating w.r.t the bottom ellipsoid

By default, the use case runs on the ellipsoid_joint_rotation dataset.
 
## Grooming Steps

1. **Isotropic Resampling**: Binary segmentations in the ellipsoid joint dataset are resampled with an isotropic voxel spacing.
2. **Reference Selection**: 
For the ellipsoid_joint datasets available on the ShapeWorks portal, the mode of variation is rotation and/or size of the second ellipsoid w.r.t to the first ellipsoid. Hence, we align the shapes using the first domain as the reference. This is domain-specific alignment. 
The reference is selected by first computing the mean (average) distance transform of the segmentations belonging to the reference domain, then selecting the sample closest to that mean (i.e., medoid).
3. **Rigid Alignment**: For all the shapes in the reference domain selected, the transformation is calculated to factor out translation and rotation. The same transformation is applied to the corresponding shape of the other domain.
4. **Bounding Box**: The smallest region which fits all of the samples is found.
5. **Cropping**: The segmentations are cropped to the size of the bounding box.
6. **Padding**: The segmentations are padded with zeros on every side.
7. **Distance Transform**: Finally, the smooth signed distance transform is computed, and the dataset is now ready for the optimize phase.

## Supported Tags

``` 
        --use_subsample --num_subsample --skip_grooming --use_single_scale --mesh_mode --tiny_test
``` 
## Optimization Parameters

NOTE: The list of `<inputs>` (binary segmentation images) should be ordered consistently for each shape.(e.g., shape1-domain1, shape1-domain2, shape2-domain1, shape2-domain2 ... etc.).

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
        "domain_type" : 'image',
        "relative_weighting" : 1, 
        "initial_relative_weighting" : 0.1,
        "procrustes_interval" : 0,
        "procrustes_scaling" : 0,
        "save_init_splits" : 0,
        "verbosity" : 3

      }
```

## Analyzing Shape Model

Here is the optimized shape.
![Ellipsoid Joint Shape Model](https://sci.utah.edu/~shapeworks/doc-resources/gifs/multiple_domain_segs_model.gif)

Below are the particles for each sample; we can see they are uniformly distributed.
![Ellipsoid Joint Samples](https://sci.utah.edu/~shapeworks/doc-resources/gifs/multi_domain_samples.png)