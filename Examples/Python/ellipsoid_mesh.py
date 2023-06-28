# -*- coding: utf-8 -*-
"""
====================================================================
Example ShapeWorks Pipeline for mesh grooming and optimization
====================================================================
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
    dataset_name = "ellipsoid_mesh"
    output_directory = "Output/ellipsoid_mesh/"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    # If running a tiny_test, then download subset of the data
    if args.tiny_test:
        dataset_name = "ellipsoid_mesh_tiny_test"
        args.use_single_scale = 1
        sw.download_dataset(dataset_name, output_directory)
        dataset_name = "ellipsoid_1mode"
        mesh_files = sorted(glob.glob(output_directory +
                            dataset_name + "/meshes/*.vtk"))[:3]
    # else download the entire dataset
    else:
        dataset_name = "ellipsoid_mesh"
        sw.download_dataset(dataset_name, output_directory)
        dataset_name = "ellipsoid_1mode"
        mesh_files = sorted(glob.glob(output_directory +
                            dataset_name + "/meshes/*.vtk"))
        # Select data if using subsample
        if args.use_subsample:
            inputMeshes =[sw.Mesh(filename) for filename in mesh_files]
            sample_idx = sw.data.sample_meshes(inputMeshes, int(args.num_subsample))
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
    groom_dir = output_directory + 'groomed_tinytest/'
    if not os.path.exists(groom_dir):
        os.makedirs(groom_dir)

    """
    First, we need to loop over the mesh files and load them
    """
    mesh_list = []
    mesh_names = []
    for mesh_file in mesh_files:
        print('Loading: ' + mesh_file)
        # get current name
        mesh_names.append(mesh_file.split('/')[-1].replace('.vtk', ''))
        """
        Grooming Step 1: load mesh and remesh
        """
        mesh = sw.Mesh(mesh_file).remeshPercent(percentage=0.99, adaptivity=1.0)
        # append to the list
        mesh_list.append(mesh)

    """
    Grooming Step 2: Select a reference
    This step requires loading all of the meshes at once so the shape
    closest to the mean can be found and selected as the reference. 
    """
    ref_index = sw.find_reference_mesh_index(mesh_list)
    # Make a copy of the reference segmentation 
    ref_mesh = mesh_list[ref_index]
    # Center the reference mesh at 0,0,0
    # ref_mesh.translate(-ref_mesh.center()).write(groom_dir + 'reference.vtk')
    ref_mesh.write(groom_dir + 'reference.vtk')
    ref_name = mesh_names[ref_index]
    print("Reference found: " + ref_name)

    """
    Grooming Step 3: Rigid alignment
    This step rigidly aligns each shape to the selected reference. 
    """
    Rigid_transforms = []
    for mesh, name in zip(mesh_list, mesh_names):
        print('Aligning ' + name + ' to ' + ref_name)
        # compute rigid transformation
        rigid_transform = mesh.createTransform(ref_mesh, sw.Mesh.AlignmentType.Rigid, 100)
        Rigid_transforms.append(rigid_transform)

    # Save groomed meshes
    groomed_mesh_files = sw.utils.save_meshes(groom_dir + 'meshes/', mesh_list, mesh_names, extension='vtk')


    print("\nStep 3. Optimize - Particle Based Optimization\n")
    """
    Step 3: OPTIMIZE - Particle Based Optimization

    Now that we have the groomed representation of data we create 
    the spreadsheet for the shapeworks particle optimization routine.
    For more details on the plethora of parameters for shapeworks please refer 
    to: http://sciinstitute.github.io/ShapeWorks/workflow/optimize.html
    """

    # Create project spreadsheet
    project_location = output_directory
    if not os.path.exists(project_location):
        os.makedirs(project_location)
    # Set subjects
    subjects = []
    number_domains = 1
    for i in range(len(mesh_names)):
        subject = sw.Subject()
        subject.set_number_of_domains(number_domains)
        rel_mesh_files = sw.utils.get_relative_paths([os.getcwd() + "/" + mesh_files[i]], project_location)
        subject.set_original_filenames(rel_mesh_files)
        rel_groom_file = sw.utils.get_relative_paths([os.getcwd() + "/" + groomed_mesh_files[i]], project_location)
        subject.set_groomed_filenames(rel_groom_file)
        transform = Rigid_transforms[i]
        transforms = [ transform.flatten() ]
        subject.set_groomed_transforms(transforms)
        subjects.append(subject)
    # Set project
    project = sw.Project()
    project.set_subjects(subjects)
    parameters = sw.Parameters()

    # Create a dictionary for all the parameters required by optimization
    parameter_dictionary = {
        "number_of_particles": 128,
        "use_normals": 0,
        "normals_strength": 10.0,
        "checkpointing_interval": 1000,
        "keep_checkpoints": 0,
        "iterations_per_split": 1000,
        "optimization_iterations": 1000,
        "starting_regularization": 10,
        "ending_regularization": 1,
        "relative_weighting": 1,
        "initial_relative_weighting": 0.05,
        "procrustes_interval": 0,
        "procrustes_scaling": 0,
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
        parameter_dictionary["multiscale_particles"] = 32

    # Add param dictionary to spreadsheet
    for key in parameter_dictionary:
        parameters.set(key, sw.Variant([parameter_dictionary[key]]))
    project.set_parameters("optimize", parameters)
    spreadsheet_file = project_location + "ellipsoid_mesh_" + args.option_set+ ".swproj"
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
