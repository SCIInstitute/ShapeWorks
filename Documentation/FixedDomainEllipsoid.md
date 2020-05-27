## Fixed Domains Ellipsoid Use Case

This use case is designed for the fixed domains functionality of ShapeWorks. The fixed domains are used for the cases where we need to place correspondences over new shapes using a pre-existing shape model. In this example we use the example dataset in Ellipsoids_FD.zip which contains a previously generated shape model on ellipsoids, plus prepped segmentations of two new ellipsoids scans.

To run the use case, run ellipsoid_fd.py with proper tags. This optimization is done only with asingle scale support.
            
              python RunUseCase.py --use_case ellipsoid_fd

This calls ellipsoid_fd.py which:
* Loads data (uses local data if it exists, otherwise data is automatically downloaded from SCI servers)
* Loads the exisitng shape model and converts the new segmentations into distance transforms.
* Optimizes particle distribution by calling in OptimizeUtils.py
* Opens View2 to visualize results by calling methods in AnalyzeUtils.py

### Optimize

For a description of the optimize tools and parameters, see: [Optimize.md](https://github.com/SCIInstitute/ShapeWorks/blob/master/Documentation/Optimize.md)

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
        "verbosity" : 3,
        "number_fixed_domains": len(fileListDT),
        "fixed_domain_model_dir": shapemodelDir,
        "mean_shape_path": meanShapePath,
    

The meanShapePath and shapemodelDir are specified in the example itself, but they correspond to points which we initialize the new shape and the directory path to the new shape model.


