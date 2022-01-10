# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Statistical Shape Modeling with ShapeWorks
====================================================================
This example is set to serve as a test case for new ShapeWorks users, and each
step is explained in the shapeworks including the pre-processing, the 
optimization and, the post ShapeWorks visualization.

First import the necessary modules
"""
import os
import glob
import shapeworks as sw
import OptimizeUtils
import AnalyzeUtils

def Run_Pipeline(args):
    print("\nStep 1. Extract Data\n")
    """
    Step 1: EXTRACT DATA

    We define dataset_name which determines which dataset to download from 
    the portal and the directory to save output from the use case in. 
    """
    
    dataset_name = "ellipsoid_joint_size_aligned"
    output_directory = "Output/ellipsoid_multiple_domain_mesh/"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)


    # If running a tiny_test, then download subset of the data
    if args.tiny_test:
        sw.data.download_subset(
            args.use_case, dataset_name, output_directory)
        mesh_files = sorted(glob.glob(output_directory +
                                     dataset_name + "/meshes/*.vtk"))[:6]
    # Else download the entire dataset
    else:
        sw.data.download_and_unzip_dataset(dataset_name, output_directory)
        mesh_files = sorted(glob.glob(output_directory +
                                     dataset_name + "/meshes/*.vtk"))

        if args.use_subsample:
            inputMeshes =[sw.Mesh(filename) for filename in mesh_files]
            sample_idx = sw.data.sample_meshes(inputMeshes, int(args.num_subsample),domains_per_shape=2)
            mesh_files = [mesh_files[i] for i in sample_idx]

    # This dataset is prealigned and does not require any grooming steps.

    print("\nStep 2. Optimize - Particle Based Optimization\n")
    """
    Step 2: OPTIMIZE - Particle Based Optimization

    Now we can run optimization directly on the meshes.
    For more details on the plethora of parameters for shapeworks please refer 
    to docs/workflow/optimze.md
    http://sciinstitute.github.io/ShapeWorks/workflow/optimize.html
    """

    # Make directory to save optimization output
    point_dir = output_directory + 'shape_models/' + args.option_set
    if not os.path.exists(point_dir):
        os.makedirs(point_dir)
    # Create a dictionary for all the parameters required by optimization
    parameter_dictionary = {
        "number_of_particles" : [512,512],
        "use_normals": [1,1],
        "normal_weight": [10.0,10.0],
        "checkpointing_interval" : 200,
        "keep_checkpoints" : 0,
        "iterations_per_split" : 500,
        "optimization_iterations" : 500,
        "starting_regularization" :3000,
        "ending_regularization" : 1,
        "recompute_regularization_interval" : 1,
        "domains_per_shape" : 2,
        "domain_type" : 'mesh',
        "relative_weighting" : 10, 
        "initial_relative_weighting" : 0.1,
        "procrustes_interval" : 0,
        "procrustes_scaling" : 0,
        "save_init_splits" : 0,
        "verbosity" : 0

      }

    if args.tiny_test:
        parameter_dictionary["number_of_particles"] = [32,32]
        parameter_dictionary["optimization_iterations"] = 25

    # Execute the optimization function
    [local_point_files, world_point_files] = OptimizeUtils.runShapeWorksOptimize(
        point_dir, mesh_files, parameter_dictionary)

    # Prepare analysis XML
    analyze_xml = point_dir + "/ellipsoid_multiple_domain_mesh_analyze.xml"
    domains_per_shape = 2
    AnalyzeUtils.create_analyze_xml(analyze_xml, mesh_files, local_point_files, world_point_files, domains_per_shape)

    # If tiny test or verify, check results and exit
    AnalyzeUtils.check_results(args, world_point_files)

    print("\nStep 3. Analysis - Launch ShapeWorksStudio - sparse correspondence model.\n")
    """
    Step 3: ANALYZE - Shape Analysis and Visualization

    Now we launch studio to analyze the resulting shape model.
    For more information about the analysis step, see docs/workflow/analyze.md
    http://sciinstitute.github.io/ShapeWorks/workflow/analyze.html
    """
    AnalyzeUtils.launch_shapeworks_studio(analyze_xml)