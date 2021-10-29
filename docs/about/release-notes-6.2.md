### What is new?

#### ShapeWorks Back-end

* **New `Mesh` grooming tools:** 
    `curvature`: computes curvature (types include principal, gaussian or mean) of given mesh and returns a field
    `fixElement`: fixes element winding of mesh which helps in its quality control
    `geodesicDistance`: computation of geodesic distances enable feature-based correspondences. Read this [paper] (https://link.springer.com/content/pdf/10.1007/978-3-642-40763-5_3.pdf) for more details.
        `geodesicDistance(pointA, pointB)`: computes geodesic distance between 2 points on mesh
        `geodesicDistance(landmark`: computes geodesic distance between all points on mesh to another point (landmark)
        `geodesicDistance(curve)`: computes geodesic distance between all points on mesh and set of points (curve)
    `computeMeanNormals`: computes average normals for each point in given set of meshes and returns a field

* **New `Mesh` query/operator tools:**
    `operator+=`: appends two meshes
    `closestPoint`: returns closest point on a face in the mesh to the given point in space
    `closestPointId`: returns closest point id in this mesh to the given point in space
    `points`: returns matrix with number of points with (x,y,z) coordinates of each point
    `faces`: returns matrix with number of faces with indices of the three points from which each face is composed
    `getFace`: return indices of the three points with which the face at the given index is composed

* **`Mesh` fields:** Added support for passing of multi-value fields in addition to scalar fields. Previously field operations such as getField, setField supported only for single value components. Now these operations can be used for multi value components as well.

* **Free-Form Constraints:** FFCs support has been added. Added a unit test for FFCs, a typical sphere unit test with a constraint that cuts the sphere like a tennis ball grove. Also added a unit test with two domains, both spheres in different locations. The first sphere has one cutting plane and 25 sphere constraints, the second sphere has one cutting plane and one free form constraint. See [Free-Form Constraints] (http://sciinstitute.github.io/ShapeWorks/new/free-form-constraints) for more details.

#### ShapeWorks Front-end

* **DeepSSM in Studio:** The ability to run DeepSSM has been added to Studio. See [DeepSSM in Studio] (http://sciinstitute.github.io/ShapeWorks/studio/deepssm-in-studio.html) for more details.

* **Multiple domains in Studio:** Support for multiple alignment strategies is now present in Studio. It allows analysis with and without articulation with a choice of reference domain, or global alignment. See [Multiple Domain Alignments] (http://sciinstitute.github.io/ShapeWorks/studio/multiple-domains.html#multiple-domain-alignments)

* **New analysis features in Studio:** Shape evaluation charts for compactness, specificty and generealization have been added. See for [Metrics Panel] (http://sciinstitute.github.io/ShapeWorks/studio/getting-started-with-studio.html#metrics-panel) more details.

* **Usability features in Studio:** New usability features such as group p-value display, feature map scalar control, surface opacity controls on a per doamin basis, message history window, suppressible error dialog, narrow band optimization parameter, multiple domain expore options (combined and support) and allow initial landmark points. See [New in ShapeWorks Studio 6.2] (http://sciinstitute.github.io/ShapeWorks/new/new-studio.html) for more details.

#### User's Support

* **Shape cohort generation:** Added segmentation and image generation for 2D contour supershapes. See this [Jupyter Notebook] (http://sciinstitute.github.io/ShapeWorks/notebooks/getting-started-with-data-augmentation.html) for more details.

* **Analyzing the group differences:** A new use case has been added, demonstrating the functionality of shape statistics tools to perform hypothesis testing of group shape differences. See [Femur: Group Difference Statistics in Python] (http://sciinstitute.github.io/ShapeWorks/use-cases/stats-based/femur-pvalues.html) for more details.   

* **Sub-sampling for multiple domains:** We can now perform subsampling for multiple domains data by combining the individual shapes from all the domains and generating combined shapes. We perform a clustering-based subset selection on the combined shapes so that the subset is representative of the entire dataset and all domains. The representative subset of the specified sample size will be helpful to run through the SSM pipeline so that the use case runs faster and uses less memory.

### Improvements

#### ShapeWorks Back-end
* **`Image` API:** Images can now be safely constructed without copying and passed without copying from/to Python. In particular, passing Images for rendering using pyvista is now transparent and copy-free (note: copying is still supported if necessary). A [Jupyter Notebook] (https://github.com/SCIInstitute/ShapeWorks/blob/master/Examples/Python/notebooks/Demo%20-%20Array%20passing%20without%20copying.ipynb) was added to demonstrate Image initialization and passing.

* **`Mesh` API:** Added efficient sharing of large data between Python and C++ (for both Mesh fields and Image data) which enables Python tools which access Mesh fields or Image data for visualization and analysis, or wish to create an Image or add fields from numpy to a Mesh, to do so with optimal efficiency without fear of memory leak due to mishandled transfers.

* **Python API:** Coordsys of images in Python can now be set using 'setCoordsys`. Fixed Python bindings and improved interactive help where necessary. More efficient conversion of shapeworks Mesh in Python to vtk mesh by creating a PolyData instead of performing IO operations.

* **Testing:** More robust testing of Python tests was achieved by adding code to ensure that all tests are performed for all functions, whereas before test failure was reported immediately without testing if any related functions failed. Concrete seeding of random number generators (only for testing) is utilized to ensure objective comparison of results across all platforms.

#### ShapeWorks Front-end

#### User's Support

* **Use cases:** Added `--mesh_mode` option to image-based use cases. When running in mesh mode, after grooming segmentations, distance transforms are converted to meshes and optimization is done directly on meshes, saving memory footprint and allowing the usage of geodesic distances for particle repulsion. This enables improved modeling for thin structures and high curvature regions. See [Use Case Documentation] (http://sciinstitute.github.io/ShapeWorks/use-cases/use-cases.html) for more details.

* **Documentation:** See [Getting Started with Use Cases] (http://sciinstitute.github.io/ShapeWorks/use-cases/use-cases.html) for information on running use cases and [Examples] (http://sciinstitute.github.io/ShapeWorks/getting-started/examples.html) for an overview of released use cases. More explanation of interpreting modes of variation in [Shapes. What & From Where?] (http://sciinstitute.github.io/ShapeWorks/getting-started/shapes.html).

* **Restructured `ellipsoid_evaluate` use case:** The computation time for calculating specificity, compactness, and generalization metrics has been reduced. A 50X speedup was experienced when calculating evaluation metrics for all modes for a dataset with 75 shapes each with 1024 particles. We can now calculate the evaluation metrics for all the modes or query the values for a specified mode. The use case has been modified to demonstrates these functionalities. See [Ellipsoid: Shape Evaluation in Python] (http://sciinstitute.github.io/ShapeWorks/use-cases/stats-based/ellipsoid-evaluate.html) for more details.

* **Optimizer exports alignment transforms:** Write individual procrustes transforms. The ShapeWorks Optimizer can now export individual procrustes transform files if requested. Use the xml tag `<write_transform_files> 1 </write_transform_files>` to enable it. Additinally, for project spreadsheets (e.g., Studio), individual procrustes transforms will be added as additional columns in the data sheet. See [Optimize] (http://sciinstitute.github.io/ShapeWorks/workflow/optimize.html) for more details.

### Fixes

#### ShapeWorks Back-end

* **Memory leak:** Fixed memory leak issues in `FEMesh` operations

#### ShapeWorks Front-end

* **Fix processing of all orientation images:** We have fixed ShapeWorks to handle all orientations of images. Previously, only a subset (such as RAI) were fully compatibile with all tools.
