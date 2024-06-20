# -*- coding: utf-8 -*-
"""
=====================================================================
Fixed Domains Example for Statistical Shape Modeling using ShapeWorks 
=====================================================================

In this example we work with a dataset of axis aligned ellipsoids. 
This example is a use case for fixed domains, i.e. we have an existing
shape model using some ellipsoids, and we want to place correspondences
on new ellipsoids (we are provided fully prepped binary images) according
to the existing shape model.
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
    dataset_name = "ellipsoid_fd_multiscale"
    output_directory = "Output/ellipsoid_fd/"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)
    sw.portal.download_dataset(dataset_name, output_directory)
    dataset_name = "ellipsoid_1mode_aligned"
    file_list_segs = sorted(glob.glob(output_directory + dataset_name + "/segmentations/fd_*.nrrd"))
    file_list_dts = sorted(glob.glob(output_directory + dataset_name + "/groomed/distance_transforms/*.nrrd"))
    file_list_new_segs = sorted(glob.glob(output_directory + dataset_name + "/segmentations/new_*.nrrd"))

    print("\nStep 2. Groom - Create distance transforms\n")
    """
    Step 2: GROOMING 
    
    The new segmentations are pre-aligned so the only grooming step 
    required is to convert them to distance transforms.

    For more information on grooming see docs/workflow/groom.md
    http://sciinstitute.github.io/ShapeWorks/workflow/groom.html
    """

    # Create a directory for groomed output
    groom_dir = output_directory + 'groomed/'
    if not os.path.exists(groom_dir):
        os.makedirs(groom_dir)

    """
    Converting segmentations to smooth signed distance transforms.
    The computeDT API needs an iso_value that defines the foreground-background interface, to create 
    a smoother interface we first antialiasing the segmentation then compute the distance transform 
    at the zero-level set. We then need to smooth the DT as it will have some remaining aliasing effect 
    of binarization. 
    So the steps are:
        - Antialias file_list_dts
        - Compute distance transform
        - Apply smoothing
        - Save the distance transform
    """

    # Define distance transform parameters
    iso_value = 0
    sigma = 1.3
    antialias_iterations = 30
    dt_list = []
    shape_names = []
    # Loop over segs and compute smooth DT
    for shape_filename in file_list_new_segs:
        print('Loading: ' + shape_filename)
        # get current shape name
        shape_name = shape_filename.split('/')[-1].replace('.nrrd', '')
        shape_names.append(shape_name)
        # load segmentation
        shape_seg = sw.Image(shape_filename)
        print('Compute DT for segmentation: ' + shape_name)
        shape_seg.antialias(antialias_iterations).computeDT(iso_value).gaussianBlur(sigma)
        dt_list.append(shape_seg)
    # Save distance transforms
    new_dt_files = sw.utils.save_images(groom_dir + 'distance_transforms/', dt_list,
                                        shape_names, extension='nrrd', compressed=True, verbose=True)
    # Get list of original and new distance transforms
    dt_files = file_list_dts + new_dt_files

    print("\nStep 3. Optimize - Particle Based Optimization\n")
    """
    Step 3: OPTIMIZE - Particle Based Optimization

    Now that we have the distance transform representation of data we create 
    the parameter files for the shapeworks particle optimization routine.
    For more details on the plethora of parameters for shapeworks please refer 
    to docs/workflow/optimze.md
    http://sciinstitute.github.io/ShapeWorks/workflow/optimize.html
    """

    # Make directory to save optimization output
    point_dir = output_directory + 'shape_models/'
    if not os.path.exists(point_dir):
        os.makedirs(point_dir)

    """
    Evaluate the mean shape of the existing shape model and use that to initialize the 
    particles on the new shapes
    """
    shape_model_dir = output_directory + dataset_name + "/shape_models/ellipsoid_aligned_multiscale_particles/"
    sw.utils.findMeanShape(shape_model_dir)
    mean_shape_path = shape_model_dir + '/meanshape_local.particles'
    fixed_local_particles = sorted(glob.glob(shape_model_dir + "/*_local.particles"))
    all_local_particles = fixed_local_particles + [mean_shape_path, mean_shape_path, mean_shape_path, mean_shape_path];

    # Create spreadsheet
    project_location = output_directory 
    subjects = []

    # Add current shape model (e.g. fixed subjects)
    for i in range(len(file_list_segs)):
        subject = sw.Subject()
        original_groom_files = sw.utils.get_relative_paths([os.getcwd() + "/" + file_list_segs[i]], project_location)
        rel_groom_files = sw.utils.get_relative_paths([os.getcwd() + "/" + file_list_dts[i]], project_location)
        rel_particle_files = sw.utils.get_relative_paths([os.getcwd() + "/" + fixed_local_particles[i]],  project_location)
        subject.set_original_filenames(original_groom_files)
        subject.set_groomed_filenames(rel_groom_files)
        subject.set_local_particle_filenames(rel_particle_files)
        subject.set_world_particle_filenames(rel_particle_files)
        subject.set_extra_values({"fixed": "yes"})
        subjects.append(subject)

    # Add new shapes
    for i in range(len(file_list_new_segs)):
        subject = sw.Subject()
        original_groom_files = sw.utils.get_relative_paths([os.getcwd() + "/" + file_list_new_segs[i]], project_location)
        rel_groom_files = sw.utils.get_relative_paths([os.getcwd() + "/" + new_dt_files[i]], project_location)
        rel_particle_files = sw.utils.get_relative_paths([os.getcwd() + "/" + mean_shape_path], project_location)
        subject.set_original_filenames(original_groom_files)
        subject.set_groomed_filenames(rel_groom_files)
        subject.set_local_particle_filenames(rel_particle_files)
        subject.set_world_particle_filenames(rel_particle_files)
        subject.set_extra_values({"fixed": "no"})
        subjects.append(subject)

    project = sw.Project()
    project.set_subjects(subjects)
    parameters = sw.Parameters()

    # Create a dictionary for all the parameters required by optimization
    parameter_dictionary = {
        "number_of_particles": 128,
        "use_normals": 0,
        "normals_strength": 15.0,
        "checkpointing_interval": 0,
        "keep_checkpoints": 0,
        "iterations_per_split": 10,
        "optimization_iterations": 10,
        "starting_regularization": 100,
        "ending_regularization": 0.1,
        "relative_weighting": 15,
        "initial_relative_weighting": 0.05,
        "procrustes_interval": 0,
        "procrustes_scaling": 0,
        "save_init_splits": 0,
        "verbosity": 0,
        "narrow_band": 1e10,
    }

    for key in parameter_dictionary:
        parameters.set(key, sw.Variant(parameter_dictionary[key]))

    project.set_parameters("optimize", parameters)

    # Set studio parameters
    studio_dictionary = {
        "show_landmarks": 0,
        "tool_state": "analysis"
    }
    studio_parameters = sw.Parameters()
    for key in studio_dictionary:
        studio_parameters.set(key, sw.Variant(studio_dictionary[key]))
    project.set_parameters("studio", studio_parameters)
    spreadsheet_file = output_directory + "ellipsoid_fd_" + args.option_set + ".swproj"
    project.save(spreadsheet_file)

    # Run optimization
    optimize_cmd = ('shapeworks optimize --progress --name ' + spreadsheet_file).split()
    print(optimize_cmd)
    subprocess.check_call(optimize_cmd)

    # If tiny test or verify, check results and exit
    sw.utils.check_results(args, spreadsheet_file)

    print("\nStep 4. Analysis - Launch ShapeWorksStudio")
    """
    Step 4: ANALYZE - open in studio
    For more information about the analysis step, see:
    # http://sciinstitute.github.io/ShapeWorks/workflow/analyze.html
    """
    analyze_cmd = ('ShapeWorksStudio ' + spreadsheet_file).split()
    subprocess.check_call(analyze_cmd)
