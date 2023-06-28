# Examples

Visit [Getting Started with Use Cases](../use-cases/use-cases.md) for information about downloading a use case dataset and running use cases.
Here we list the available use cases grouped according to different categories based on their unique features.

## Segmentation Based Use Cases
This use case set demonstrates the [Shape Modeling Workflow](../getting-started/workflow.md) on segmentation images.

### [Ellipsoid: Basic Example](../use-cases/segmentation-based/ellipsoid.md)
This example is a stepping stone for the user to get familiar with the workflow of ShapeWorks. This use case represents the standard ShapeWorks workflow on a synthetically generated ellipsoid dataset, including the entire grooming process.

### [Left Atrium: Shape Model from Segmentations](../use-cases/segmentation-based/left-atrium.md)
This use case demonstrates using the ShapeWorks functionality to groom real segmentations and corresponding imaging data (e.g., MRI) of a real-world left atrium dataset. This use case also showcases a single-scale and multi-scale optimization for correspondence models.

### [Fixed Domains Ellipsoid: Shape Model on New Shapes](../use-cases/multistep/fixed-domain-ellipsoid.md)
This use case is designed to demonstrate the functionality of the fixed domain of ShapeWorks. Fixed domains are used for the cases where we need to place correspondences on new shapes using a pre-existing shape model.

### [Shape Model for Multiple Domains from Segmentations](../use-cases/segmentation-based/ellipsoid-multiple-domain.md)
This use case demonstrates using ShapeWorks to perform shape modelings for anatomies with multiple structures (domains), e.g., joints, to capture inter-domain correlations and interactions on segmentation images. This use case uses a synthetically generated ellipsoid joint dataset. This use case exhibits how local alignment can be performed for multiple domain datasets.


## Mesh-Based Use Cases
This use case demonstrates the [Shape Modeling Workflow](../getting-started/workflow.md) directly on meshes. Currently, mesh-based use cases run on pre-groomed meshes. They will be updated soon to demonstrate mesh grooming.

### [Ellipsoid Mesh: Basic Example](../use-cases/mesh-based/ellipsoid_mesh.md)
This use case uses the same dataset as the [Ellipsoid: Basic Example](../use-cases/segmentation-based/ellipsoid.md) use case, but optimization is done on meshes rather than distance transforms.

### [Lumps: Shape Model directly from Mesh](../use-cases/mesh-based/lumps.md)
This use case demonstrates a minimal example of running ShapeWorks directly on a mesh using a synthetic dataset. The shapes in this dataset are spheres with two lumps or nodes that vary in size. The use case demonstrates that the ShapeWorks workflow results in a correct shape model- i.e., only the position of particles on the lumps vary; the rest are constant across the shape population.

### [Thin Cavity Bean: Shape Model with Geodesic Distances](../use-cases/mesh-based/thin-cavity-bean.md)
This use case demonstrates using ShapeWorks tools to perform Geodesic distance-based repulsion for mesh domains. The dataset comprises ellipsoids or beans with a thin cavity where the shapes vary only in the location of the thin cavity. This use case demonstrates that ShapeWorks optimization with geodesic distance can correctly capture this single mode of variation.

### [Shape Model for Multiple Domains directly from Mesh](../use-cases/mesh-based/ellipsoid-multiple-domain-mesh.md)
This use case uses the same dataset as the [Shape Model for Multiple Domains from Segmentations](../use-cases/segmentation-based/ellipsoid-multiple-domain.md); use case optimization is done on meshes rather than distance transforms.  

## Contour Based Use Cases
### [Supershapes:Shape Model for Contour Domains](../use-cases/contour-based/supershapes-contour.md)
This use case demonstrates using ShapeWorks tools to perform optimization for N-dimensional contours. Contours are represented as a series of lines that may be a closed loop. 

## Constraints Based Use Cases
### [Ellipsoid: Shape Model with Cutting Planes](../use-cases/constraint-based/ellipsoid-cutting-planes.md)
This use case demonstrates using multiple cutting planes to constrain the distribution of particles on ellipsoids that are already aligned. Cutting planes can be used in modeling scenarios where statistical modeling/analysis is needed for a region of interest in the anatomy/object class without affecting the input data. 

