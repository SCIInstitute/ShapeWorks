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
import AnalyzeUtils
import numpy as np
import subprocess


def Run_Pipeline(args):
    print("\nStep 1. Acquire Data\n")
    """
    Step 1: ACQUIRE DATA

    We define dataset_name which determines which dataset to download from 
    the portal and the directory to save output from the use case in. 
    """
    output_directory = "Output/ellipsoid_multiple_domain_mesh/"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    # If running a tiny_test, then download subset of the data
    if args.tiny_test:
        dataset_name = "ellipsoid_multiple_domain_mesh_tiny_test"
        sw.download_dataset(dataset_name, output_directory)
        dataset_name = "ellipsoid_joint_rotation"
        mesh_files = sorted(glob.glob(output_directory +
                                      dataset_name + "/meshes/*.vtk"))[:6]
    # Else download the entire dataset
    else:
        dataset_name = "ellipsoid_multiple_domain_mesh"
        sw.download_dataset(dataset_name, output_directory)
        mesh_files = sorted(glob.glob(output_directory + "/meshes/*.vtk"))

        if args.use_subsample:
            inputMeshes = [sw.Mesh(filename) for filename in mesh_files]
            sample_idx = sw.data.sample_meshes(inputMeshes, int(args.num_subsample), domains_per_shape=2)
            mesh_files = [mesh_files[i] for i in sample_idx]

    print("\nStep 2. Groom - Data Pre-processing\n")
    """
    Step 2: GROOMING 
    The required grooming steps are: 
    1. Remesh 
    2. Reference selection
    3. Rigid Alignment
    For more information on grooming see docs/workflow/groom.md
    http://sciinstitute.github.io/ShapeWorks/workflow/groom.html
    """

    # Create a directory for groomed output
    groom_dir = output_directory + 'groomed/'
    if not os.path.exists(groom_dir):
        os.makedirs(groom_dir)

    """
    First, we need to loop over the mesh files and load them
    """
    # list of shape segmentations
    mesh_list = []
    # list of shape names (shape files prefixes) to be used for saving outputs
    mesh_names = []
    domain_ids = []
    for mesh_file in mesh_files:
        print('Loading: ' + mesh_file)
        # get current shape name
        mesh_name = mesh_file.split('/')[-1].replace('.vtk', '')
        mesh_names.append(mesh_name)
        # get domain identifiers
        domain_ids.append(mesh_name.split(".")[0].split("_")[-1])

        # load mesh
        mesh = sw.Mesh(mesh_file)
        # do initial grooming steps
        print("Grooming: " + mesh_name)
        mesh.remeshPercent(percentage=0.9, adaptivity=1.0)
        # append to the mesh list
        mesh_list.append(mesh)

    # domain identifiers for all shapes
    domain_ids = np.array(domain_ids)
    # shape index for all shapes in domain 1
    domain1_indx = list(np.where(domain_ids == 'd1')[0])
    # shape index for all shapes in domain 2
    domain2_indx = list(np.where(domain_ids == 'd2')[0])
    """
    Grooming Step 2: Select a reference
    This step requires loading all of the meshes at once so the shape
    closest to the mean can be found and selected as the reference. 
    """
    domains_per_shape = 2
    domain_1_meshes = []
    # get domain 1 shapes
    for i in range(int(len(mesh_list)/domains_per_shape)):
        domain_1_meshes.append(mesh_list[i*domains_per_shape])

    ref_index = sw.find_reference_mesh_index(domain_1_meshes)
    domain1_reference = mesh_list[ref_index*domains_per_shape].copy()
    domain2_reference = mesh_list[ref_index*domains_per_shape+1].copy()
    domain1_ref_name = mesh_names[ref_index*domains_per_shape]
    domain2_ref_name = mesh_names[ref_index*domains_per_shape+1]
    reference = [domain1_reference, domain2_reference]
    ref_name = [domain1_ref_name, domain2_ref_name]

    # Create a combined mesh for the global alignment
    combined_reference = domain1_reference.copy()
    combined_reference += domain2_reference

    """
    Grooming Step 3: Rigid alignment
    Now we can loop over all of the meshes again to find the rigid
    alignment transform 
    """

    transforms = []
    for i in range(len(domain_1_meshes)):

        # calculate the transformation
        for d in range(domains_per_shape):
            # compute rigid transformation
            rigidTransform = mesh_list[i*domains_per_shape +
                                       d].createTransform(reference[d], sw.Mesh.AlignmentType.Rigid, 100)
            name = mesh_names[i*domains_per_shape+d]
            print('Aligning ' + name + ' to ' + ref_name[d])
            transforms.append(rigidTransform)

        combined_mesh = mesh_list[i*domains_per_shape].copy()
        for d in range(domains_per_shape):
            # skip the first domain
            if d == 0:
                continue
            combined_mesh += mesh_list[i*domains_per_shape+d]
        transform = combined_mesh.createTransform(combined_reference, sw.Mesh.AlignmentType.Rigid, 100)
        transforms.append(transform)

    # Save groomed meshes
    groomed_mesh_files = sw.utils.save_meshes(groom_dir + 'meshes/', mesh_list, mesh_names, extension='vtk')

    print("\nStep 3. Optimize - Particle Based Optimization\n")
    """
    Step 3: OPTIMIZE - Particle Based Optimization

    Now we can run optimization directly on the meshes.
    For more details on the plethora of parameters for shapeworks please refer 
    to docs/workflow/optimize.md
    http://sciinstitute.github.io/ShapeWorks/workflow/optimize.html
    """

    # Create project spreadsheet
    project_location = output_directory
    if not os.path.exists(project_location):
        os.makedirs(project_location)
    # Set subjects
    subjects = []

    for i in range(len(domain_1_meshes)):
        subject = sw.Subject()
        subject.set_number_of_domains(domains_per_shape)
        rel_mesh_files = []
        rel_groom_files = []
        transform = []
        for d in range(domains_per_shape):
            rel_mesh_files += sw.utils.get_relative_paths([os.getcwd() +
                                                          '/' + mesh_files[i*domains_per_shape+d]], project_location)
            rel_groom_files += sw.utils.get_relative_paths([os.getcwd() + '/' +
                                                           groomed_mesh_files[i*domains_per_shape+d]], project_location)
            transform.append(transforms[i*(domains_per_shape+1)+d].flatten())
        # add the global alignment transform
        transform.append(transforms[i*(domains_per_shape+1)+domains_per_shape].flatten())
        subject.set_groomed_transforms(transform)
        subject.set_groomed_filenames(rel_groom_files)
        subject.set_original_filenames(rel_mesh_files)
        subjects.append(subject)
    # Set project
    project = sw.Project()
    project.set_subjects(subjects)
    parameters = sw.Parameters()

    parameter_dictionary = {
        "checkpointing_interval": 200,
        "keep_checkpoints": 0,
        "iterations_per_split": 200,
        "optimization_iterations": 200,
        "starting_regularization": 1000,
        "ending_regularization": 0.1,
        "relative_weighting": 10,
        "initial_relative_weighting": 0.1,
        "procrustes_interval": 0,
        "procrustes_scaling": 0,
        "save_init_splits": 0,
        "verbosity": 0

    }
    num_particles = [128, 128]

    # If running a tiny test, reduce some parameters
    if args.tiny_test:
        num_particles = [32, 32]
        parameter_dictionary["optimization_iterations"] = 30

    # setting the argument to singlescale for the output filename
    args.use_single_scale = True
    args.option_set = args.option_set.replace("multiscale", "singlescale")
    # Add param dictionary to spreadsheet
    for key in parameter_dictionary:
        parameters.set(key, sw.Variant([parameter_dictionary[key]]))
    parameters.set("number_of_particles", sw.Variant(num_particles))
    project.set_parameters("optimize", parameters)

    spreadsheet_file = output_directory + "ellipsoid_multiple_domain_mesh_" + args.option_set + ".swproj"
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
