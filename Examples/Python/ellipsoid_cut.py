# -*- coding: utf-8 -*-
"""
====================================================================
ShapeWorks Optimization with Multiple Cutting Planes
====================================================================
This example demonstrates how to optimize with cutting planes on 
simple aligned ellipsoid data.
"""
import os
import glob
import subprocess
import numpy as np
import shapeworks as sw

def Run_Pipeline(args):
    print("\nStep 1. Acquire Data\n")
    """
    Step 1: ACQUIRE DATA

    We define dataset_name which determines which dataset to download from 
    the portal and the directory to save output from the use case in. 
    """
    output_directory = "Output/ellipsoid_cut/"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    # If running a tiny_test, then download subset of the data
    if args.tiny_test:
        dataset_name = "ellipsoid_cut_tiny_test"
        args.use_single_scale = 1
        sw.download_dataset(dataset_name, output_directory)
        dataset_name = "ellipsoid_1mode_aligned"
        file_list = sorted(glob.glob(output_directory +
                                     dataset_name + "/segmentations/*.nrrd"))[:3]
        plane_files = sorted(glob.glob(output_directory +
                            dataset_name + "/constraints/*.json"))[:3]
    # Else download the entire dataset
    else:
        dataset_name = "ellipsoid_cut"
        sw.download_dataset(dataset_name, output_directory)
        dataset_name = "ellipsoid_1mode_aligned"
        file_list = sorted(glob.glob(output_directory +
                                     dataset_name + "/segmentations/*.nrrd"))
        plane_files = sorted(glob.glob(output_directory +
                            dataset_name + "/constraints/*.json"))
        # Select representative data if using subsample
        if args.use_subsample:
            inputImages =[sw.Image(filename) for filename in file_list]
            sample_idx = sw.data.sample_images(inputImages, int(args.num_subsample))
            file_list = [file_list[i] for i in sample_idx]
            plane_files = [plane_files[i] for i in sample_idx]

    print("\nStep 2. Groom - Create distance transforms\n")
    """
    Step 2: GROOMING 
    
    This use case makes uses prealigned data so the only grooming step 
    required is to convert the segmentations to distance transforms.

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
        - Antialias 
        - Compute distance transform
        - Apply smoothing
        - Save the distance transform
    """
    iso_value = 0
    sigma = 1.3
    antialias_iterations = 30
    dt_list = []
    shape_names = []
    # Loop over segs and compute smooth DT
    for shape_filename in file_list:
        print('Loading: ' + shape_filename)
        # get current shape name
        shape_name = shape_filename.split('/')[-1].replace('.nrrd', '')
        shape_names.append(shape_name)
        # load segmentation
        shape_seg = sw.Image(shape_filename)
        print('Compute DT for segmentation: ' + shape_name)
        shape_seg.antialias(antialias_iterations).computeDT(
            iso_value).gaussianBlur(sigma)
        dt_list.append(shape_seg)
    # Save distance transforms
    groomed_files = sw.utils.save_images(groom_dir + 'distance_transforms/', dt_list,
                                    shape_names, extension='nrrd', compressed=True, verbose=True)

    # Get data input (meshes if running in mesh mode, else distance transforms)
    domain_type, groomed_files = sw.data.get_optimize_input(groomed_files, args.mesh_mode)

    print("\nStep 3. Optimize - Particle Based Optimization\n")
    """
    Step 3: OPTIMIZE - Particle Based Optimization

    Now that we have the distance transform representation of data we create 
    the parameter files for the shapeworks particle optimization routine.
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
    for i in range(len(file_list)):
        subject = sw.Subject()
        subject.set_number_of_domains(number_domains)
        rel_seg_files = sw.utils.get_relative_paths([os.getcwd() + '/' + file_list[i]], project_location)
        subject.set_original_filenames(rel_seg_files)
        rel_groom_files = sw.utils.get_relative_paths([os.getcwd() + '/' + groomed_files[i]], project_location)
        subject.set_groomed_filenames(rel_groom_files)
        transform = [ np.eye(4).flatten() ]
        subject.set_groomed_transforms(transform)
        rel_plane_files = sw.utils.get_relative_paths([os.getcwd() + '/' + plane_files[i]], project_location)
        subject.set_constraints_filenames(rel_plane_files)
        subjects.append(subject)
    # Set project
    project = sw.Project()
    project.set_subjects(subjects)
    parameters = sw.Parameters()

    # Create a dictionary for all the parameters required by optimization
    parameter_dictionary = {
        "number_of_particles": 32,
        "use_normals": 1,
        "normals_strength": 15,
        "checkpointing_interval": 200,
        "keep_checkpoints": 0,
        "iterations_per_split": 3000,
        "optimization_iterations": 3000,
        "starting_regularization": 100,
        "ending_regularization": 10,
        "relative_weighting": 15,
        "initial_relative_weighting": 0.05,
        "verbosity": 0,
    }
    # If running a tiny test, reduce some parameters
    if args.tiny_test:
        parameter_dictionary["number_of_particles"] = 16
        parameter_dictionary["optimization_iterations"] = 25
    # Run multiscale optimization unless single scale is specified
    if not args.use_single_scale:
        parameter_dictionary["multiscale"] = 1
        parameter_dictionary["multiscale_particles"] = 16

    # Add param dictionary to spreadsheet
    for key in parameter_dictionary:
        parameters.set(key,sw.Variant([parameter_dictionary[key]]))
    project.set_parameters("optimize",parameters)
    spreadsheet_file = project_location + "/ellipsoid_cut_" + args.option_set+ ".swproj"
    project.save(spreadsheet_file)

    # Run optimization
    optimize_cmd = ('shapeworks optimize --progress --name ' + spreadsheet_file).split()
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
