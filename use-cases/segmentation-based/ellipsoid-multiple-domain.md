# Shape Model for Multiple Domains from Segmentations

## What is the Use Case?

The `ellipsoid_multiple_domain` use case demonstrates using ShapeWorks tools to perform shape modelings for anatomies with multiple structures (domains), e.g., joints, to capture inter-domains correlations and interactions.
There are three datasets available for this use case:

* ellipsoid_joint_rotation - two stacked ellipsoids with the same radii, with the top ellipsoid rotating w.r.t the bottom ellipsoid. 
* ellipsoid_joint_size - two stacked ellipsoids, with the top ellipsoid displaying varying radii in all three directions
* ellipsoid_joint_size_rotation - two stacked ellipsoids, with the top ellipsoid displaying varying radii and rotating w.r.t the bottom ellipsoid

By default, the use case runs on the ellipsoid_joint_rotation dataset. The dataset can be changed by changing the name and output folder at the beginning of the Python script. 
 
## Grooming Steps
This is how the segmentations in the dataset look before grooming.Here it can been seen that the ellipsoids have random centers andn orientations.![This is how the segmentations in the dataset look before grooming.](https://sci.utah.edu/~shapeworks/doc-resources/pngs/ellipsoid_multi_pre_groom.png)

1. [**Isotropic Resampling**](../../workflow/groom.md#resampling-images-and-segmentations): Binary segmentations in the ellipsoid joint dataset are resampled with an isotropic voxel spacing.
2. [**Reference Selection**](../../workflow/groom.md#aligning-segmentations): 
For the ellipsoid_joint datasets available on the ShapeWorks portal, the mode of variation is rotation and/or size of the second ellipsoid w.r.t to the first ellipsoid. Hence, we align the shapes using the first domain as the reference. This is domain-specific alignment. 
The reference is selected by first computing the mean (average) distance transform of the segmentations belonging to the reference domain, then selecting the sample closest to that mean (i.e., medoid).
3. [**Rigid Alignment**](../../workflow/groom.md#aligning-segmentations): For all the shapes in the reference domain selected, the transformation is calculated to factor out translation and rotation. The same transformation is applied to the corresponding shape of the other domain.
4.[**Bounding Box**](../../workflow/groom.md#cropping-and-padding-segmentations):  The smallest region which fits all of the samples is found.
5. [**Cropping**](../../workflow/groom.md#cropping-and-padding-segmentations): The segmentations are cropped to the size of the bounding box.
6. [**Padding**](../../workflow/groom.md#cropping-and-padding-segmentations): The segmentations are padded with zeros on every side.
7. [**Distance Transform**](../../workflow/groom.md#converting-segmentations-to-smooth-signed-distance-transforms): Finally, the smooth signed distance transform is computed, and the dataset is now ready for the optimize phase.

Distance transform obtained after grooming.The ellipsoid joints are now centred and aligned ready to be sent to the optimizer![Distance transform obtained after grooming](https://sci.utah.edu/~shapeworks/doc-resources/pngs/ellipsoid_multi_post_groom.png)

## Relevant Arguments

[--use_subsample](../use-cases.md#-use_subsample)
[--num_subsample](../use-cases.md#-use_subsample)
[--skip_grooming](../use-cases.md#-skip_grooming)
[--use_single_scale](../use-cases.md#-use_single_scale)
[--mesh_mode](../use-cases.md#-mesh_mode)
[--tiny_test](../use-cases.md#-tiny_test)

## Optimization Parameters
The python code for the use case calls the `optimize` command of ShapeWorks which requires the parameters of the optimization to be specified in a python dictionary. Please refer to [Parameter Dictionory in Python](../../workflow/optimize.md#parameter-dictionary-in-python) for more details.
NOTE: The list of `<inputs>` (binary segmentation images) should be ordered consistently for each shape.(e.g., shape1-domain1, shape1-domain2, shape2-domain1, shape2-domain2 ... etc.).
Below are the default optimization parameters for this use case.
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
        "verbosity" : 0

      }
```

## Analyzing Shape Model
Once the python code runs the optimization, the files are saved in the `Output` folder after which ShapeWorks Studio is launched from the python code to analyze the model. 
Here is the optimized shape.
![Ellipsoid Joint Shape Model](https://sci.utah.edu/~shapeworks/doc-resources/gifs/multiple_domain_segs_model.gif)

Below are the particles for each sample; we can see they are uniformly distributed.
![Ellipsoid Joint Samples](https://sci.utah.edu/~shapeworks/doc-resources/pngs/multi_domain_samples.png)