### [Femur: Shape Model from Meshes with Cutting Planes](../use-cases/constraint-based/femur-cutting-planes.md)
This use case demonstrates using shape modeling workflow using ShapeWorks on a real-world femur dataset. It involves performing grooming on femur meshes and corresponding imaging data (CT scans) of the hip. The femur meshes in this dataset have been segmented with various shaft lengths, so cutting planes are used in optimization to remove this variability, preventing it from being captured in the shape model. Cutting planes can limit the statistical analysis to the standard anatomical regions across all samples.

## Multi-Step Optimization Use Cases 
### [Fixed Domains Ellipsoid: Shape Model on New Shapes](../use-cases/multistep/fixed-domain-ellipsoid.md)
This use case is designed to demonstrate the functionality of the fixed domain of ShapeWorks. Fixed domains are used for the cases where we need to place correspondences on new shapes using a pre-existing shape model. 
### [Incremental Supershapes: Building a Shape Model Incrementally](../use-cases/multistep/incremental_supershapes.md)
This use case demonstrates how a shape model can be built incrementally. In this case, by initially fitting a model on the most similar shapes, then incrementally adding outlier shapes, we can achieve a more compact shape model with better correspondence than if we were to optimize the whole dataset at once.

### [Studio Femur: Shape Model from Meshes with Cutting Planes Using Studio](../use-cases/constraint-based/femur-cutting-planes-studio.md)
This use case demonstrates using shape modeling workflow using ShapeWorks on real-world femur dataset. This tutorial use-case teaches you how to use constraints in Studio to limit the statistical analysis to certain anatomical regions.

## Statistics Based Use Cases
### [Femur: Group Difference Statistics in Python](../use-cases/stats-based/femur-pvalues.md)
This use case demonstrates the functionality of shape statistics tools to perform hypothesis testing of group shape differences. It also shows the use of Linear Discrimination of Variation (LDA) for analyzing shape variation between the subgroups of patients. This use case explores the femur shape model obtained from running the [Femur: Shape Model on Distance Transforms from Meshes](../use-cases/constraint-based/femur-cutting-planes.md) use case. 

### [Ellipsoid: Shape Statistics in Python](../use-cases/stats-based/ellipsoid-pca.md)
This use case demonstrates the functionality of shape statistics tools of ShapeWorks python API. These APIs include reading particle files and computing eigenvectors, eigenvalues, and PCA loadings. This use case reads the correspondence model of the [Ellipsoid: Basic Example](../use-cases/segmentation-based/ellipsoid.md) use case.

### [Ellipsoid: Shape Evaluation in Python](../use-cases/stats-based/ellipsoid-evaluate.md)
This use case demonstrates the functionality of shape evaluation tools of ShapeWorks python API. These APIs include the calculation of quantitative evaluation metrics such as specificity, generalization, and compactness.For detailed explanation of the evaluation metrics, refer to [Shape Model Evaluation.](../new/ssm-eval.md) 

## Deep Learning Based Use Cases
### [Femur Shape Model Directly from Images](../use-cases/deep-learning-based/deep-ssm-femur.md)
This use case demonstrates how to get shape models from unsegmented images using deep learning on the femur data. This includes performing data augmentation and building, training, and testing a DeepSSM model. For a detailed description of these processes, please see [Data Augmentation for Deep Learning](../deep-learning/data-augmentation.md) and [SSMs Directly from Images](../deep-learning/deep-ssm.md).

## Shared Boundary Use Cases
### [Peanut: Shape Model of Multiple Domains with Shared Boundaries](../use-cases/shared-boundary-based/peanut.md)
This use case demonstrates using ShapeWorks tools to perform shape modelings for anatomies with multiple structures (domains), e.g., joints, with shared boundaries to capture inter-domain correlations and interactions.

## Your Use Case

You can use any of these use cases as a starting point and customize it to your dataset.

In `Examples/notebooks/tutorials`, we provide step-by-step, hands-on tutorials on different aspects of the shape modeling workflow in a transparent, reproducible, and sharable manner. For this purpose, we have chosen Juypter Notebooks as the front-end tools for these demonstrations. These hands-on tutorials are designed to reflect the thought process that a non-expert user could go through during different shape modeling phases, starting from processing or grooming your data to analyze your optimized shape model. See [ShapeWorks in Python](../new/shapeworks-python.md) for more details.

!!! important "ShapeWorks with Python support."
    ShapeWorks Python library is currently under active development and is part of our major releases as of **ShapeWorks 6**.


Please [contact ShapeWorks team](../about/contact.md) if you need further questions and guidance. _Software maintenance and support are provided within the funding period._
