# Examples

Visit [Getting Started with Use Cases](../use-cases/use-cases.md) for information about downloading a use case dataset and running use cases.
Here we list the available use cases grouped according to different categories based on the unique features.

## Segmentation Based Use Cases
This use case set demonstrates the [Shape Modeling Workflow](../getting-started/workflow.md) on segmentation images. 

### [Ellipsoid: Basic Example](../use-cases/ellipsoid.md)

This example is a stepping stone for the user to get familiar with the workflow of ShapeWorks. This use case represents the standard use version of a shape modeling workflow using ShapeWorks on a synthetically generated axis-aligned ellipsoid dataset.  

### [Fixed Domains Ellipsoid: SSM on New Shapes](../use-cases/fixed-domain-ellipsoid.md)

This use case is designed for the functionality of the fixed domain of ShapeWorks. The fixed domains are used for the cases where we need to place correspondences on new shapes using a pre-existing shape model. 

### [Left Atrium: SSM from Segmentations](../use-cases/left-atrium.md)

This use case demonstrates using the ShapeWorks functionality to groom segmentations and their corresponding imaging data (e.g., MRI) of a real-world left atrium dataset. This use case also showcases a single-scale and multi-scale optimization for correspondence models.

### [Femur: SSM from Segmentations](../use-cases/femur.md)

This use case demonstrates using shape modeling workflow using ShapeWorks on real-world femur dataset. This use case exhibits how to handle challenges that arise when using real-world clinical data. It shows grooming imaging data (CT scans) of the hip to be tied with the groomed shape data. As femur meshes in this dataset have been segmented with various shaft lengths, this use case includes the ability for the user to select a cutting plane on a single mesh (e.g., representative sample) to remove the shaft length variability so that it is not captured in the shape model. 

### [SSM for Multiple Domains](../use-cases/ellipsoid-multiple-domain.md)

This use case demonstrates using ShapeWorks to perform shape modelings for anatomies with multiple structures (domains), e.g., joints, to capture inter-domains correlations and interactions on segmentation images. This use case uses a synthetically generated ellipsoid joint dataset. This use case exhibits how local alignment can be performed for multiple domain datasets.



## Mesh-Based Use Cases
This use case set demonstrates the [Shape Modeling Workflow](../getting-started/workflow.md) directly on meshes. Currently, mesh-based use cases run on pre-groomed meshes. They will be updated soon to demonstrate mesh grooming.

### [Ellipsoid Mesh: Basic Example](../use-cases/ellipsoid_mesh.md)

This use case uses the same dataset as the [Ellipsoid: Basic Example](../use-cases/ellipsoid.md) use case, but optimization is done on meshes rather than distance transforms. 

### [Femur-Mesh: Shape Model directly from Mesh](../use-cases/femur-mesh.md)
This use case uses the same dataset as the [Femur: SSM from Segmentations](../use-cases/femur.md) use case, but optimization is done on meshes rather than distance transforms. No image grooming is demonstrated here.

### [Lumps: Shape Model directly from Mesh](../use-cases/lumps.md)
This use case demonstrates a minimal example to run ShapeWorks directly on a mesh using a synthetic dataset. The shapes in this dataset are spheres with two lumps that vary in size.

### [Thin Cavity Bean: Shape Model with Geodesic Distances](../use-cases/thin-cavity-bean.md)
This use case demonstrates using ShapeWorks tools to perform Geodesic distance-based repulsion for mesh domains.

### [SSM for Multiple Domains directly from Mesh](../use-cases/ellipsoid-multiple-domain-mesh.md)
This use case uses the same dataset as the [SSM for Multiple Domains](../use-cases/ellipsoid-multiple-domain.md) use case, but optimization is done on meshes rather than distance transforms. 

## Deep Learning Based Use Cases
### [Femur SSM Directly from Images](../use-cases/deep-ssm-femur.md)
This use case demonstrates how to get shape models from unsegmented images using deep learning on the femur data. This includes performing data augmentation as well as building, training, and testing a DeepSSM model. For a detailed description of these processes, please see [Data Augmentation for Deep Learning](../deep-learning/data-augmentation.md) and [SSMs Directly from Images](../deep-learning/deep-ssm.md). 

## Constraints Based Use Cases
### [Ellipsoid: Cutting Planes](../use-cases/ellipsoid-cutting-planes.md)
This use case demonstrates using multiple cutting planes to constrain the distribution of particles on ellipsoids that are already aligned. Cutting planes can be used in modeling scenarios where statistical modeling/analysis is needed for a region-of-interest on the anatomy/object-class at hand without affecting the input data. 

### [Femur with Cutting Planes](../use-cases/femur-cutting-planes.md)
This use case is similar to [Femur: SSM from Segmentations](../use-cases/femur.md), but it demonstrates cutting planes to constrain the particle distribution on the femur surface. The femur meshes in this data set have been segmented with various shaft lengths, as can be seen below. To remove this variability so that it is not captured in the shape model, cutting planes can limit the statistical analysis to the standard anatomical regions across all samples.


## Statistics Based Use Cases
### [Femur: Group Difference Statistics in Python](../use-cases/femur-pvalues.md)
This use case demonstrates the functionality of shape statistics tools to perform hypothesis testing of group shape differences. This use case analyzes the femur shape model obtained from running the [Femur: SSM from Segmentations](../use-cases/femur.md) use case. 

### [Ellipsoid: Shape Statistics in Python](../use-cases/ellipsoid-pca.md)
This use case demonstrates the functionality of shape statistics tools of ShapeWorks python API. These APIs include reading particle files and computing eigenvectors, eigenvalues, and PCA loadings. This use case reads the correspondence model of the [Ellipsoid: Basic Example](../use-cases/ellipsoid.md) use case. 

## Contour Based Use Cases
### [Shape Model for Contour Domains](../use-cases/supershapes-contour.md)
This use case demonstrates using ShapeWorks tools to perform optimization for N-dimensional contours. Contours are represented as a series of lines that may be a closed-loop. 




## Your Use Case

You can use any of these use cases as a starting point and customize it to your dataset. 

In `Examples/notebooks/tutorials`, we provide step-by-step, hands-on tutorials on different aspects of the shape modeling workflow in a transparent, reproducible, and sharable manner. For this purpose, we have chosen Juypter Notebooks as the front-end tools for these demonstrations. These hands-on tutorials are designed to reflect the thought process that a non-expert user could go through at different shape modeling phases, starting from processing or grooming your data to analyzing your optimized shape model. See [ShapeWorks in Python](../new/shapeworks-python.md) for more details.

!!! important "ShapeWorks with Python support."
    ShapeWorks Python library is currently under active development and is part of our major releases as of **ShapeWorks 6**. 


Please [contact ShapeWorks team](../about/contact.md) if you need further questions and guidance. _Software maintenance and support are provided within the funding period._
