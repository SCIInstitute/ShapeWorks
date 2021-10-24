### What is new?

Shireen: General comment, whenever applicable, add links to documentation and/or use cases that demonstrate the utiltity and usage of new/improved/fixed tools. Putting together release notes also allow us to find what is missing in our online documentation, so please work with relevant team members to add documentation for every item listed below.   

Shireen: Another general comment, please be consistent with capatilization. I will fix some, but i would double check.

#### ShapeWorks Back-end

* **New mesh grooming tools:** curvature, cvdDecimate, fixElement, geodesicDistance between 2 points on mesh, geodesicDistance between all points on mesh to another point (landmark), geodesicDistance between all points on mesh and set of points (curve), computeMeanNormals

Shireen:     
    (a) cvdDecimate will be revised to use ACVD in place of FEMesh lib that crashes and does not handle nonmanifold meshes. Let's remove it from the notes for this release and add it to 6.3.    
    (b) use `command/tool name` convention to differentiate notes text from tools introduced.   
    (c) add a high-level description for each new tool added, e.g., curvature and computeMeanNormals do not have any description here.   
    (d) I would comment on how these tools can enable further use cases whenever applicable, e.g., geodesic functions can enable feature-based correspondences where you can cite Datar's paper that I shared earlier.   

* **New mesh query/operator tools:** operator+=, closest point, closestPointId, points (list of all points on mesh), faces (list of all faces on mesh, getFace

Shireen: Our release notes are not meant to just list what has been done, think of it for an advertizing perspective. Please add a decription to each of the new tools, use `tool name` convention, and comment on the utility of these tools.    
    
* **Mesh fields:** Added support for passing of multi-value fields in addition to scalar fields.   

Shireen: Please comment on utility and/or refer to where fields are used if applicable.   

* **Free-Form constraints:** FFCs support has been added. Added a unit test for FFCs, a typical sphere unit test with a constraint that cuts the sphere like a tennis ball grove. Also added a unit test with two domains, both spheres in different locations. The first sphere has one cutting plane and 25 sphere constraints, the second sphere has one cutting plane and one free form constraint.

Shireen: Did we add documentation for the FFCs feature? If not, please work with Hong to add this in and refer to it in the release notes. As of now, FFCs are not fully developed in terms of the full pipeline (e.g. grooming and annotating constraints are not yet developed). So, I would tailor the documentation on how users can now use such feature.    

#### ShapeWorks Front-end

* **Studio:** DeepSSM, Multiple Domain Alignments, Shape Evaluation Charts, Group p-value Display, Feature map scalar control, Opacity controls, Expose narrow band parameter, Added log window, Added new suppressible error dialog, Multiple domain export options (combined + separate), Allow initial landmark points.    


Shireen: This item has to be further divided into (a) DeepSSM in Studio, (b) Multiple domains in Studio, (c) New analysis features in Studio, and (d) Usability features in Studio. Each item should have a more detailed description of what has been added, not just a list and refer to where more details can be found in the documentation.    


* **Optimizer exports alignment transforms:** Write individual procrustes transforms. The ShapeWorks Optimizer can now export individual procrustes transform files if requested. Use the xml tag `<write_transform_files> 1 </write_transform_files>` to enable it. Additinally, for project spreadsheets (e.g., Studio), individual procrustes transforms will be added as additional columns in the data sheet.   

Shireen: add links to where more details can be found in the documentation. 

#### User's Support

* **Shape cohort generation:** Added segmentation and image generation for 2D contour supershapes.   

Shireen: add links to where more details can be found in the documentation. 


* **Analyzing the group differences:** A new use case has been added, demonstrating the functionality of shape statistics tools to perform hypothesis testing of group shape differences. This use case analyzes the femur shape model obtained from running the Femur: SSM from Meshes use case. Separate statistical tests for differences in correspondence positions are applied to every correspondence index. We use Hotelling $T^2$ metric(nonparametric permutation test) with false discovery rate correction (FDR) for multiple comparisons. This method helps identify and visualize localized regions of significant shape differences.   


Shireen:     
    (a) add links to where more details can be found in the documentation.     
    (b) The first statement is the right level of detail for release notes, the rest of the text should move to the documentation and here just link to it.    
    (c) "Femur: SSM from Meshes" should this be linked to the documentation?    


* **Sub-sampling for multiple domains:** We can now perform subsampling for multiple domains data by combining the individual shapes from all the domains and generating combined shapes. We perform a clustering-based subset selection on the combined shapes so that the subset is representative of the entire dataset and all domains.

Shireen: Please add a comment on how this can be useful.

### Improvements

#### ShapeWorks Back-end
* **Image API:** Images can now be safely constructed without copying and passed without copying from/to Python. In particular, passing Images for rendering using pyvista is now transparent and copy-free (note: copying is still supported if necessary). Example jupyter notebook added to demonstrate Image initialization and passing.

Shireen:     
    (a) should we use `Image` rather than Image here?    
    (b) please link to the jupyter notebook in the documention here

* **Mesh API:** Copy-free bidirectional passing of mesh fields

Shireen: add more context/details and how is this helpful. 


* **Python API:** Coordsys of images in Python can now be set. More robust Python bindings of shapeworks API with interactive help. More efficient conversion of shapeworks mesh to vtk mesh


Shireen: please use `something` convention for classes, APIs, ... etc. What do you mean by more robust bindings and more efficient conversions? can you add more context here?


* **Testing:** More robust testing of Python tests. Reproducibility of functionality using randoms and performance testing

Shireen: same here.  


#### ShapeWorks Front-end

#### User's Support

* **Use cases:** Added `--mesh_mode` option to image-based use cases. When running in mesh mode, after grooming segmentations, distance transforms are converted to meshes and optimization is done directly on meshes, saving memory footprint and allowing the usage of geodesic distances for particle repulsion. This enables improved modeling for thin structures and high curvature regions.

Shireen: add links to where more details can be found in the documentation. 

* **Documentation:** Improved use case documentation and explanation of modes of variation.

Shireen: add links to where more details can be found in the documentation. 

* **Restructured `ellipsoid_evaluate` use case:** The computation time for calculating specificity, compactness, and generalization metrics has been reduced. We can now calculate the evaluation metrics for all the modes or query the values for a specified mode. The use case has been modified to demonstrates these functionalities.

Shireen:     
    (a) add links to where more details can be found in the documentation.     
    (b) can we add time savings/speedups here? (a rough estimate should work)

### Fixes

#### ShapeWorks Back-end

* **Memory Leak:** Fixed memory leak issues in `FEMesh` operations

#### ShapeWorks Front-end

* **Fix processing of all orientation images:** We have fixed ShapeWorks to handle all orientations of images. Previously, only a subset were fully compatibile with all tools.

Shireen: add links to where more details can be found in the documentation. Also, add which orientations were supported previously.

