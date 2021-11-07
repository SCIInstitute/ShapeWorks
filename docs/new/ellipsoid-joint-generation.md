# Ellipsoid Joint Generation

##About ShapeCohortGenerator

`ShapeCohortGenerator` is a python package that generates synthetic shape cohorts with ground truth surface correspondences by varying different parameters describing such shape families.

## What is new?
We have added a new family of shapes in the `ShapeCohortGenerator` package. You can now generate ellipsoid joints (two anatomies per shape) which can be used for troubleshooting multiple domain shape modeling workflows.

Similar to the `ellipsoid` and `supershapes` generator, the joint generator has three functions:
- `generate()` for mesh generation (function specific to generator type)
- `generate_segmentations()` for segmentation generation based on meshes (general function shared by all generator types)
- `generate_images()` for image generation based on segmentations (general function shared by all generator types)

Please refer to this notebook for all the options for generating the cohorts. 
### [Getting Started with Shape Cohort Generator](../notebooks/getting-started-with-shape-cohort-generation.ipynb) 

Arguments specific to ellipsoid joint generation are:

- `mode_size`: uses size as the mode of variation 
- `mode_rotation`: uses rotation as the mode of variation
- `separation`: distance between the two anatomies of the joint


The [notebook](../notebooks/getting-started-with-shape-cohort-generation.ipynb) will soon be updated with examples of ellipsoid joint generation