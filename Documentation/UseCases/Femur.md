## Femur Use Case

##### Minimum of 32GB of RAM required to run full use case.

In this use case we start with full unsegmented images (CT scans) of the hip and segmented meshes of each femur.
ShapeWorks requires a binary volume format for input segmentations so these meshes must first be converted into binary volumes. Additionally, the corresponding unsegmented images need to be carried through each grooming step with the meshes so that they can be used for analysis.

The femur meshes in this data set have been segmented with various shaft lengths as can be seen below. In order to remove this variablity so that it is not captured in the shape model, the femurs are clipped using a cutting plane defined by the user. There are two ways to do so as explained below and they are differentiated by using the "--interactive" tag or not. 

![Femur Lengths](../images/femurLengths.png)

### Running the Use Case
The use case is located at: [/Examples/Python](https://github.com/SCIInstitute/ShapeWorks/tree/master/Examples/Python)

To run the use case, run:
            
              python RunUseCase.py --use_case femur
              
This calls femur.py which:
* Loads data (uses local data if it exists, otherwise data is automatically downloaded from SCI servers)
* Grooms the images and meshes by calling methods in GroomUtils.py
* Optimizes particle distribution by calling methods in OptimizeUtils.py
* Opens View2  to visualize results by calling methods in AnalyzeUtils.py

#### Running without the --interactive tag

If the --interactive tag is not used, the user will select the cutting plane in the beggining of the grooming steps on a sample of their choice.

The user will be prompted with “Type the prefix of the sample you wish to use to select the cutting plane from listed options and press enter.” and the options are listed. After they've typed the chosen sample prefix, an interactive window will pop up in which they can select the cutting plane. When the user is content with their selection, they simply close this window and the grooming process will continue. This process can be seen below.

![Alt Text](../images/notInteractiveFemur.gif)

Note that internally, whatever transformations are done to the sample the user has defined the cutting plane on will be done to the cutting plane as well, so that when it is time to clip the samples the cutting plane is still well defined. 

#### Running with the --interactive tag
To run the use case interactively, either run:
            
              python RunUseCase.py --use_case femur --interactive

If the --interactive tag is used, the user will be asked to select a cutting plane for the femur shaft in the middle of the grooming process. Once the reference sample for alignment has been selected, an interactive window will pop up with the reference sample and the user can define the cutting plane. Closing the window will continue the grooming process. 

![Alt Text](../images/interactiveFemur.gif)

### Grooming
For a description of the grooming tools and parameters, see: [Groom.md](https://github.com/SCIInstitute/ShapeWorks/blob/master/Documentation/Workflow/Groom.md)

The steps are described below and the results of each step are shown for the meshes (note every step is perfmored on both the meshes the images although the resulting images are not shown here).

0. Define cutting plane either in beginning or after rigid alignment using the interactive window. 

            Within this window:

            - Zoom in and out by scrolling
 
            - Rotate view point by clicking in the surrounding area
 
            - Move cutting plane by clicking on it and dragging
 
            - Change normal vector (represented with arrow) by clicking on it and dragging
  
1. Reflect - In this use case we often have both right and left femur surface meshes. We want to align all the femurs, so we choose one side to reflect both the image and mesh.
2. Meshes to Volumes - Meshes must be turned into binary volumes using rasterization. The corresponding image origin, size, and spacing are used to generate the volume. 
3. Isotropic Resampling - Both the image and mesh are resampled to have uniform voxel spacing. 
4. Apply Padding- Segmentations which lie on the image boundary will have a hole on that intersection. Padding is added to the images and segmentations prevent this.
5a. Center of Mass Alignment - Center of mass alignment is performed before aligning the samples to a reference. This factors out translations reducing the risk of misalignment and allows for a median sample to be selected as the reference. Images are then centered so the origin is in the center.
5b. Centering - The samples must be centered before they are aligned to a reference. This step can be performed with Isotropic Resampling as it is in the left atrium use case. In the Femur use case we do so separately so that we can get the translation and apply it to the cutting plane if it has already been selected.
6. Reference Selection - The reference is selected by first getting the mean distance transform of the segmentations, then selecting the sample closest to that mean.
7. Rigid Alignment - All of the segmentations and images are now aligned to the reference using rigid alignment.
8. Clip Segmentations - Because the femur segmentations vary in shaft lengths, we use the defined cutting plane to clip them so only the region of interest remains.
9. Crop - The images and segmentations are cropped so that all of the samples are within the same bounding box.
10. Distance Transform - Finally the distance transform it taken and the data is ready for ShapeWorks optimize.

![Grooming steps](../images/FemurGroomPipeline.PNG)

### Optimize
For a description of the optimize tools and parameters, see: [Optimize.md](https://github.com/SCIInstitute/ShapeWorks/blob/master/Documentation/Workflow/Optimize.md)

Single scale optimization is used with procrustes scaling to factor out size as a mode of variation. Below are the default optimization parameters for this use case. 

            "number_of_particles" : 1024, 
            "use_normals": 0,
            "normal_weight": 10.0,
            "checkpointing_interval" : 10,
            "keep_checkpoints" : 1,
            "iterations_per_split" : 4000,
            "optimization_iterations" : 4000,
            "starting_regularization" : 100,
            "ending_regularization" : 0.1,
            "recompute_regularization_interval" : 2,
            "domains_per_shape" : 1,
            "relative_weighting" : 10,
            "initial_relative_weighting" : 1,
            "procrustes_interval" : 1,
            "procrustes_scaling" : 1,
            "save_init_splits" : 1,
            "debug_projection" : 0,
            "verbosity" : 3,
            "use_statistics_in_init" : 0

### Analyze

The particle based model for the mean shape and samples and the primary modes of variation are visualized using ShapeWorksStudio.
For more information see: [Analyze.md](https://github.com/SCIInstitute/ShapeWorks/blob/master/Documentation/Workflow/Analyze.md)

#### Mean Shape
![Alt Text](../images/mean_femur.gif)

#### Samples
![Zoom Femur Samples](../images/femur_samples_zoom.png)
![Femur Samples](../images/femur_samples.png)

#### PCA
![Alt Text](../images/femur_PCA.gif)
