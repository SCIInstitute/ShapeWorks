## Ellipsoid Use Case

The ellipsoid dataset comprises of axis-aligned ellipsoids which are varying along a singular axis. This example is a steeping stone for the user to get familiar with the workflow of ShapeWorks. The ellipsoid.py use case represent the standard use version of a shape modeling workflow using ShapeWorks. It includes the full pipeline for processed as well as unprocessed data. 

The use case is located at: [/Examples/Python](https://github.com/SCIInstitute/ShapeWorks/tree/master/Examples/Python)

To run the use case, run ellipsoid.py with proper tags. The tags control the type of input data and the optimization method.
* --start_with_prepped_data: just does the optimization on previously processed data
* --use_single_scale: to use the single scale optimization, the default is multiscale optimization
            
              python RunUseCase.py --use_case ellipsoid 

This calls ellipsoid.py which:
* Loads data (uses local data if it exists, otherwise data is automatically downloaded from SCI servers)
* Grooms the images and segmentations by calling methods in GroomUtils.py
* Optimizes particle distribution by calling methods in OptimizeUtils.py
* Opens View2  to visualize results by calling methods in AnalyzeUtils.py

### Grooming

These are pre-processing steps which are only performed when we start with unprepped data, i.e. the tag --start_with_prepped_data is not used.

For a description of the grooming tools and parameters, see: [Groom.md](../Workflow/Groom.md)
1. Isotropic Resampling - Both the image and mesh are resampled to have uniform voxel spacing. 
2. Apply Padding- Segmentations which lie on the image boundary will have a hole on that intersection. Padding is added to the images and segmentations prevent this.
3. Center of Mass Alignment - Center of mass alignment is performed before aligning the samples to a reference. This factors out translations reducing the risk of misalignment and allows for a median sample to be selected as the reference.
4. Reference Selection - The reference is selected by first getting the mean distance transform of the segmentations, then selecting the sample closest to that mean.
5. Rigid Alignment - All of the segmentations and images are now aligned to the reference using rigid alignment.
6. Crop - The images and segmentations are cropped so that all of the samples are within the same bounding box.
7. Distance Transform - Finally, the distance transform is taken and the data is ready for ShapeWorks optimize.

### Optimize
For a description of the optimize tools and parameters, see: [Optimize.md](../Workflow/Optimize.md)

* Single scale optimization uses the user defined number of particles for optimization and uses procrustes scaling to factor out size as a mode of variation. 
Below are the default optimization parameters for this use case. 

            "number_of_particles" : 128,
            "use_normals": 0,
            "normal_weight": 10.0,
            "checkpointing_interval" : 200,
            "keep_checkpoints" : 0,
            "iterations_per_split" : 100,
            "optimization_iterations" : 2000,
            "starting_regularization" : 100,
            "ending_regularization" : 0.1,
            "recompute_regularization_interval" : 2,
            "domains_per_shape" : 1,
            "relative_weighting" : 10,
            "initial_relative_weighting" : 0.01,
            "procrustes_interval" : 0,
            "procrustes_scaling" : 0,
            "save_init_splits" : 0,
            "debug_projection" : 0,
            "verbosity" : 3

* Multiscale optimization uses use defined starting number of particles and number of optimization levels, 
and the optimized particles of each level are used to initialize the next level particles. 
This method runs single scale optimization for each level and generates robust particle system.
The default values of this use case are as below.

            "starting_particles" : 32,
            "number_of_levels" : 3,
            "use_normals": 1,
            "normal_weight": 10.0,
            "checkpointing_interval" : 200,
            "keep_checkpoints" : 0,
            "iterations_per_split" : 100,
            "optimization_iterations" : 2000,
            "starting_regularization" : 100,
            "ending_regularization" : 0.1,
            "recompute_regularization_interval" : 2,
            "domains_per_shape" : 1,
            "relative_weighting" : 10,
            "initial_relative_weighting" : 0.01,
            "procrustes_interval" : 0,
            "procrustes_scaling" : 0,
            "save_init_splits" : 0,
            "debug_projection" : 0,
            "verbosity" : 3