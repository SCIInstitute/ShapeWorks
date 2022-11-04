import os
import glob
import subprocess
import numpy as np
import shapeworks as sw

def Run_Pipeline(args):
    print("\nStep 1. Extract Data")
    """
    Step 1: EXTRACT DATA

    We define dataset_name which determines which dataset to download from 
    the portal and the directory to save output from the use case in. 
    """
    dataset_name = "supershapes3D_multiple_domain"
    output_directory = "Output/supershapes_multi_level/"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)
    # If running a tiny_test, then download subset of the data
    if args.tiny_test:
        args.use_single_scale = 1
        sw.data.download_subset(args.use_case, dataset_name, output_directory)
        mesh_files = sorted(glob.glob(output_directory +
                            dataset_name + "/meshes/*.vtk"))[:15]
    # else download the entire dataset
    else:
        sw.data.download_and_unzip_dataset(dataset_name, output_directory)
        mesh_files = sorted(glob.glob(output_directory +
                            dataset_name + "/meshes/*.vtk"))

        # Select data if using subsample
        if args.use_subsample:
            inputMeshes =[sw.Mesh(filename) for filename in mesh_files]
            sample_idx = sw.data.sample_meshes(inputMeshes, int(args.num_subsample), domains_per_shape=5)
            mesh_files = [mesh_files[i] for i in sample_idx]

    print(f'downloaded')
    """
    Step 2: GROOMING 
    Align the multi-domain meshes using center of mass alignment 
    """

    transforms = {}
    for mesh_file in mesh_files:
        mesh = sw.Mesh(mesh_file)
        translation_matrix = np.identity(4)
        com = np.ones(4)
        com[:3] = mesh.centerOfMass()
        translation_matrix[:, -1] = -com
        transforms[mesh_file] = translation_matrix
    
    
    print("\nStep 3. Optimize - Particle Based Optimization\n")
    """
    Step 3: OPTIMIZE - Particle Based Optimization
    The supershapes data does not require further grooming, so we can run optimization directly on the meshes using the alignment
    For more details on the plethora of parameters for shapeworks please refer 
    to docs/workflow/optimze.md
    http://sciinstitute.github.io/ShapeWorks/workflow/optimize.html
    """

    # get unique subject and domain names from the shape cohort
    subject_names = set()
    domain_names = set()
    for mesh_file in mesh_files:
        mesh_name = mesh_file.split('/')[-1].split('.vtk')[0]
        domain_name = mesh_name.split('_')[2]
        domain_names.add(domain_name)
        subject_names.add(mesh_name.split(domain_name)[0][:-1])
    subject_names = list(subject_names)
    domain_names = list(domain_names)

    # Create project spreadsheet
    domains_per_shape = 5
    project_location = output_directory + "shape_models/"
    if not os.path.exists(project_location):
        os.makedirs(project_location)
    # Set subjects
    subjects = []

    for i in range(len(subject_names)):
        subject = sw.Subject()
        subject.set_number_of_domains(domains_per_shape)
        rel_mesh_files = []
        transform = []
        for d in range(len(domain_names)):
            file_name = [s for s in mesh_files if f"{subject_names[i]}_{domain_names[d]}" in s][0]
            # print(file_name)
            rel_mesh_files += sw.utils.get_relative_paths([os.getcwd() + '/' + file_name], project_location)
            transform.append(transforms[file_name].flatten())
        subject.set_groomed_transforms(transform)
        subject.set_original_filenames(rel_mesh_files)
        subject.set_groomed_filenames(rel_mesh_files)
        subjects.append(subject)

    # Set project
    project = sw.Project()
    project.set_subjects(subjects)
    parameters = sw.Parameters()

    parameter_dictionary = {
        "checkpointing_interval" : 200,
        "keep_checkpoints" : 0,
        "iterations_per_split" : 2000,
        "optimization_iterations" : 1000,
        "starting_regularization" :1000,
        "ending_regularization" : 10,
        "recompute_regularization_interval" : 1,
        "domains_per_shape" : domains_per_shape,
        "relative_weighting" : 1, 
        "initial_relative_weighting" : 0.07,
        "procrustes_interval" : 0,
        "procrustes_scaling" : 0,
        "save_init_splits" : 0,
        "verbosity" : 0
      }
    num_particles = [512] * len(domain_names)

    # If running a tiny test, reduce some parameters
    if args.tiny_test:
        num_particles = [128] * len(domain_names)
        parameter_dictionary["optimization_iterations"] = 200

    #setting the argument to singlescale for the output filename
    args.use_single_scale = True
    args.option_set = args.option_set.replace("multiscale","singlescale")
    # Add param dictionary to spreadsheet
    for key in parameter_dictionary:
        parameters.set(key, sw.Variant([parameter_dictionary[key]]))
    parameters.set("number_of_particles" ,sw.Variant(num_particles))
    project.set_parameters("optimize", parameters)
    
    spreadsheet_file = output_directory + "shape_models/supershapes_multi_level" + args.option_set + ".xlsx"
    project.save(spreadsheet_file)

    # Run optimization
    optimize_cmd = ('shapeworks optimize --name ' + spreadsheet_file).split()
    subprocess.check_call(optimize_cmd)

    # If tiny test or verify, check results and exit
    # TODO: Add data to verification folder
    # sw.utils.check_results(args, spreadsheet_file)

    print("\nStep 4. Analysis - Launch ShapeWorksStudio")
    """
    Step 4: ANALYZE - open in studio and Check Multi-Level Analysis Tab to see Shape and Pose variations
    For more information about the analysis step, see:
    # http://sciinstitute.github.io/ShapeWorks/workflow/analyze.html
    """
    analyze_cmd = ('ShapeWorksStudio ' + spreadsheet_file).split()
    subprocess.check_call(analyze_cmd)
