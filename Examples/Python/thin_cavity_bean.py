# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Geodesics Distances in ShapeWorks
====================================================================

In this example we provide a full pipeline with an example dataset of ellipsoids with
a thin cavity translating over the x axis
"""
import os
import glob
import subprocess
import shapeworks as sw

def Run_Pipeline(args):
    print("\nStep 1. Acquire Data\n")
    """
    Step 1: ACQUIRE DATA

    We define dataset_name which determines which dataset to download from 
    the portal and the directory to save output from the use case in. 
    """
    dataset_name = "thin_cavity_bean"
    output_directory = "Output/thin_cavity_bean/"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    # If running a tiny_test, then download subset of the data
    if args.tiny_test:
        dataset_name = "thin_cavity_bean_tiny_test"
        args.use_single_scale = 1
        sw.download_dataset(dataset_name, output_directory)
        dataset_name = "thin_cavity_bean"
        mesh_files = sorted(glob.glob(output_directory +
                            dataset_name + "/meshes/*.ply"))[:3]
    # else download the entire dataset
    else:
        sw.download_dataset(dataset_name, output_directory)
        mesh_files = sorted(glob.glob(output_directory +
                            dataset_name + "/meshes/*.ply"))

        # Select data if using subsample
        if args.use_subsample:
            sample_idx = sw.data.samplemeshes(mesh_files, int(args.num_subsample))
            mesh_files = [mesh_files[i] for i in sample_idx]
    
    """
    This dataset is prealigned and does not require any grooming steps.
    We loop over the files and load the meshes.
    """
    
    mesh_list = []
    for mesh_filename in mesh_files:
        print('Loading: ' + mesh_filename)
        # Get mesh
        mesh = sw.Mesh(mesh_filename)
        mesh_list.append(mesh)

    print("\nStep 2. Optimize with Geodesics\n")
    """
    Step 2: OPTIMIZE 

    Now we can run optimization directly on the meshes using geodesics.
    For more details on the plethora of parameters for shapeworks please refer 
    to docs/workflow/optimze.md
    http://sciinstitute.github.io/ShapeWorks/workflow/optimize.html
    """

     # Create project spreadsheet
    project_location = output_directory
    if not os.path.exists(project_location):
        os.makedirs(project_location)
    # Set subjects
    subjects = []
    number_domains = 1
    for i in range(len(mesh_list)):
        subject = sw.Subject()
        subject.set_number_of_domains(number_domains)
        rel_mesh_files = sw.utils.get_relative_paths([os.getcwd() + '/' + mesh_files[i]], project_location)
        subject.set_original_filenames(rel_mesh_files)
        #groomed file is same as input file
        subject.set_groomed_filenames(rel_mesh_files)
        subjects.append(subject)
    # Set project
    project = sw.Project()
    project.set_subjects(subjects)
    parameters = sw.Parameters()

    # Create a dictionary for all the parameters required by optimization
    parameter_dictionary = {
        "number_of_particles": 1024,
        "use_normals": 1,
        "normals_strength": 10.0,
        "checkpointing_interval": 200,
        "keep_checkpoints": 0,
        "iterations_per_split": 300,
        "optimization_iterations": 2500,
        "starting_regularization": 3000,
        "ending_regularization": 0.1,
        "relative_weighting": 15,
        "initial_relative_weighting": 0.01,
        "procrustes_interval": 0,
        "procrustes_scaling": 0,
        "save_init_splits": 0,
        "verbosity": 0,
        "use_geodesic_distance": 1,
    }
    # If running a tiny test, reduce some parameters
    if args.tiny_test:
        parameter_dictionary["number_of_particles"] = 32
        parameter_dictionary["optimization_iterations"] = 50
        parameter_dictionary["iterations_per_split"] = 50
    # Run multiscale optimization unless single scale is specified
    if not args.use_single_scale:
        parameter_dictionary["multiscale"] = 1
        parameter_dictionary["multiscale_particles"] = 32

    # Add param dictionary to spreadsheet
    for key in parameter_dictionary:
        parameters.set(key,sw.Variant([parameter_dictionary[key]]))
    project.set_parameters("optimize",parameters)
    spreadsheet_file = output_directory + "thin_cavity_bean_" + args.option_set+ ".swproj"
    project.save(spreadsheet_file)

    # Run optimization
    optimize_cmd = ('shapeworks optimize --progress --name ' + spreadsheet_file).split()
    subprocess.check_call(optimize_cmd)

    # If tiny test or verify, check results and exit
    sw.utils.check_results(args, spreadsheet_file)

    print("\nStep 3. Analysis - Launch ShapeWorksStudio")
    """
    Step 3: ANALYZE - open in studio
    For more information about the analysis step, see:
    # http://sciinstitute.github.io/ShapeWorks/workflow/analyze.html
    """
    analyze_cmd = ('ShapeWorksStudio ' + spreadsheet_file).split()
    subprocess.check_call(analyze_cmd)
