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
import numpy as np
import shapeworks as sw
import AnalyzeUtils
import subprocess

def Run_Pipeline(args):
    print("\nStep 1. Acquire Data\n")
    """
    Step 1: ACQUIRE DATA

    We define dataset_name which determines which dataset to download from 
    the portal and the directory to save output from the use case in. 
    """
    
    output_directory = "Output/ellipsoid_multiple_domain/"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    # If running a tiny_test, then download subset of the data
    if args.tiny_test:
        dataset_name = "ellipsoid_multiple_domain_tiny_test"
        sw.download_dataset(dataset_name, output_directory)
        dataset_name = "ellipsoid_joint_rotation"
        file_list = sorted(glob.glob(output_directory +
                                     dataset_name + "/segmentations/*.nrrd"))[:6]
    # Else download the entire dataset
    else:
        dataset_name = "ellipsoid_multiple_domain"
        sw.download_dataset(dataset_name, output_directory)
        file_list = sorted(glob.glob(output_directory + "/segmentations/*.nrrd"))

        if args.use_subsample:
            inputImages =[sw.Image(filename) for filename in file_list]
            sample_idx = sw.data.sample_images(inputImages, int(args.num_subsample),domains_per_shape=2)
            file_list = [file_list[i] for i in sample_idx]


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

    Grooming Step 1: First, we need to loop over the shape segmentation files and 
    load the segmentations and apply the initial grooming steps
    """
    # list of shape segmentations
    shape_seg_list = []
    # list of shape names (shape files prefixes) to be used for saving outputs
    shape_names = []
    domain_ids = []
    for shape_filename in file_list:
        print('Loading: ' + shape_filename)
        # get current shape name
        shape_name = shape_filename.split('/')[-1].replace('.nrrd', '')
        shape_names.append(shape_name)

        # get domain identifiers
        name = shape_filename.split('/')[-1].replace('.nrrd', '')
        domain_ids.append(name.split(".")[0].split("_")[-1])
        
        # load segmentation
        shape_seg = sw.Image(shape_filename)
        # do initial grooming steps
        print("Grooming: " + shape_name)
         # Individually crop each segmentation using a computed bounding box
        iso_value = 0.5  # voxel value for isosurface
        bounding_box = sw.ImageUtils.boundingBox([shape_seg], iso_value).pad(5)
        shape_seg.crop(bounding_box)
        # Resample to isotropic spacing using linear interpolation
        antialias_iterations = 30   # number of iterations for antialiasing
        iso_spacing = [1, 1, 1]     # isotropic spacing
        shape_seg.antialias(antialias_iterations).resample(iso_spacing, sw.InterpolationType.Linear).binarize()

        # append to the shape list
        shape_seg_list.append(shape_seg)
    #domain identifiers for all shapes
    domain_ids = np.array(domain_ids)
    #shape index for all shapes in domain 1 
    domain1_indx = list(np.where(domain_ids == 'd1')[0])
    #shape index for all shapes in domain 2
    domain2_indx = list(np.where(domain_ids == 'd2')[0])



    """
    Grooming Step 2: Select a reference
    This step requires breaking the loop to load all of the segmentations at once so the shape
    closest to the mean can be found and selected as the reference.
    For the ellipsoid_joint available on the ShapeWorks portal, the mode of variation are 
    rotation and/or size of the second ellipsoid w.r.t to the first ellipsoid. 
    Hence, we align the shapes using the first domain as the reference. 

    If both the ellipsoids vary w.r.t each other, then we would use 'global alignment'. 
    For this, the reference shape can be estimated by combined the shapes from all domains. 


    ref_index,combined_mesh = sw.find_reference_image_index(shape_seg_list,domains_per_shape=2)
    for i in range(len(combined_mesh)):

        bbox = combined_mesh[i].boundingBox().pad(20.0)
        combined_segs.append(combined_mesh[i].toImage(bbox))

    After finding the combined reference image, the transformation will be calculated for the combined shapes
    but the transformation will be applied to each domain in each shape individually. 


    """

    domains_per_shape = 2
    domain_1_shapes = []
    # get domain 1 shapes 
    for i in range(int(len(shape_seg_list)/domains_per_shape)):
        domain_1_shapes.append(shape_seg_list[i*domains_per_shape])

    ref_index = sw.find_reference_image_index(domain_1_shapes)
    reference = domain_1_shapes[ref_index].copy()
    ref_name = shape_names[ref_index*domains_per_shape]
    domain1_reference = domain_1_shapes[ref_index].copy()
    domain2_reference = shape_seg_list[ref_index*domains_per_shape+1].copy()
    domain1_ref_name = shape_names[ref_index*domains_per_shape]
    domain2_ref_name = shape_names[ref_index*domains_per_shape+1]
    reference = [domain1_reference,domain2_reference]
    ref_name = [domain1_ref_name,domain2_ref_name]

    # Create a combined mesh for the global alignment
    combined_reference = domain1_reference.toMesh(0.5)
    combined_reference += domain2_reference.toMesh(0.5)

    """
    Grooming Step 3: Rigid alignment
    Now we can loop over all of the segmentations again to find the rigid
    alignment transform and compute a distance transform
    """
    iso_value = 0.5
    icp_iterations = 200
    transforms = []
    for i in range(len(domain_1_shapes)):

        
        # get the transformation to each domain(each subject)
        for d in range(domains_per_shape):
            # compute rigid transformation using the domain 1 segmentations
            name = shape_names[i*domains_per_shape+d]
            print('Aligning ' + name + ' to ' + ref_name[d]) 
            rigidTransform = shape_seg_list[i*domains_per_shape+d].createRigidRegistrationTransform(
            reference[d], iso_value, icp_iterations)
            rigid_transform = sw.utils.getVTKtransform(rigidTransform)
            transforms.append(rigid_transform)
            
        combined_mesh = shape_seg_list[i*domains_per_shape+d].toMesh(0.5)
        for d in range(domains_per_shape):
            # skip the first domain
            if d == 0:
                continue
            combined_mesh += shape_seg_list[i*domains_per_shape+d].toMesh(0.5)
        transform = combined_mesh.createTransform(combined_reference, sw.Mesh.AlignmentType.Rigid, 100)
        transforms.append(transform)
   
    """
    Grooming Step 4: Converting segmentations to smooth signed distance transforms.
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

    # Define distance transform parameters
    iso_value = 0
    sigma = 2
    # Loop over segs and compute smooth DT
    for shape_seg, shape_name in zip(shape_seg_list, shape_names):
        print('Compute DT for segmentation: ' + shape_name)
        shape_seg.antialias(antialias_iterations).computeDT(
            iso_value).gaussianBlur(sigma)
    # Save distance transforms
    dt_files = sw.utils.save_images(groom_dir + 'distance_transforms/', shape_seg_list,
                                    shape_names, extension='nrrd', compressed=True, verbose=True)

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
    
    for i in range(len(domain_1_shapes)):
        subject = sw.Subject()
        subject.set_number_of_domains(domains_per_shape)
        rel_seg_files = []
        rel_groom_files = []
        transform = []
        for d in range(domains_per_shape):
            rel_seg_files += sw.utils.get_relative_paths([os.getcwd() + '/' + file_list[i*domains_per_shape+d]], project_location)
            rel_groom_files += sw.utils.get_relative_paths([os.getcwd() + '/' + dt_files[i*domains_per_shape+d]], project_location)
            transform.append(transforms[i*(domains_per_shape+1)+d].flatten())
        # add the global alignment transform
        transform.append(transforms[i*(domains_per_shape+1)].flatten())
        subject.set_groomed_transforms(transform)
        subject.set_groomed_filenames(rel_groom_files)
        subject.set_original_filenames(rel_seg_files)
        subjects.append(subject)
    # Set project
    project = sw.Project()
    project.set_subjects(subjects)
    parameters = sw.Parameters()

    # Create a dictionary for all the parameters required by optimization
    parameter_dictionary = {
        "checkpointing_interval" : 200,
        "keep_checkpoints" : 0,
        "iterations_per_split" : 200,
        "optimization_iterations" : 200,
        "starting_regularization" :1000,
        "ending_regularization" : 0.1,
        "relative_weighting" : 10, 
        "initial_relative_weighting" : 0.1,
        "procrustes_interval" : 0,
        "procrustes_scaling" : 0,
        "save_init_splits" : 0,
        "verbosity" : 0
      }
    num_particles = [128,128]

    # If running a tiny test, reduce some parameters
    if args.tiny_test:
        num_particles = [32,32]
        parameter_dictionary["optimization_iterations"] = 30

    #setting the argument to singlescale for the output filename
    args.use_single_scale = True
    args.option_set = args.option_set.replace("multiscale","singlescale")
        
    # Add param dictionary to spreadsheet
    for key in parameter_dictionary:
        parameters.set(key, sw.Variant([parameter_dictionary[key]]))
    parameters.set("number_of_particles" ,sw.Variant(num_particles))
    project.set_parameters("optimize", parameters)
    spreadsheet_file = output_directory + "ellipsoid_multiple_domain_" + args.option_set + ".swproj"
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
