import os
import glob
import math
import shutil
import subprocess
import numpy as np
import shapeworks as sw
import ShapeCohortGen


def Run_Pipeline(args):

    # Set options
    if args.interactive:
        # Select sorting type
        sorting_type = input(
            "\nPlease enter method of sorting to use. Options:{random, median, distribution} Default: median \nSorting type: ")
        if sorting_type not in ['random', 'median', 'distribution']:
            sorting_type = 'median'
        print("Using", sorting_type, "sorting.")

        # Set initial model size
        initial_model_size_input = input(
            "\nPlease enter the number of shapes to use in the initial model. Options:{2, 3, ..., 50} Default: 10 \nInitial shapes: ")
        try:
            initial_model_size = int(initial_model_size_input)
            if initial_model_size < 2 or initial_model_size > 50:
                initial_model_size = 10
        except:
            initial_model_size = 10
        print("Using", initial_model_size, "initial shapes.")

        # Set batch size
        incremental_batch_size_input = input(
            "\nPlease enter the number of shapes in a batch to incrementally add. Options:{1, 2, ...} Default: 5 \nIncremental batch size: ")
        try:
            incremental_batch_size = int(incremental_batch_size_input)
            if incremental_batch_size < 1 or incremental_batch_size > (50-initial_model_size):
                incremental_batch_size = 5
        except:
            incremental_batch_size = 5
        print("Using an incremental batch size of", incremental_batch_size, ".")
    else:
        sorting_type = 'median'
        initial_model_size = 10
        incremental_batch_size = 5

    print("\nStep 1. Acquire Data")
    """
    Step 1: ACQUIRE DATA

    We define dataset_name which determines which dataset to download from 
    the portal and the directory to save output from the use case in. 
    """
    dataset_name = "incremental_supershapes"
    output_directory = "Output/incremental_supershapes/"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)
    # If running a tiny_test, then download subset of the data
    if args.tiny_test:
        args.use_single_scale = 1
        dataset_name = "incremental_supershapes_tiny_test"
        sw.download_dataset(dataset_name, output_directory)
        dataset_name = "supershapes3D"
        mesh_files = sorted(glob.glob(output_directory +
                            dataset_name + "/meshes/*.ply"))[:3]
        initial_model_size = 1
        incremental_batch_size = 1
    # else download the entire dataset
    else:
        sw.download_dataset(dataset_name, output_directory)
        dataset_name = "supershapes3D"
        mesh_files = sorted(glob.glob(output_directory +
                            dataset_name + "/meshes/*.ply"))

        # Select data if using subsample
        if args.use_subsample:
            inputMeshes = [sw.Mesh(filename) for filename in mesh_files]
            sample_idx = sw.data.sample_meshes(
                inputMeshes, int(args.num_subsample))
            mesh_files = [mesh_files[i] for i in sample_idx]

    """
    Step 2: Sort meshes into batches
    We sort meshes using the specified sorting method.
    We sort meshes using the specified sorting method.
    """
    print("\nStep 2: Sort meshes into batches")
    if sorting_type == "random":
        print("Randomly sorting.")
        sorted_indices = np.arange(len(mesh_files))
        np.random.shuffle(sorted_indices)
    else:
        # Load meshes
        meshes = []
        for mesh_file in mesh_files:
            meshes.append(sw.Mesh(mesh_file))
        # Get distance matrix
        print("Finding surface-to-surface distances for sorting...")
        distances = np.zeros((len(meshes), len(meshes)))
        for i in range(len(meshes)):
            for j in range(len(meshes)):
                if i != j:
                    distances[i][j] = np.mean(meshes[i].distance(meshes[j])[0])
        median_index = np.argmin(
            np.sum(distances, axis=0) + np.sum(distances, axis=1))
        # Sort
        if sorting_type == "median":
            print("Sorting using median.")
            sorted_indices = np.argsort(
                distances[median_index] + distances[:, median_index])
        elif sorting_type == "distribution":
            print("Sorting using distribution.")
            sorted_indices = [median_index]
            while len(sorted_indices) < len(mesh_files):
                dists = np.sum(distances[sorted_indices], axis=0) + \
                    np.sum(distances[:, sorted_indices], axis=1)
                next_ind = [i for i in np.argsort(
                    dists) if i not in sorted_indices][0]
                sorted_indices.append(next_ind)
        else:
            print("Error: Sorting type unrecognized.")
    sorted_mesh_files = np.array(mesh_files)[sorted_indices]

    # Make batches
    initial_shapes = sorted_mesh_files[:initial_model_size]
    remaining = range(initial_model_size, len(mesh_files))
    incremental_batches = [sorted_mesh_files[i:i + incremental_batch_size]
                           for i in range(initial_model_size, len(sorted_mesh_files), incremental_batch_size)]
    batches = [initial_shapes]+incremental_batches

    """
    Step 3: Base optimization
    Next we create a base shape model by optimizing on the first batch
    For more details on the plethora of parameters for shapeworks please refer 
    to: http://sciinstitute.github.io/ShapeWorks/workflow/optimize.html
    """
    print("\nStep 3: Optimize particles on initial shapes.")
    print("\nStep 3: Optimize particles on initial shapes.")
    # Create project spreadsheet
    project_location = output_directory + "/"
    if not os.path.exists(project_location):
        os.makedirs(project_location)
    # Remove particle dir if it already exists
    shape_model_dir = project_location + 'incremental_supershapes_particles/'
    if os.path.exists(shape_model_dir):
        shutil.rmtree(shape_model_dir)
    # Set subjects
    subjects = []
    number_domains = 1
    for i in range(len(batches[0])):
        subject = sw.Subject()
        subject.set_number_of_domains(number_domains)
        rel_mesh_file = sw.utils.get_relative_paths(
            [os.getcwd() + "/" + batches[0][i]], project_location)
        subject.set_original_filenames(rel_mesh_file)
        subject.set_groomed_filenames(rel_mesh_file)
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
        "checkpointing_interval": 300,
        "keep_checkpoints": 0,
        "iterations_per_split": 300,
        "optimization_iterations": 300,
        "starting_regularization": 10,
        "ending_regularization": 1,
        "relative_weighting": 1,
        "initial_relative_weighting": 0.05,
        "procrustes_interval": 0,
        "procrustes_scaling": 0,
        "save_init_splits": 0,
        "verbosity": 0
    }

    # Run multiscale optimization unless single scale is specified
    if not args.use_single_scale:
        parameter_dictionary["multiscale"] = 1
        parameter_dictionary["multiscale_particles"] = 32
    # If running a tiny test, reduce some parameters
    if args.tiny_test:
        parameter_dictionary["number_of_particles"] = 8
        parameter_dictionary["optimization_iterations"] = 1
        parameter_dictionary["starting_regularization"] = 10000
        parameter_dictionary["ending_regularization"] = 1000
    # Add param dictionary to spreadsheet
    for key in parameter_dictionary:
        parameters.set(key, sw.Variant([parameter_dictionary[key]]))
    project.set_parameters("optimize", parameters)
    spreadsheet_file = output_directory + "/incremental_supershapes.swproj"
    project.save(spreadsheet_file)

    # Run optimization
    optimize_cmd = ('shapeworks optimize --progress --name ' + spreadsheet_file).split()
    print(optimize_cmd)
    subprocess.check_call(optimize_cmd)

    # Analyze initial model
    if args.interactive:
        print("\nOpening studio to display initial model. \nClose studio to continue running the use case.\n")
        analyze_cmd = ('ShapeWorksStudio ' + spreadsheet_file).split()
        subprocess.check_call(analyze_cmd)

    # Analyze initial model
    if args.interactive:
        print("\nOpening studio to display initial model. \nClose studio to continue running the use case.\n")
        analyze_cmd = ('ShapeWorksStudio ' + spreadsheet_file).split()
        subprocess.check_call(analyze_cmd)

    """
    Step 4: Incrementally optimize 
    Next we incrementally add meshes in optimization using the previous shape model to
    initialize particles. The mean particles are used as initalization for new shapes.
    """
    print("\nStep 4: Incremental optimization")

    # Update parameters for incremental optimization
    parameter_dictionary["use_landmarks"] = 1 				# For particle initialization
    # No initialization iterations
    parameter_dictionary["iterations_per_split"] = 0
    # Fewer optimization iterations
    parameter_dictionary["optimization_iterations"] = 100
    parameter_dictionary["multiscale"] = 0 					# Single scale
    if args.tiny_test:
        parameter_dictionary["number_of_particles"] = 8
        parameter_dictionary["optimization_iterations"] = 1
        parameter_dictionary["starting_regularization"] = 10000
        parameter_dictionary["ending_regularization"] = 1000

    # Run optimization on each batch
    for batch_index in range(1, len(batches)):
        print("Running incremental optimization " +
              str(batch_index) + " out of " + str(len(batches)-1))
        # Update meanshape
        sw.utils.findMeanShape(shape_model_dir)
        mean_shape_path = shape_model_dir + '/meanshape_local.particles'
        # Set subjects
        subjects = []
        # Add current shape model (e.g. all previous batches)
        for i in range(0, batch_index):
            for j in range(len(batches[i])):
                subject = sw.Subject()
                subject.set_number_of_domains(1)
                rel_mesh_file = sw.utils.get_relative_paths(
                    [os.getcwd() + "/" + batches[i][j]], project_location)
                subject.set_original_filenames(rel_mesh_file)
                subject.set_groomed_filenames(rel_mesh_file)
                particle_file = shape_model_dir + \
                    os.path.basename(rel_mesh_file[0]).replace(
                        ".ply", "_local.particles")
                rel_particle_file = sw.utils.get_relative_paths(
                    [os.getcwd() + "/" + particle_file],  project_location)
                subject.set_landmarks_filenames(rel_particle_file)
                subjects.append(subject)
        # Add new shapes in current batch - intialize with meanshape
        for j in range(len(batches[batch_index])):
            subject = sw.Subject()
            subject.set_number_of_domains(1)
            rel_mesh_file = sw.utils.get_relative_paths(
                [os.getcwd() + "/" + batches[batch_index][j]], project_location)
            subject.set_original_filenames(rel_mesh_file)
            subject.set_groomed_filenames(rel_mesh_file)
            rel_particle_file = sw.utils.get_relative_paths(
                [os.getcwd() + "/" + mean_shape_path],  project_location)
            subject.set_landmarks_filenames(rel_particle_file)
            subjects.append(subject)
        # Set project
        project = sw.Project()
        project.set_subjects(subjects)
        parameters = sw.Parameters()

        # Add param dictionary to spreadsheet
        for key in parameter_dictionary:
            parameters.set(key, sw.Variant([parameter_dictionary[key]]))
        project.set_parameters("optimize", parameters)
        spreadsheet_file = output_directory + "/incremental_supershapes.swproj"
        project.save(spreadsheet_file)

        # Run optimization
        optimize_cmd = ('shapeworks optimize --progress --name ' +
                        spreadsheet_file).split()
        subprocess.check_call(optimize_cmd)

    # If tiny test or verify, check results and exit
    sw.utils.check_results(args, spreadsheet_file)

    """
    Step 5: ANALYZE - open in studio
    For more information about the analysis step, see:
    # http://sciinstitute.github.io/ShapeWorks/workflow/analyze.html
    """
    print("\nStep 5. Analysis - Launch ShapeWorksStudio")
    analyze_cmd = ('ShapeWorksStudio ' + spreadsheet_file).split()
    subprocess.check_call(analyze_cmd)


"""
The dataset used in this use case was generated using the following function
"""


def generate_data(num_samples=50):
    out_dir = "Output/incremental_supershapes/supershapes3D/"
    ss_generator = ShapeCohortGen.SupershapesCohortGenerator(out_dir)
    meshFiles = ss_generator.generate(
        num_samples, randomize_center=False, randomize_rotation=False, m=-1)


# generate_data()
