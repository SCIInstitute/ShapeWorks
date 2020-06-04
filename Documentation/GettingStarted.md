# Getting Started with ShapeWorks 

GettingStarted.md provides overviews of dataset grooming, optimization, and analysis, ShapeWorks Studio and command line instructions, and practical use cases.

Table of Contents
====================
- [Shape Modeling Workflow](#shape-modeling-workflow)  
- [Use Cases](#use-cases) 
- [How Tos](#how-tos)



Shape Modeling Workflow
=====================



Imaging Data and Shape Models
---------------------

The grooming stage entails rigid transformations to align samples for groupwise modeling and analysis. Imaging data (e.g., CT, MRI) becomes out of alignment and cannot be tied to the resulting shape models. We have developed [segmentation-based](UseCases/LeftAtrium.md) and [mesh-based](UseCases/Femur.md) grooming tools and associated python scripts (i.e., use cases) to carry volumetric data through each grooming step with the shapes (meshes or segmentations) such that they can be used for subsequent analysis and visualization. These tools include image reflection (for paired anatomies), isotropic voxel resampling, image padding, applying shape-based alignment to images (center of mass and rigid alignment), and image cropping.


Use Cases
=====================

Ellipsoid Use Case
---------------------

This [example](UseCases/Ellipsoid.md) is a steeping stone for the user to get familiar with the workflow of ShapeWorks. This use case represent the standard use version of a shape modeling workflow using ShapeWorks. 


Fixed Domains Ellipsoid Use Case
---------------------

This [use case](UseCases/FixedDomainEllipsoid.md) is designed for the fixed domains functionality of ShapeWorks. The fixed domains are used for the cases where we need to place correspondences on new shapes using a pre-existing shape model. 

Left Atrium Use Case
---------------------

This segmentation-based [use case](UseCases/LeftAtrium.md) demonstrates using the ShapeWorks functionality to groom shapes (given as binary segmentations) and their corresponding imaging data (e.g., MRI). This use case also showcase single scale and multiscale optimization for correspondence models.

Femur Use Case
---------------------

This mesh-based [use case](UseCases/Femur.md) demonstrates using ShapeWorks tools to convert shapes (femurs in this case) given as surface mehes to signed distance maps (the currently supported data type to optimize the particle system). It also show grooming imaging data (CT scans) of the hip to be tied with the groom shape data. As femur meshes in this dataset have been segmented with various shaft lengths, this use case include an interactive tool for the user to select a cutting plane on a single mesh (e.g., representative sample) to remove this variablity so that it is not captured in the shape model. 

Your Own Use Case
---------------------

You can use any of these use cases as a starting point and customize it to your own dataset. 

How Tos
=====================
- How to run and see ShapeWorks in action on exemplar [use cases](UseCases/UseCases.md). 
- How to preprocess or [groom](Workflow/Groom.md) your dataset.
- How to [optimize](Workflow/Optimize.md) your shape model.
- How to [visualize and analyze](Workflow/Analyze.md) your optimized shape model.
- How to [contact ShapeWorks team](../README.md#contact-us) to help you customize a use case to your own dataset.
- How to get the [latest ShapeWorks binary release](https://github.com/SCIInstitute/ShapeWorks/releases/latest) or up-to-date development builds from the master branch for [Windows](https://github.com/SCIInstitute/ShapeWorks/releases/tag/dev-windows), [Mac](https://github.com/SCIInstitute/ShapeWorks/releases/tag/dev-mac), or [Linux](https://github.com/SCIInstitute/ShapeWorks/releases/tag/dev-linux). *Please understand that these are in-progress development builds, not official releases.*
- How to install ShapeWorks on [Windows](Install/Windows_README.md), [Mac](Install/Mac_README.md), or [Linux](Install/Linux_README.md).
- How to [build](Build/BUILD.md) ShapeWorks from source (for developers).
