## Left Atrium Use Case

##### Minimum of 32GB of RAM required to run full use case.

The goal of this use case is using the ShapeWorks functionality to groom different types of input data (MRI and segmentation images),
and test two methods to optimize the particles.
ShapeWorks needs binary input images for grooming and prepares the data for optimization, in this use case we use the grooming 
functionality of ShapeWorks to groom the raw input images(MRI/CT) alongside with binary segmentations.
This example shows how to process the data (MRI and binary images) with the same parameters for downstream tasks.
There are multiple steps in the grooming stage to have proper data for optimization, these steps change the origin, 
voxel size, image size, orientation, etc. of input data. Although we don't need the raw images for the optimization, 
we might need them for any downstream task or warping the optimized particle system. 
For this purpose, we need to be consistent in term of changes between segmentation and raw files. 
In each step of grooming, we use the segmentation files to find the grooming parameters such as finding reference image 
for alignment or the bounding box for cropping, then we save them in a TXT file and use the same set of parameters 
to groom the raw images. At the end of this stage, we have groomed segmentations to convert to distance transforms 
and run the optimization and also, grooming parameters, and groomed raw files.
For this use case, we have 58 MRI images and their corresponding binary segmentation of left atrium(more details of 
[data](https://www.insight-journal.org/midas/collection/view/197))

This use case also demonstrates evaluation of different methods for optimization. 
We optimized the particle system with two different optimization methods available in ShapeWorks,
 single scale and multi-scale optimization. 
The single scale optimization takes a fixed number of particles (usually a power of 2 ) and performs the initialization step,
 then performs ShapeWorks optimization.  The multi-scale optimization runs the single scale optimization for different 
 number of particles in a hierarchical manner. It starts with a low number of particles, runs the single scale optimization
  until convergence, then uses optimum particles for the initialization of next level and increases particles by 
  the power of two at each level to reach the desired number of particles.

For the dataset like left atrium with lots of variation, it is hard  to tune parameters of single scale optimization. 
The multi-scale optimization is providing the optimize particle system at each level, so we can use them to tune the parameters
 and have more stable particle system for the desired number of particles.


### Running Left Atrium
The use case is located at: [/Examples/Python](https://github.com/SCIInstitute/ShapeWorks/tree/master/Examples/Python)

To run the use case, run LAMain.py with proper tags. The tags control the type of input data and the optimization method.
* --start_with_image_and_segmentation_data: to groom raw images as well as segmentation, the default is only segmentation
* --use_single_scale: to use the single scale optimization, the default is multiscale optimization
            
              python RunUseCase.py --use_case left_atrium 

This calls left_atrium.py which:
* Loads data (uses local data if it exists, otherwise data is automatically downloaded from SCI servers)
* Grooms the images and segmentations by calling methods in GroomUtils.py
* Optimizes particle distribution by calling methods in OptimizeUtils.py
* Opens ShapeWorksStudio  to visualize results by calling methods in AnalyzeUtils.py

### Grooming
For a description of the grooming tools and parameters, see: [Groom.md](https://github.com/SCIInstitute/ShapeWorks/blob/master/Documentation/Workflow/Groom.md)
1. Isotropic Resampling - Both the image and mesh are resampled to have uniform voxel spacing. 
2. Apply Padding- Segmentations which lie on the image boundary will have a hole on that intersection. Padding is added to the images and segmentations prevent this.
3. Center of Mass Alignment - Center of mass alignment is performed before aligning the samples to a reference. This factors out translations reducing the risk of misalignment and allows for a median sample to be selected as the reference.
4. Reference Selection - The reference is selected by first getting the mean distance transform of the segmentations, then selecting the sample closest to that mean.
5. Rigid Alignment - All of the segmentations and images are now aligned to the reference using rigid alignment.
6. Crop - The images and segmentations are cropped so that all of the samples are within the same bounding box.
7. Distance Transform - Finally, the distance transform is taken and the data is ready for ShapeWorks optimize.

![left Atrium Groom](../images/leftatrium_groom.png)
### Optimize
For a description of the optimize tools and parameters, see: [Optimize.md](https://github.com/SCIInstitute/ShapeWorks/blob/master/Documentation/Workflow/Optimize.md)

* Single scale optimization uses the user defined number of particles for optimization and uses procrustes scaling to factor out size as a mode of variation. 
Below are the default optimization parameters for this use case. 

            "number_of_particles" : 1024, 
            "use_normals": 1,
            "normal_weight": 10.0,
            "checkpointing_interval" : 200,
            "keep_checkpoints" : 0,
            "iterations_per_split" : 4000,
            "optimization_iterations" : 4000,
            "starting_regularization" : 50000,
            "ending_regularization" : 0.1,
            "recompute_regularization_interval" : 2,
            "domains_per_shape" : 1,
            "relative_weighting" : 50,
            "initial_relative_weighting" : 0.1,
            "procrustes_interval" : 0,
            "procrustes_scaling" : 1,
            "save_init_splits" : 0,
            "debug_projection" : 0,
            "verbosity" : 3

#### Mean Shape
![left Atrium singleScale](../images/leftatrium_singlescale.png)

#### PCA
![left Atrium singleScale PCA mode](../images/leftatrium_singlescale_pca.gif)

* Multiscale optimization uses use defined starting number of particles and number of optimization levels, 
and the optimized particles of each level are used to initialize the next level particles. 
This method runs single scale optimization for each level and generates robust particle system.
The default values of this use case are as below.

            "starting_particles" : 128,
            "number_of_levels" : 4,
            "use_normals": 1,
            "normal_weight": 10.0,
            "checkpointing_interval" : 200,
            "keep_checkpoints" : 0,
            "iterations_per_split" : 4000,
            "optimization_iterations" : 4000,
            "starting_regularization" : 50000,
            "ending_regularization" : 0.1,
            "recompute_regularization_interval" : 2,
            "domains_per_shape" : 1,
            "relative_weighting" : 50,
            "initial_relative_weighting" : 0.1,
            "procrustes_interval" : 0,
            "procrustes_scaling" : 1,
            "save_init_splits" : 0,
            "debug_projection" : 0,
            "verbosity" : 3

#### Mean Shape          
![left Atrium multiScale](../images/leftatrium_multiscale.png)

#### PCA
![left Atrium singleScale PCA mode](../images/leftatrium_multiScale_pca.gif)


### Analyze

The particle based model for the mean shape and samples and the primary modes of variation are visualized using ShapeWorksStudio.
For more information see: [Analyze.md](https://github.com/SCIInstitute/ShapeWorks/blob/master/Documentation/Workflow/Analyze.md)            
