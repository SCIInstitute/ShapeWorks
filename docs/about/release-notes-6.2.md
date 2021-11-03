### What is new?

#### ShapeWorks Back-end

* **New `Mesh` grooming tools:**  
    - `curvature`: computes curvature (types include principal, gaussian, and mean curvature) of a given triangular mesh and returns a field that contains a scalar value for each mesh vertex. See [mesh-curvature] (https://sciinstitute.github.io/ShapeWorks/tools/ShapeWorksCommands.html#mesh-curvature) to know about function parameters. 
    - `fixElement`: fixes element winding of a given triangular mesh as a quality control step for preparing meshes for shape modeling. See [fix-element] (https://sciinstitute.github.io/ShapeWorks/tools/ShapeWorksCommands.html#fix-element) to know about function parameters.  
    - `geodesicDistance`: the computation of geodesic distances enables feature-based correspondences. Read this [paper] (https://link.springer.com/content/pdf/10.1007/978-3-642-40763-5_3.pdf) for more details. Below are exemplar usage scenarios.    
        - `geodesicDistance (pointA, pointB)`: computes geodesic distance between 2 points on a triangular mesh. See [geodesic-distance] (https://sciinstitute.github.io/ShapeWorks/tools/ShapeWorksCommands.html#geodesic-distance) to know about function parameters.  
        - `geodesicDistance (landmark)`: computes geodesic distances between all points on a triangular mesh to a given point (landmark). See [geodesic-distance-landmark] (https://sciinstitute.github.io/ShapeWorks/tools/ShapeWorksCommands.html#geodesic-distance-landmark) to know about function parameters.   
        - `geodesicDistance (curve)`: computes geodesic distances between all points on mesh and set of points (curve)   
    `computeMeanNormals`: computes the average surface normal for each mesh vertex in a given set of triangular meshes with vertex-wise correspondences and returns a field containing a normal vector for each mesh vertex. See [mean-normals] (https://sciinstitute.github.io/ShapeWorks/tools/ShapeWorksCommands.html#mean-normals) to know about function parameters. 

* **New `Mesh` query/operator tools:**   
    - `operator+=`: appends a mesh to an existing mesh. The result of this operator is a single mesh with a single vertex and face lists.
    - `closestPoint`: returns the closest point on a face in the mesh to a given point in space. See [closest-point] (https://sciinstitute.github.io/ShapeWorks/tools/ShapeWorksCommands.html#closest-point) to know about function parameters. 
    - `closestPointId`: returns closest point id in the mesh to a given point in space   
    - `points`: returns matrix with number of points with (x,y,z) coordinates of each point   
    - `faces`: returns matrix with number of faces with indices of the three points from which each face is composed   
    - `getFace`: return indices of the three points with which the face at the given index is composed   

* **`Mesh` fields:** Added support for passing multi-valued fields in addition to scalar fields. Previously field operations such as `getField`, `setField` supported only single-value components. Now, these operations can be used for multi-valued components as well. This is useful for associated surface meshes with positional (i.e., spatially varying) features, e.g., application-specific features such as cortical thickness and bone density, and computationally driven features such as geodesics to anatomical landmarks, curvatures, and surface normals.   

* **Free-form constraints (FFCs):** FFCs support has been added. Added a unit test for FFCs, a typical sphere unit test with a constraint that cuts the sphere like a tennis ball grove. Also added a unit test with two domains, both spheres in different locations. The first sphere has one cutting plane and 25 sphere constraints, and the second sphere has one cutting plane and one free form constraint. See [Free-Form Constraints] (http://sciinstitute.github.io/ShapeWorks/new/free-form-constraints) for more details.

#### ShapeWorks Front-end

* **Multiple domains in Studio:** Support for multiple alignment strategies is now present in Studio. It allows analysis with and without articulation with a choice of reference domain or global alignment. See [Multiple Domain Alignments] (http://sciinstitute.github.io/ShapeWorks/studio/multiple-domains.html#multiple-domain-alignments)

* **New analysis features in Studio:** Shape evaluation charts for compactness, specificity, and generalization have been added. See for [Metrics Panel] (http://sciinstitute.github.io/ShapeWorks/studio/getting-started-with-studio.html#metrics-panel) more details.

* **Usability features in Studio:** New usability features such as group p-value display, feature map scalar control, surface opacity controls on a per doamin basis, message history window, suppressible error dialog, narrow band optimization parameter, multiple domain expore options (combined and support) and allow initial landmark points. See [New in ShapeWorks Studio 6.2] (http://sciinstitute.github.io/ShapeWorks/new/new-studio.html#usability-features) for more details.

#### User's Support

* **Shape cohort generation:** Added segmentation and image generation for 2D contour supershapes. See this [Jupyter Notebook] (http://sciinstitute.github.io/ShapeWorks/notebooks/getting-started-with-data-augmentation.html) for more details.

* **Analyzing the group differences:** A new use case has been added, demonstrating the functionality of shape statistics tools to perform hypothesis testing of group shape differences. See [Femur: Group Difference Statistics in Python] (http://sciinstitute.github.io/ShapeWorks/use-cases/stats-based/femur-pvalues.html) for more details.   

* **Sub-sampling for multiple domains:** We can now perform subsampling for multiple domains data by combining the individual shapes from all the domains and generating combined shapes. We perform a clustering-based subset selection on the combined shapes so that the subset is representative of the entire dataset and all domains. The representative subset of the specified sample size will be helpful to run through the SSM pipeline so that the use case runs faster and uses less memory.

#### Deep Learning

* **DeepSSM in Studio:** The ability to run DeepSSM has been added to Studio. See [DeepSSM in Studio] (http://sciinstitute.github.io/ShapeWorks/studio/deepssm-in-studio.html) for more details.

### Improvements

#### ShapeWorks Back-end

* **Safe construction of `Image` instances in Python:** Images can now be safely constructed without copying and passed without copying from/to Python. In particular, passing Images for rendering using `pyvista` is now transparent and copy-free (note: copying is still supported if necessary). A [Jupyter Notebook] (http://sciinstitute.github.io/ShapeWorks/notebooks/array-passing-without-copying.html) was added to demonstrate `Image` initialization and passing.

* **Efficient data sharing between Python and C++:** Added efficient sharing of large data between Python and C++ (for both `Mesh` fields and `Image` data) that enables Python tools which access `Mesh` fields or `Image` data for visualization and analysis, or wish to create an Image or add fields from NumPy to a `Mesh`, to do so with optimal efficiency without fear of memory leak due to mishandled transfers. 

* **Improved Python APIs:** `Coordsys` of images in Python can now be set using `setCoordsys`. Fixed Python bindings and improved interactive help where necessary. More efficient conversion of shapeworks `Mesh` in Python to vtk mesh by creating a `PolyData` instead of performing IO operations. 

* **Improved testing:** More robust testing of Python tests was achieved by adding code to ensure that all tests are performed for all functions, whereas before the test failure was reported immediately without testing if any related functions failed. Concrete seeding of random number generators (only for testing) is utilized to ensure objective comparison of results across all platforms. Improved verification of use case testing by comparing shape statistics compactness, generalization, and specificity against a good shape model instead of checking for a file. A log file (`verify.log`) is written that contains the use case outputs and summary.

#### User's Support

* **Use cases:** Added `--mesh_mode` option to image-based use cases. When running in mesh mode, after grooming segmentations, distance transforms are converted to meshes, and optimization is done directly on meshes, saving memory footprint and allowing the usage of geodesic distances for particle repulsion. This enables improved modeling for thin structures and high curvature regions. See [Use Case Documentation] (http://sciinstitute.github.io/ShapeWorks/use-cases/use-cases.html#use-cases-arguments) for more details. 

* **Improved use cases documentation:** Restructured use cases documentation to reduce repetition and better highlight the focus and differences of each demonstration. Added relevant documentation links for every step and parameter in use case descriptions. Reorganized use case documentation based on category (i.e., mesh-based, constraint-based, etc.). See [Getting Started with Use Cases] (http://sciinstitute.github.io/ShapeWorks/use-cases/use-cases.html) for information on running use cases and [Examples] (http://sciinstitute.github.io/ShapeWorks/getting-started/examples.html) for an overview of released use cases. More explanation of interpreting modes of variation in [Shapes. What & From Where?] (http://sciinstitute.github.io/ShapeWorks/getting-started/shapes.html).

* **Restructured `ellipsoid_evaluate` use case:** The computation time for calculating specificity, compactness, and generalization metrics have been dramatically reduced. A 50X speedup was experienced when calculating evaluation metrics for all modes for a dataset with 75 shapes, each with 1024 particles. We can now calculate the evaluation metrics for all the modes or query the values for a specified mode. The use case has been modified to demonstrate these functionalities. See [Ellipsoid: Shape Evaluation in Python] (http://sciinstitute.github.io/ShapeWorks/use-cases/stats-based/ellipsoid-evaluate.html) for more details.

* **Optimizer exports alignment transforms:** Write individual procrustes transforms. The ShapeWorks `Optimizer` can now export individual procrustes transform files if requested. Use the xml tag `<write_transform_files> 1 </write_transform_files>` to enable it. Additionally, for project spreadsheets (e.g., Studio), individual procrustes transforms will be added as additional columns in the data sheet. See [Optimize] (http://sciinstitute.github.io/ShapeWorks/workflow/optimize.html) for more details.

### Fixes

#### ShapeWorks Back-end

* **Memory leak:** Fixed memory leak issues in `FEMesh` operations

#### ShapeWorks Front-end

* **Fix processing of all orientation images:** We have fixed ShapeWorks to handle all orientations of images. Previously, only a subset (such as RAI) was fully compatible with all tools. 
