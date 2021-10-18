### What is new?

#### ShapeWorks Back-end

* **New Mesh grooming tools:** curvature, cvdDecimate, fixElement, geodesicDistance between 2 points on mesh, geodesicDistance between all points on mesh to another point (landmark), geodesicDistance between all points on mesh and set of points (curve), computeMeanNormals
* **New mesh query/operator tools:** operator+=, closest point, closestPointId, points (list of all points on mesh), faces (list of all faces on mesh, getFace
* **Mesh Fields:** Added support for passing of multi-value fields in addition to scalar fields
* **Free-Form Constraints:** FFCs support has been added. Added a unit test for FFCs, a typical sphere unit test with a constraint that cuts the sphere like a tennis ball grove. Also added a unit test with two domains, both spheres in different locations. The first sphere has one cutting plane and 25 sphere constraints, the second sphere has one cutting plane and one free form constraint.

#### ShapeWorks Front-end

* **Studio:** DeepSSM, Multiple Domain Alignments, Shape Evaluation Charts, Group p-value Display, Feature map scalar control, Opacity controls, Expose narrow band parameter, Added log window, Added new suppressible error dialog, Multiple domain export options (combined + separate), Allow initial landmark points
* **Optimizer:** Write individual procrustes transforms. The ShapeWorks Optimizer can now export individual procrustes transform files if requested. Use the xml tag <write_transform_files> 1 </write_transform_files> to enable it. Additinally, for project spreadsheets (e.g. Studio), individual procrustes transforms will be added as additional columns in the data sheet.

#### User's Support

* **Shape Cohort Generation:** Added segmentation and image generation for 2D contour supershapes.
* **Femur use case analyzing the group differences:** A new use case has been added, demonstrating the functionality of shape statistics tools to perform hypothesis testing of group shape differences. This use case analyzes the femur shape model obtained from running the Femur: SSM from Meshes use case. Separate statistical tests for differences in correspondence positions are applied to every correspondence index. We use Hotelling $T^2$ metric(nonparametric permutation test) with false discovery rate correction (FDR) for multiple comparisons. This method helps identify and visualize localized regions of significant shape differences.
* **Sub-sampling for multiple domains:** We can now perform subsampling for multiple domains data by combining the individual shapes from all the domains and generating combined shapes. We perform a clustering-based subset selection on the combined shapes so that the subset is representative of the entire dataset and all domains.

### Improvements

#### ShapeWorks Back-end
* **Image API:** Images can now be safely constructed without copying and passed without copying from/to Python. In particular, passing Images for rendering using pyvista is now transparent and copy-free (note: copying is still supported if necessary). Example jupyter notebook added to demonstrate Image initialization and passing
* **Mesh API:** Copy-free bidirectional passing of mesh fields
* **Python API:** Coordsys of images in Python can now be set. More robust Python bindings of shapeworks API with interactive help. More efficient conversion of shapeworks mesh to vtk mesh
* **Testing:** More robust testing of Python tests. Reproducibility of functionality using randoms and performance testing

#### ShapeWorks Front-end

#### User's Support

* **Use Cases:** Added --mesh_mode option to image-based use cases. When running in mesh mode, after grooming distance transforms are converted to meshes and optimization is done on meshes 
* **Documentation:** Improved use case documentation and explanation of modes of variation.
* **Restructured ellipsoid_evaluate use case:** The computation time for calculating specificity, compactness, and generalization has been reduced. We can now calculate the evaluation metrics for all the modes or query the values for a specified mode. The use case has been modified to demonstrates these functionalities.

### Fixes

#### ShapeWorks Back-end

* **Memory Leak:** Fixed memory leak issues in FEMesh operations

#### ShapeWorks Front-end

* **Fix processing of all orientation images:** We have fixed ShapeWorks to handle all orientations of images. Previously, only a subset were fully compatibile with all tools.
