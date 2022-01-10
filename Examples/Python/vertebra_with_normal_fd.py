# -*- coding: utf-8 -*-
"""
=====================================================================
Fixed Domains Example for Statistical Shape Modeling using ShapeWorks 
=====================================================================

In this example we work with a dataset of axis aligned ellipsoids. 
This examples is a use case for fixed domains, i.e. we have an existing 
shape model using some ellipsoids and we want to place correspondences 
on new ellisoids (we are provided fully prepped binary images) according 
to the existing shape model.
"""
import os
import glob
import shapeworks as sw
import OptimizeUtils
import AnalyzeUtils

def Run_Pipeline():
    print("\nStep 1. Extract Data\n")
    """
    Step 1: EXTRACT DATA

    We define dataset_name which determines which dataset to download from 
    the portal and the directory to save output from the use case in. 
    """
    output_directory = "/home/sci/nawazish.khan/VerSe_Dataset/Final-SSM-Models/Lumbarr/L2-new/L2-fixed-domain/"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    file_list_meshes = sorted(glob.glob(
        output_directory + "fixed-domain-meshes/*.vtk"))
    file_list_new_meshes = sorted(
        glob.glob(output_directory + "/new-meshes/*.stl"))
    
    # Get list of original and new distance transforms
    mesh_files = file_list_meshes + file_list_new_meshes

    print("\nStep 3. Optimize - Particle Based Optimization\n")

    # Make directory to save optimization output
    point_dir = output_directory + 'shape_models/'
    if not os.path.exists(point_dir):
        os.makedirs(point_dir)

    shape_model_dir = output_directory + "/fixed-domain-particles/512/"
    OptimizeUtils.findMeanShape(shape_model_dir)
    mean_shape_path = shape_model_dir + '/meanshape_local.particles'

    # Create a dictionary for all the parameters required by optimization
    parameter_dictionary = {
        "number_of_particles": 512,
        "use_normals": 1,
        "normal_weight": 1.0,
        "checkpointing_interval": 200,
        "keep_checkpoints": 0,
        "iterations_per_split": 2000,
        "optimization_iterations": 1000,
        "starting_regularization": 1000,
        "ending_regularization": 5,
        "recompute_regularization_interval": 2,
        "domains_per_shape": 1,
        "domain_type": 'mesh',
        "relative_weighting": 4,
        "initial_relative_weighting": 0.5,
        "procrustes_interval": 3,
        "procrustes_scaling": 1,
        "save_init_splits": 0,
        "verbosity": 2,
        "number_fixed_domains": len(file_list_meshes),
        "fixed_domain_model_dir": shape_model_dir,
        "mean_shape_path": mean_shape_path,
    }

    # Execute the optimization function
    [local_point_files, world_point_files] = OptimizeUtils.runShapeWorksOptimize_FixedDomains(
        point_dir, mesh_files, parameter_dictionary, mesh=True)

    # Prepare analysis XML
    analyze_xml = point_dir + "/lumbar_analyze.xml"
    AnalyzeUtils.create_analyze_xml(analyze_xml, mesh_files, local_point_files, world_point_files)

    AnalyzeUtils.launch_shapeworks_studio(analyze_xml)


Run_Pipeline()