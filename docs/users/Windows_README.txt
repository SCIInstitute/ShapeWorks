Welcome to ShapeWorks!

### To complete your installation:

1. Download and install Miniconda for Windows.

  https://repo.anaconda.com/miniconda/Miniconda3-latest-Windows-x86_64.exe

3. Double-click on the installer that you downloaded and follow the instructions.

4. Open an Anaconda terminal and change directory to the chosen installation path.

`cd "C:\Program Files\ShapeWorks"`

5. Run install_shapeworks.bat to create a conda environment and install python packages

`install_shapeworks.bat`

  **Note:** You can pass a different name for the environment, enabling multiple installations.
  `install_shapeworks shapeworks_61`

  **Warning:** If you already have anaconda/miniconda installed, this step may
    hang.  If so please uninstall anaconda/miniconda, re-install it, then run
    install_shapeworks again. This seems to be an occasional problem with
    Anaconda on Windows.

**Important:** Your shapeworks conda environment must always be activated before using ShapeWorks python packages.


### ShapeWorks comes with examples to get you started.

1. Copy the Examples folder to another location of your choosing (you can also use Explorer).
`xcopy /E/H Examples %HOMEPATH%\ShapeWorks-Examples\`

### Studio ###

1. Open ShapeWorks Studio

2. Select "Open Exising Project"

3. Select Examples/Studio/Ellipsoid/ellipsoid.xlsx

### Python ###

1. Open an Anaconda terminal and activate the shapeworks conda environment (use the environment name passed to `install_shapeworks` above).

`conda activate shapeworks`

2. Change to the Python folder of the Examples directory you copied.
`cd %HOMEPATH%\ShapeWorks-Examples\Python`

3. Run one of the included use cases. To list them all, run: `python RunUseCase.py --help`.
`python RunUseCase.py ellipsoid`

### ShapeWorks also includes interactive Jupyter Python notebook examples.

1. Open an Anaconda terminal and activate the shapeworks conda environment (use the environment name passed to `install_shapeworks` above).
`conda activate shapeworks`

2. Change to the notebook tutorials folder of the Examples directory you copied.

`cd %HOMEPATH%\ShapeWorks-Examples\Python\notebooks\tutorials`

3. Start the Jupyter notebook server. This will open a new tab in your web brower.

`jupyter notebook`

4. Click on a notebook to get started.


=====================

Please contact us with any questions or ideas.

Website    sciinstitute.github.io/ShapeWorks
Email      shapeworks-users@sci.utah.edu
Discourse  shapeworks.discourse.group
Twitter    @ShapeWorksSuite

=====================

Release Notes:

==========================================
 ShapeWorks   v6.3.1 - 2022-07-22
==========================================

### Fixes
  * Fix Studio python problem causing crash on group differences (#1781)
  * Fix Studio slow/hanging on certain free form constraints (#1817, #1801)
  * Fix crashes on certain projects (#1800, #1815)

==========================================
 ShapeWorks   v6.3.0 - 2022-06-09
==========================================

### What is new?
  * **ShapeWorks Back-end**
    * Added constraints functionality for the mesh domain both clipping and augmented lagrangian together with a flag to flip between the two options.
    * Group Difference Statistics in Python can now perform LDA.  The use case also demonstrates Linear Discrimination of Variation (LDA) for analyzing shape variation between the subgroups.
  * **ShapeWorks Front-end**
    * New selectable and configurable colormap support
    * Added new support for showing the difference to the mean for any given mesh (subjects or generated PCA mode positions)
    * Added new support for displaying multiple image types (e.g. original vs groomed) with individual opacity settings. Also ability to show surface to surface distance.
    * The multiple domains groom section has been upgraded to its own panel for better UI consistency. A new checkbox for applying grooming steps to all domains has been added to assist when many domains are in use
    * A new image export dialog as been added with various export options
    * PCA Montage and Fringe plot export.  Building on the image export dialog, the PCA Montage exporter allows you to create a multi-image montage across PCA modes.
    * Export scalar values: Addition export options have been added to export mesh scalars, particle scalars, and all subjects particle scalars.
    * Image volume support: New support has been added for displaying 2D slices from image volumes (e.g. CT/MRI)
    * Landmark placement UI in Studio, landmark registration, landmarks as initializers
    * Cutting planes UI in Studio: Added new support for defining and manipulating cutting planes
    * Free form constraints UI in Studio: Added new support for defining free form constraints
    * Procrustes scaling only mode: New support for running procrustes in a scaling-only mode has been added.
    * Good/bad particle display: The Particles Panel enabled the display of "good/bad particles" in ShapeWorks Studio.
    * Group LDA chart in Studio: Support for the group LDA chart has been added in Studio
  * **User's Support**
    * Added grooming steps to mesh-based use cases using the mesh Python API
    * Alignment transforms are now passed to the optimizer and used in optimization instead of being applied before optimization. This results in local particles in the original data's coordinate system, allowing for easier subsequent analysis
    * The use cases now use project spreadsheets in optimizations instead of XML files. This format is more interpretable and allows of better integration with Studio. The project sheets support multiple domains, fixed domains, constraints
    * The femur use case has been refactored into a single use case where alignment transforms and cutting plane constraints are passed in optimization.
    * Grooming added for multiple domain use cases. The pipeline demonstrates alignment w.r.t domain 1 ellipsoids.
    * DeepSSM Use Case:
        * The DeepSSM use case has been updated to demonstrate the full pipeline, including training data generation instead of relying on the femur use case to create a training shape model.
        * The use case now demonstrates how to optimize validation particles via fixed domain optimization where the training particles are unchanged.
        * Image-to-image registration tools have been added to prepare DeepSSM input images without requiring corresponding segmentations or meshes. This allows for true inference with DeepSSM.
    * Incremental Use Case:
        * A new use case has been added, demonstrating how a shape model can be optimized incrementally on 3D supershapes. This approach is beneficial when the cohort of shapes is very large, and single optimization would be slow, and when the dataset is small but contains a large amount of shape variation.
        * Functionality has been added to select the order of shape optimization based on the distance of each shape to all others in the cohort. This allows for particles to be fit to inlier shapes first, then outliers.
        * Documentation has been added that explains the use case and quantitatively demonstrates the benefit of incremental optimization.
    * Added a Studio use case for constraints and a pseudo-tutorial for it in the documentation.

### Fixes
  * Studio: TabWidget rendering on MacOS 11/12 fixed
  * Mesh::toDistanceTransform fixed
  * Studio: Fixed optimization abort not always aborting
  * Optimize: Fixed particle splitting for use with input transforms
  * Studio: Fix clamping of glyph size
  * Studio: Fix bug when groom output path is blank

  
==========================================
 ShapeWorks   v6.2.1 - 2022-01-07
==========================================

### What is new?
  * **ShapeWorks Back-end**
    * Added new `isolate` functionality that isolates the largest object in a segmentation
    * Added remeshing using ACVD library
    * Added option to save mesh file as binary (default is ASCII)
    * Uniform transform interface added: ITK transforms can be applied to meshes and VTK transforms can be applied to images. 
  * **ShapeWorks Front-end**
    * Studio: Added convert to mesh pipeline for segmentation inputs (can run both image and mesh pipelines)
    * Studio: Added reflection, remeshing, image cropping, image resampling to grooming
    * Studio: New UI for grooming
  * **User's Support**
    * Updated `ellipsoid_mesh` use case to demonstrate mesh grooming
    * Combined the functionality of the three femur use cases - `femur`,`femur_mesh`,`femur_cut` into one use case. The `femur_cut` use case now demonstrates the process of grooming meshes and the associated images , optimization on meshes with a single cutting plane as a constraint.
### Fixes
  * Studio: Fixed display names in the corner (removing long paths)
  * Studio: Fixed recomputation of shape statistics upon re-running shape model and removing shapes

==========================================
 ShapeWorks   v6.2.0 - 2021-11-16
==========================================

### What is new?

#### ShapeWorks Back-end

* **New `Mesh` grooming tools:**  
The following grooming tools are supported from all three different interfaces - command line, C++ and Python:  
    - `curvature`: computes curvature (types include principal, gaussian, and mean curvature) of a given triangular mesh and returns a field that contains a scalar value for each mesh vertex. See [mesh-curvature]( ../tools/ShapeWorksCommands.md#mesh-curvature) to know about function parameters. 
    - `fixElement`: fixes element winding of a given triangular mesh as a quality control step for preparing meshes for shape modeling. See [fix-element](../tools/ShapeWorksCommands.md#fix-element) to know about function parameters.  
    - `geodesicDistance`: the computation of geodesic distances enables feature-based correspondences. Read this [paper](https://link.springer.com/content/pdf/10.1007/978-3-642-40763-5_3.pdf) for more details. Below are exemplar usage scenarios.    
        - `geodesicDistance (pointA, pointB)`: computes geodesic distance between 2 points on a triangular mesh. See [geodesic-distance](../tools/ShapeWorksCommands.md#geodesic-distance) to know about function parameters.  
        - `geodesicDistance (landmark)`: computes geodesic distances between all points on a triangular mesh to a given point (landmark). See [geodesic-distance-landmark](../tools/ShapeWorksCommands.md#geodesic-distance-landmark) to know about function parameters.   
        - `geodesicDistance (curve)`: computes geodesic distances between all points on mesh and set of points (curve)   
    `computeMeanNormals`: computes the average surface normal for each mesh vertex in a given set of triangular meshes with vertex-wise correspondences and returns a field containing a normal vector for each mesh vertex. See [mean-normals](../tools/ShapeWorksCommands.md#mean-normals) to know about function parameters. 

* **New `Mesh` query/operator tools:**  
The following tools are supported from all three different interfaces - command line, C++ and Python:    
    - `operator+=`: appends a mesh to an existing mesh. The result of this operator is a single mesh with a single vertex and face lists.
    - `closestPoint`: returns the closest point on a face in the mesh to a given point in space. See [closest-point](../tools/ShapeWorksCommands.md#closest-point) to know about function parameters. 
    - `closestPointId`: returns closest point id in the mesh to a given point in space   
    - `points`: returns matrix with number of points with (x,y,z) coordinates of each point   
    - `faces`: returns matrix with number of faces with indices of the three points from which each face is composed   
    - `getFace`: return indices of the three points with which the face at the given index is composed   

* **`Mesh` fields:** Added support for passing multi-valued fields in addition to scalar fields. Previously field operations such as `getField`, `setField` supported only single-value components. Now, these operations can be used for multi-valued components as well. This is useful for associated surface meshes with positional (i.e., spatially varying) features, e.g., application-specific features such as cortical thickness and bone density, and computationally driven features such as geodesics to anatomical landmarks, curvatures, and surface normals.   

* **Free-form constraints (FFCs):** FFCs support has been added. Added a unit test for FFCs, a typical sphere unit test with a constraint that cuts the sphere like a tennis ball grove. Also added a unit test with two domains, both spheres in different locations. The first sphere has one cutting plane and 25 sphere constraints, and the second sphere has one cutting plane and one free form constraint. See [Free-Form Constraints](../new/free-form-constraints.md) for more details.

#### ShapeWorks Front-end

* **Multiple domains in Studio:** Support for multiple alignment strategies is now present in Studio. It allows analysis with and without articulation with a choice of reference domain or global alignment. See [Multiple Domain Alignments](../studio/multiple-domains.md#multiple-domain-alignments)

* **New analysis features in Studio:** Shape evaluation charts for compactness, specificity, and generalization have been added. See for [Metrics Panel](../studio/getting-started-with-studio.md#metrics-panel) more details.

* **Usability features in Studio:** New usability features such as group p-value display, feature map scalar control, surface opacity controls on a per doamin basis, message history window, suppressible error dialog, narrow band optimization parameter, multiple domain expore options (combined and support) and allow initial landmark points. See [New in ShapeWorks Studio 6.2](../new/new-studio.md#usability-features) for more details.

#### User's Support

* **Shape cohort generation:** Added segmentation and image generation for 2D contour supershapes. See this [Jupyter Notebook](../notebooks/getting-started-with-data-augmentation.ipynb) for more details.

* **Analyzing the group differences:** A new use case has been added, demonstrating the functionality of shape statistics tools to perform hypothesis testing of group shape differences. See [Femur: Group Difference Statistics in Python](../use-cases/stats-based/femur-pvalues.md) for more details.   

* **Sub-sampling for multiple domains:** We can now perform subsampling for multiple domains data by combining the individual shapes from all the domains and generating combined shapes. We perform a clustering-based subset selection on the combined shapes so that the subset is representative of the entire dataset and all domains. The representative subset of the specified sample size will be helpful to run through the SSM pipeline so that the use case runs faster and uses less memory.

* **API reference in Documentation:** Information about different classes (e.g Image), functions (e.g Image::antialias), function parameters, function return types in C++ API has been added. Check out [Groups](http://sciinstitute.github.io/ShapeWorks/api/Modules/index_groups.html), [Classes](http://sciinstitute.github.io/ShapeWorks/api/Classes/index_classes.html), [Namespaces](http://sciinstitute.github.io/ShapeWorks/api/Namespaces/index_namespaces.html), [Files](http://sciinstitute.github.io/ShapeWorks/api/Files/index_files.html) for more details.

#### Deep Learning

* **DeepSSM in Studio:** The ability to run DeepSSM has been added to Studio. See [DeepSSM in Studio](../studio/deepssm-in-studio.md) for more details.

### Improvements

#### ShapeWorks Back-end

* **Safe construction of `Image` instances in Python:** Images can now be safely constructed without copying and passed without copying from/to Python. In particular, passing Images for rendering using `pyvista` is now transparent and copy-free (note: copying is still supported if necessary). A [Jupyter Notebook](../notebooks/array-passing-without-copying.ipynb) was added to demonstrate `Image` initialization and passing.

* **Efficient data sharing between Python and C++:** Added efficient sharing of large data between Python and C++ (for both `Mesh` fields and `Image` data) that enables Python tools which access `Mesh` fields or `Image` data for visualization and analysis, or wish to create an Image or add fields from NumPy to a `Mesh`, to do so with optimal efficiency without fear of memory leak due to mishandled transfers. 

* **Improved Python APIs:** `Coordsys` of images in Python can now be set using `setCoordsys`. Fixed Python bindings and improved interactive help where necessary. More efficient conversion of shapeworks `Mesh` in Python to vtk mesh by creating a `PolyData` instead of performing IO operations.

* **Transforms in `Image` and `Mesh` API:** Separate transforms such as center of mass, rigid registration and thin plate spiline can be created. They are passed efficiently and transparently between Python and C++. These transform functions create and accept numpy arrays instead of using proprietary transform type in the Python API. A jupyter notebook was to demonstrate this for [images](../notebooks/create-and-apply-image-transforms.ipynb) and [meshes](../notebooks/create-and-apply-mesh-transforms.ipynb). 

* **Optimizer exports alignment transforms:** Write individual procrustes transforms. The ShapeWorks `Optimizer` can now export individual procrustes transform files if requested. Use the xml tag `<write_transform_files> 1 </write_transform_files>` to enable it. Additionally, for project spreadsheets (e.g., Studio), individual procrustes transforms will be added as additional columns in the data sheet. See [Optimize](../workflow/optimize.md) for more details.

* **Improved testing:** More robust testing of Python tests was achieved by adding code to ensure that all tests are performed for all functions, whereas before the test failure was reported immediately without testing if any related functions failed. Concrete seeding of random number generators (only for testing) is utilized to ensure objective comparison of results across all platforms. Improved verification of use case testing by comparing shape statistics compactness, generalization, and specificity against a good shape model instead of checking for a file. A log file (`verify.log`) is written that contains the use case outputs and summary.

#### User's Support

* **Use cases:** Added `--mesh_mode` option to image-based use cases. When running in mesh mode, after grooming segmentations, distance transforms are converted to meshes, and optimization is done directly on meshes, saving memory footprint and allowing the usage of geodesic distances for particle repulsion. This enables improved modeling for thin structures and high curvature regions. See [Use Case Documentation](../use-cases/use-cases.md#use-cases-arguments) for more details. 

* **Improved use cases documentation:** Restructured use cases documentation to reduce repetition and better highlight the focus and differences of each demonstration. Added relevant documentation links for every step and parameter in use case descriptions. Reorganized use case documentation based on category (i.e., mesh-based, constraint-based, etc.). See [Getting Started with Use Cases](../use-cases/use-cases.md) for information on running use cases and [Examples](../getting-started/examples.md) for an overview of released use cases. More explanation of interpreting modes of variation in [Shapes. What & From Where?](../getting-started/shapes.md).

* **Restructured `ellipsoid_evaluate` use case:** The computation time for calculating specificity, compactness, and generalization metrics have been dramatically reduced. A 50X speedup was experienced when calculating evaluation metrics for all modes for a dataset with 75 shapes, each with 1024 particles. We can now calculate the evaluation metrics for all the modes or query the values for a specified mode. The use case has been modified to demonstrate these functionalities. See [Ellipsoid: Shape Evaluation in Python](../use-cases/stats-based/ellipsoid-evaluate.md) for more details.


### Fixes

#### ShapeWorks Back-end

* **Memory leak:** Fixed memory leak issues in `FEMesh` operations

#### ShapeWorks Front-end

* **Fix processing of all orientation images:** We have fixed ShapeWorks to handle all orientations of images. Previously, only a subset (such as RAI) was fully compatible with all tools. 

==========================================
 ShapeWorks   v6.1.0 - Released June 2021
==========================================

The ShapeWorks software is an open-source distribution of a new method for
constructing compact statistical point-based models of ensembles of similar
shapes that does not rely on any specific surface parameterization. The method
requires very little preprocessing or parameter tuning and applies to a wide
range of shape analysis problems, including nonmanifold surfaces and objects of
arbitrary topology. The proposed correspondence point optimization uses an
entropy-based minimization that balances the simplicity of the model
(compactness) with the accuracy of the surface representations. The ShapeWorks
software includes tools for preprocessing data, computing point-based shape
models, and visualizing the results.

=====================

