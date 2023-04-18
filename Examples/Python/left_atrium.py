# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Statistical Shape Modeling with ShapeWorks
====================================================================

In this example, we provide unpre-processed left atrial dataset,
which consists of the LGE scan and its segmentation across some number of subjects.
Since the data set is not pre-processed, it requires all re-processing steps;
resampling, padding, alignment, etc. To run the ShapeWorks particle-based optimization,
the processed segmentation files are converted to the signed distance transform.
This example is set to serve as a test case for users who want to process the raw(gray-sclae)
images as well as their binary segmentation images.

First import the necessary modules
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
    dataset_name = "left_atrium"
    output_directory = "Output/left_atrium/"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    # If running a tiny_test, then download subset of the data
    if args.tiny_test:
        dataset_name = "left_atrium_tiny_test"
        args.use_single_scale = 1
        sw.download_dataset(dataset_name, output_directory)
        dataset_name = "left_atrium"
        file_list = sorted(
            glob.glob(output_directory + dataset_name + "/segmentations/*.nrrd"))[:3]

    # Else download the entire dataset
    else:
        sw.download_dataset(dataset_name, output_directory)
        file_list = sorted(
            glob.glob(output_directory + dataset_name + "/segmentations/*.nrrd"))

        # Select representative data if using subsample
        if args.use_subsample:
            inputSegs =[sw.Image(filename) for filename in file_list]
            sample_idx = sw.data.sample_images(inputSegs, int(args.num_subsample))
            file_list = [file_list[i] for i in sample_idx]
        else:
            sample_idx = []

    print("\nStep 2. Groom - Data Pre-processing\n")
    """
    Step 2: GROOM
    The following grooming steps are performed:
        - Crop (cropping first makes resampling faster)
        - Resample to have isotropic spacing
        - Pad with zeros
        - Compute alignment transforms to use in optimization
        - Compute distance transforms
    For more information about grooming, please refer to:
    http://sciinstitute.github.io/ShapeWorks/workflow/groom.html
    """
   
    # Create a directory for groomed output
    groom_dir = output_directory + 'groomed/'
    if not os.path.exists(groom_dir):
        os.makedirs(groom_dir)

    """
    We loop over the shape segmentation files and load the segmentations
    and apply the intial grooming steps
    """
    # list of shape segmentations
    shape_seg_list = []
    # list of shape names (shape files prefixes) to be used for saving outputs
    shape_names = []
    for shape_filename in file_list:
        print('Loading: ' + shape_filename)
        # get current shape name
        shape_name = shape_filename.split('/')[-1].replace('.nrrd', '')
        shape_names.append(shape_name)
        # load segmentation
        shape_seg = sw.Image(shape_filename)
        shape_seg_list.append(shape_seg)

        # do initial grooming steps
        print("Grooming: " + shape_name)
         # Individually crop each segmentation using a computed bounding box
        iso_value = 0.5  # voxel value for isosurface
        bounding_box = sw.ImageUtils.boundingBox([shape_seg], iso_value).pad(10)
        shape_seg.crop(bounding_box)
        # Resample to isotropic spacing using linear interpolation
        antialias_iterations = 30   # number of iterations for antialiasing
        iso_spacing = [1, 1, 1]     # isotropic spacing
        shape_seg.antialias(antialias_iterations).resample(iso_spacing, sw.InterpolationType.Linear).binarize()
        # Pad segmentations with zeros
        pad_size = 10    # number of voxels to pad for each dimension
        pad_value = 0   # the constant value used to pad the segmentations
        shape_seg.pad(pad_size, pad_value)
        
    """
    To find the alignment transforms and save them for optimization,
    we must break the loop to select a reference segmentation
    """
    ref_index = sw.find_reference_image_index(shape_seg_list)
    ref_seg = shape_seg_list[ref_index].write(groom_dir + 'reference.nrrd')
    ref_name = shape_names[ref_index]
    print("Reference found: " + ref_name)

    """
    Now we can loop over all of the segmentations again to find the rigid
    alignment transform and compute a distance transform
    """
    rigid_transforms = [] # Save rigid transorm matrices
    for shape_seg, shape_name in zip(shape_seg_list, shape_names):
        print('Finding alignment transform from ' + shape_name + ' to ' + ref_name)
        # Get rigid transform
        iso_value = 0.5      # voxel value for isosurface
        icp_iterations = 100 # number of ICP iterations
        rigid_transform = shape_seg.createRigidRegistrationTransform(
            ref_seg, iso_value, icp_iterations)
        # Convert to vtk format for optimization
        rigid_transform = sw.utils.getVTKtransform(rigid_transform)
        rigid_transforms.append(rigid_transform)

        # Convert segmentations to smooth signed distance transforms
        print("Converting " + shape_name + " to distance transform")
        iso_value = 0   # voxel value for isosurface
        sigma = 1.5     # for Gaussian blur
        shape_seg.antialias(antialias_iterations).computeDT(iso_value).gaussianBlur(sigma)

    # Save distance transforms
    groomed_files = sw.utils.save_images(groom_dir + 'distance_transforms/', shape_seg_list,
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

    point_dir = output_directory + args.option_set
    if not os.path.exists(point_dir):
        os.makedirs(point_dir)

    # Create spreadsheet
    project_location = output_directory
    subjects = []
    number_domains = 1
    for i in range(len(shape_seg_list)):
        subject = sw.Subject()
        subject.set_number_of_domains(number_domains)
        rel_mesh_files = sw.utils.get_relative_paths([os.getcwd() + '/' + file_list[i]], project_location)
        subject.set_original_filenames(rel_mesh_files)
        rel_groom_files = sw.utils.get_relative_paths([os.getcwd() + '/' + groomed_files[i]], project_location)
        subject.set_groomed_filenames(rel_groom_files)
        transform = [ rigid_transforms[i].flatten() ]
        subject.set_groomed_transforms(transform)
        subjects.append(subject)

    project = sw.Project()
    project.set_subjects(subjects)
    parameters = sw.Parameters()

    # Create a dictionary for all the parameters required by optimization
    parameter_dictionary = {
        "number_of_particles": 512,
        "use_normals": 0,
        "normals_strength": 10.0,
        "checkpointing_interval": 200,
        "keep_checkpoints": 0,
        "iterations_per_split": 4000,
        "optimization_iterations": 4000,
        "starting_regularization": 1000,
        "ending_regularization": 10,
        "relative_weighting": 10,
        "procrustes": 1,
        "initial_relative_weighting": 0.1,
        "procrustes_interval": 1,
        "procrustes_scaling": 1,
        "save_init_splits": 0,
        "verbosity": 0
    }
    # If running a tiny test, reduce some parameters
    if args.tiny_test:
        parameter_dictionary["number_of_particles"] = 32
        parameter_dictionary["optimization_iterations"] = 25
        parameter_dictionary["iterations_per_split"] = 25
    # Run multiscale optimization unless single scale is specified
    if not args.use_single_scale:
        parameter_dictionary["multiscale"] = 1
        parameter_dictionary["multiscale_particles"] = 128
    
    print('Generating project file')
    # Add param dictionary to spreadsheet
    for key in parameter_dictionary:
        parameters.set(key, sw.Variant([parameter_dictionary[key]]))
    project.set_parameters("optimize", parameters)
    spreadsheet_file = output_directory + "left_atrium_" + args.option_set + ".swproj"
    project.save(spreadsheet_file)

    # Run optimization
    print('Running optimization')
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
