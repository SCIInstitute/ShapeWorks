
import os
import glob
import math
import shutil
import subprocess
import numpy as np
import shapeworks as sw
import ShapeCohortGen

def Run_Pipeline(args):
    print("\nStep 1. Extract Data")
    """
    Step 1: EXTRACT DATA

    We define dataset_name which determines which dataset to download from 
    the portal and the directory to save output from the use case in. 
    """
    dataset_name = "supershapes3D"
    output_directory = "Output/incremental_supershapes/"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)
    # If running a tiny_test, then download subset of the data
    if args.tiny_test:
        args.use_single_scale = 1
        sw.data.download_subset(args.use_case, dataset_name, output_directory)
        mesh_files = sorted(glob.glob(output_directory +
                            dataset_name + "/meshes/*.ply"))[:3]
    # else download the entire dataset
    else:
        sw.data.download_and_unzip_dataset(dataset_name, output_directory)
        mesh_files = sorted(glob.glob(output_directory +
                            dataset_name + "/meshes/*.ply"))

        # Select data if using subsample
        if args.use_subsample:
            inputMeshes =[sw.Mesh(filename) for filename in mesh_files]
            sample_idx = sw.data.sample_meshes(inputMeshes, int(args.num_subsample))
            mesh_files = [mesh_files[i] for i in sample_idx]

    """
    Step 2: Sort meshes into batches

    We sort meshes using surface-to-surface distance so that they are ordered from 
    least unque to most unique, then create batches.
    """
    print("\nStep 2: Sort meshes into batches")
    # Load meshes
    meshes = []
    for mesh_file in mesh_files:
        meshes.append(sw.Mesh(mesh_file))
    # Get distances
    print("Sorting based on surface-to-surface distance...")
    distances = np.zeros(len(meshes))
    ref_index = sw.find_reference_mesh_index(meshes)
    ref_mesh = meshes[ref_index]
    for i in range(len(meshes)):
        distances[i] = np.mean(meshes[i].distance(ref_mesh)[0])
    # Sort
    sorted_indices = np.argsort(distances)
    sorted_mesh_files = np.array(mesh_files)[sorted_indices]
    # Make 5 batches
    batch_size = math.ceil(len(mesh_files)/5)
    batches = [sorted_mesh_files[i:i + batch_size] for i in range(0, len(sorted_mesh_files), batch_size)]
    print("Created " + str(len(batches))+ " batches of size " + str(len(batches[0])))
    
    """
    Step 3: Base optimization
    Next we create a base shape model by optimizing on the first batch
    For more details on the plethora of parameters for shapeworks please refer 
    to: http://sciinstitute.github.io/ShapeWorks/workflow/optimize.html
    """
    print("\nStep 3: Optimize initial particles on first batch")
    # Create project spreadsheet
    project_location = output_directory + "shape_models/"
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
        rel_mesh_file = sw.utils.get_relative_paths([os.getcwd() + "/" + batches[0][i]], project_location)
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
        "normal_weight": 10.0,
        "checkpointing_interval": 300,
        "keep_checkpoints": 0,
        "iterations_per_split": 300,
        "optimization_iterations": 300,
        "starting_regularization": 10,
        "ending_regularization": 1,
        "recompute_regularization_interval": 1,
        "domains_per_shape": 1,
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
        parameter_dictionary["number_of_particles"] = 32
        parameter_dictionary["optimization_iterations"] = 1
        parameter_dictionary["starting_regularization"] = 10000
        parameter_dictionary["ending_regularization"] = 1000
    # Add param dictionary to spreadsheet
    for key in parameter_dictionary:
        parameters.set(key, sw.Variant([parameter_dictionary[key]]))
    parameters.set("domain_type",sw.Variant('mesh'))
    project.set_parameters("optimize", parameters)
    spreadsheet_file = output_directory + "shape_models/incremental_supershapes.xlsx"
    project.save(spreadsheet_file)

    # Run optimization
    optimize_cmd = ('shapeworks optimize --name ' + spreadsheet_file).split()
    print(optimize_cmd)
    subprocess.check_call(optimize_cmd)

    """
    Step 4: Incrementally optimize 
    Next we incrementally add meshes in optimization using the previous shape model to
    initialize particles. The mean particles are used as initalization for new shapes.
    """
    print("\nStep 4: Incremental optimization")

    # Update parameters for incremental optimization 
    parameter_dictionary["use_landmarks"] = 1 				# For particle initialization
    parameter_dictionary["iterations_per_split"] = 0 		# No initialization iterations
    parameter_dictionary["optimization_iterations"] = 100 	# Fewer optimization iterations
    parameter_dictionary["multiscale"] = 0 					# Single scale
    if args.tiny_test:
        parameter_dictionary["number_of_particles"] = 32
        parameter_dictionary["optimization_iterations"] = 1
        parameter_dictionary["starting_regularization"] = 10000
        parameter_dictionary["ending_regularization"] = 1000

    # Run optimization on each batch
    for batch_index in range(1, len(batches)):
        print("Running incremental optimization " + str(batch_index) + " out of " + str(len(batches)-1))
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
                rel_mesh_file = sw.utils.get_relative_paths([os.getcwd() + "/" + batches[i][j]], project_location)
                subject.set_original_filenames(rel_mesh_file)
                subject.set_groomed_filenames(rel_mesh_file)
                particle_file = shape_model_dir + os.path.basename(rel_mesh_file[0]).replace(".ply", "_local.particles")
                rel_particle_file = sw.utils.get_relative_paths([os.getcwd() + "/" + particle_file],  project_location)
                subject.set_landmarks_filenames(rel_particle_file)
                subjects.append(subject)
        # Add new shapes in current batch - intialize with meanshape
        for j in range(len(batches[batch_index])):
            subject = sw.Subject()
            subject.set_number_of_domains(1)
            rel_mesh_file = sw.utils.get_relative_paths([os.getcwd() + "/" + batches[batch_index][j]], project_location)
            subject.set_original_filenames(rel_mesh_file)
            subject.set_groomed_filenames(rel_mesh_file)
            rel_particle_file = sw.utils.get_relative_paths([os.getcwd() + "/" + mean_shape_path],  project_location)
            subject.set_landmarks_filenames(rel_particle_file)
            subjects.append(subject)
        # Set project
        project = sw.Project()
        project.set_subjects(subjects)
        parameters = sw.Parameters()

        # Add param dictionary to spreadsheet
        for key in parameter_dictionary:
            parameters.set(key, sw.Variant([parameter_dictionary[key]]))
        parameters.set("domain_type",sw.Variant('mesh'))
        project.set_parameters("optimize", parameters)
        spreadsheet_file = output_directory + "shape_models/incremental_supershapes.xlsx"
        project.save(spreadsheet_file)

        # Run optimization
        optimize_cmd = ('shapeworks optimize --name ' + spreadsheet_file).split()
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
    meshFiles = ss_generator.generate(num_samples, randomize_center=False, randomize_rotation=False, m=-1)


# generate_data()
