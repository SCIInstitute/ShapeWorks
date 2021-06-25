# Shapes, What & From Where?

## What is Shape?

![What is Shape?](../img/getting-started/shape.png)

The shape is the characteristic that remains after removing all global geometrical information from an object. To study shape, we would like to study the differences among these characteristics in populations of objects belonging to the same class.

## Where Shapes Come From? 

In medical imaging, shapes can be obtained from images of anatomies (e.g., CTs and MRIs), where anatomies of interest can be manually or semi-automatically segmented/delineated. Other applications could entail modeling geometries using computer-aided design systems, or acquiring real-world objects through sampling (2D via image acquisition devices, 3D via laser scanners).

![Where Shapes Come From?](../img/getting-started/shape-where.png)

Here are some examples of free software packages that include enable both manual and semi-automated anatomy segmentation:

- [Seg3D](https://www.sci.utah.edu/software/seg3d.html)
- [ITK-SNAP](http://www.itksnap.org)
- [3DSlicer](https://www.slicer.org)

## What surfaces are ”good” for shape modeling?

The results from the segmentation process are often not directly usable for shape modeling tools. Let’s go over some examples of good and bad segmentation results for shape modeling.

Here is an example of a poor segmentation for the calcaneus on the right of the below figure. Segmentation methods, especially those that rely on intensity thresholding, can easily result in bone islands. Segmented anatomies can have rough surfaces and aliasing artifacts due to intensity noise and limitations on voxel spacing. Planar striations and non-physiological bone holes can result due to imaging artifacts. All these types of artifacts are not anatomically relevant and should not be captured when we study the statistics of shapes. This is in comparison to a smoothed and decimated segmentation on the left of the below figure where we can see relevant morphological features to be studied.

![Ankle - good and bad surfaces](../img/getting-started/ankle-good-bad.png)


Below on the right is an example of a scapula segmentation where limitations in voxel spacing, noisy image intensities, and patient positioning in the scanner can result in stair-step features, irrelevant bone protrusions, and non-anatomical holes in thin structures. These types of artifacts negatively affect the integrity of the underlying geometry, however, they can be fixed by some data preprocessing steps before feeding the surfaces to shape modeling. On the left is the same scapula after hole filing and surface smoothing.

![Scapula - good and bad surfaces](../img/getting-started/scapula-good-bad.png)


Below are another examples of bad surfaces for shape modeling where the surface meshing step can  produce some connectivity artifacts such as non-watertight meshes. A surface mesh could also have evident transverse slices due to voxel spacing and patient positioning resulting in some ringing artifacts on the surface. A smooth surface mesh like that one on the left is the best fit for shape modeling and analysis.

![Hip - good and bad surfaces](../img/getting-started/hip-good-bad.png)


!!! danger "Data preparation and quality control is the first step!"    
    For shape modeling purposes, it is recommended that we quality control segmentation results before feeding them to the shape modeling process. Most of these surface integrity issues can be resolved by some data preprocessing step, which is the [groom stage](../workflow/groom.md) in a typical [shape modeling workflow](../getting-started/workflow.md). 


## What is Shape Modeling?

Shape modeling is about learning population-specific parameterization. 

![Shape Modeling](../img/getting-started/shape-modeling.png)

