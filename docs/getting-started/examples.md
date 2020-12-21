# Examples

Visit [Getting Started with Use Cases](../use-cases/use-cases.md) for information about downloading a use case dataset and running use cases.

## Ellipsoid: Basic Example

This [example](../use-cases/ellipsoid.md) is a stepping stone for the user to get familiar with the workflow of ShapeWorks. This use case represents the standard use version of a shape modeling workflow using ShapeWorks. 


## Fixed Domains Ellipsoid: SSM on New Shapes

This [use case](../use-cases/fixed-domain-ellipsoid.md) is designed for the fixed domains functionality of ShapeWorks. The fixed domains are used for the cases where we need to place correspondences on new shapes using a pre-existing shape model. 

## Left Atrium: SSM from Segmentations

This segmentation-based [use case](../use-cases/left-atrium.md) demonstrates using the ShapeWorks functionality to groom shapes (given as binary segmentations) and their corresponding imaging data (e.g., MRI). This use case also showcases a single-scale and multi-scale optimization for correspondence models.

## Femurs: SSM from Meshes

This mesh-based [use case](../use-cases/femur.md) demonstrates using ShapeWorks tools to convert shapes (femurs in this case) given as surface meshes to signed distance maps (the currently supported data type to optimize the particle system). It also shows grooming imaging data (CT scans) of the hip to be tied with the groomed shape data. As femur meshes in this dataset have been segmented with various shaft lengths, this use case includes an interactive tool for the user to select a cutting plane on a single mesh (e.g., representative sample) to remove this variability so that it is not captured in the shape model. 


## Your Own Use Case

You can use any of these use cases as a starting point and customize it to your own dataset. 

In `Examples/notebooks/tutorials`, we also provide step-by-step, hands-on tutorials on different aspects of the shape modeling workflow in a transparent, reproducible, and sharable manner. For this purpose, we have chosen Juypter Notebooks as the front-end tools for these demonstrations. These hands-on tutorials are designed to reflect the thought process that a non-expert user could go through at different shape modeling phases, starting from processing or grooming your data to analyzing your optimized shape model. See [ShapeWorks in Python](../new/shapeworks-python.md) for more details.

!!! important "ShapeWorks with Python support"
    ShapeWorks Python library is currently under active development and is part of our major releases as of **ShapeWorks 6**. 


Please [contact ShapeWorks team](../about/contact.md) if you need further questions and guidance. _Software maintenance and support are provided within the funding period._
