import shapeworks as sw
import glob
import os
import shutil
import numpy as np
import subprocess
import json
"""
Calculate Rigid alignment matrix
Now we can loop over all of the meshes again to find the rigid
alignment transform 
"""


def calculate_transforms(shapes, reference_shape):
    transforms = []
    number_of_shapes = len(shapes)

    center_of_mass = reference_shape.centerOfMass()

    for i in range(number_of_shapes):
        # compute rigid transformation
        rigidTransform = shapes[i].createTransform(reference_shape, sw.Mesh.AlignmentType.Rigid, 100)

        # add centering of the reference shape
        rigidTransform[0, 3] -= center_of_mass[0]
        rigidTransform[1, 3] -= center_of_mass[1]
        rigidTransform[2, 3] -= center_of_mass[2]

        transforms.append(rigidTransform)
    return transforms


def Run_Pipeline(args):
    print("\nStep 1. Acquire Data\n")
    """
	Step 1: ACQUIRE DATA

	We define dataset_name which determines which dataset to download from 
	the portal and the directory to save output from the use case in. 
	"""

    output_directory = "Output/hip_multiple_domain/"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    # If running a tiny_test, then download subset of the data
    if args.tiny_test:
        dataset_name = "hip_multiple_domain_tiny_test"
        sw.download_dataset(dataset_name, output_directory)
        dataset_name = "hip"
        mesh_files = sorted(glob.glob(output_directory +
                                      dataset_name + "/meshes/*.vtk"))[:6]
        plane_files = sorted(glob.glob(output_directory +
                                       dataset_name + "/constraints/*.json"))[:6]
    # Else download the entire dataset
    else:
        dataset_name = "hip_multiple_domain"
        sw.download_dataset(dataset_name, output_directory)
        dataset_name = "hip"
        mesh_files = sorted(glob.glob(output_directory +
                                      dataset_name + "/meshes/*.vtk"))
        plane_files = sorted(glob.glob(output_directory +
                                       dataset_name + "/constraints/*.json"))
        if args.use_subsample:
            inputMeshes = [sw.Mesh(filename) for filename in mesh_files]
            sample_idx = sw.data.sample_meshes(inputMeshes, int(args.num_subsample), domains_per_shape=2)
            mesh_files = [mesh_files[i] for i in sample_idx]

    print("\nStep 2. Groom - Data Pre-processing\n")
    """
	Step 2: GROOMING 
	The required grooming steps are: 
	1. Apply smoothing and remeshing and save groomed meshes
    2. Apply clipping with planes for finding alignment transform
    3. Select reference mesh
    4. Find rigid alignment transform
	For more information on grooming see docs/workflow/groom.md
	"""
    # Create a directory for groomed output
    groom_dir = output_directory + 'groomed/'
    if not os.path.exists(groom_dir):
        os.makedirs(groom_dir)
    """
	First, we need to loop over the mesh files and load them
	"""
    # list of shape meshes
    mesh_list = []
    # list of shape names (shape files prefixes) to be used for saving outputs
    mesh_names = []
    domain_ids = []
    for mesh_file in mesh_files:
        print('Loading: ' + mesh_file)
        # get current shape name
        mesh_name = os.path.splitext(os.path.basename(mesh_file))[0]
        mesh_names.append(mesh_name)
        # get domain identifiers
        domain_id = mesh_name.split("_")[-1]
        domain_ids.append(domain_id)

        # load mesh
        mesh = sw.Mesh(mesh_file)
        # do initial grooming steps
        if not args.tiny_test:
            print("Grooming: " + mesh_name)
            mesh.remeshPercent(percentage=0.9, adaptivity=1.0)
        # append to the mesh list
        mesh_list.append(mesh)

    # shape index for all shapes in domain 1
    domain1_indx = domain_ids.index("femur")
    # shape index for all shapes in domain 2
    domain2_indx = domain_ids.index("pelvis")

    for mesh, name in zip(mesh_list, mesh_names):
        """
        Grooming step 2: Apply clipping for finding alignment transform
        """
        # Load plane
        for plane_file in plane_files:
            if name in plane_file:
                corresponding_plane_file = plane_file
        with open(corresponding_plane_file) as json_file:
            file = json.load(json_file)
            if (file != None):
                plane = file['planes'][0]['points']
                # Clip mesh
                print("Clipping: " + name)
                mesh.clip(plane[0], plane[1], plane[2])

    """
	Grooming Step 3: Select a reference
	This step requires loading all of the meshes at once so the shape
	closest to the mean can be found and selected as the reference. 
	"""
    domains_per_shape = 2
    num_samples = int(len(mesh_list)/domains_per_shape)
    domain_1_meshes = []
    domain_2_meshes = []
    # create combined meshes and also generate independent domain mesh list
    combined_meshes = []
    for i in range(num_samples):
        domain_1_meshes.append(mesh_list[i*domains_per_shape])
        domain_2_meshes.append(mesh_list[i*domains_per_shape+1])
        combined_mesh = mesh_list[i*domains_per_shape].copy()
        combined_mesh += mesh_list[i*domains_per_shape+1]
        combined_meshes.append(combined_mesh)
        # del combined_mesh

    ref_index = sw.find_reference_mesh_index(combined_meshes)
    combined_reference = combined_meshes[ref_index].copy()
    domain1_reference = mesh_list[ref_index*domains_per_shape].copy()
    domain2_reference = mesh_list[ref_index*domains_per_shape+1].copy()
    domain1_ref_name = mesh_names[ref_index*domains_per_shape]
    domain2_ref_name = mesh_names[ref_index*domains_per_shape+1]
    reference = [domain1_reference, domain2_reference]
    ref_name = [domain1_ref_name, domain2_ref_name]
    print("Reference shape found: ", domain2_ref_name, domain2_ref_name)

    # """
    # Grooming Step 4: Rigid alignment
    # We calculate three kinds of alignments
    # """

    # Alginment matrix for domain 1 shapes using domain 1 reference
    transforms_domain_1 = calculate_transforms(domain_1_meshes, domain1_reference)
    # Alginment matrix for domain 2 shapes using domain 2 reference
    transforms_domain_2 = calculate_transforms(domain_2_meshes, domain2_reference)
    # Alginment matrix for all shapes using the combined reference mesh denoting the global alignment option
    global_transforms = calculate_transforms(combined_meshes, combined_reference)

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
        rel_plane_files = []
        transform = []
        for d in range(domains_per_shape):
            rel_mesh_files += sw.utils.get_relative_paths([os.getcwd() + '/' +
                                                          groomed_mesh_files[i*domains_per_shape+d]], project_location)
            rel_groom_files += sw.utils.get_relative_paths([os.getcwd() + '/' +
                                                           groomed_mesh_files[i*domains_per_shape+d]], project_location)
            if (d == 0):
                transform.append(transforms_domain_1[i].flatten())
            else:
                transform.append(transforms_domain_2[i].flatten())

        transform.append(global_transforms[i].flatten())
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
        "iterations_per_split": 2500,
        "optimization_iterations": 200,
        "starting_regularization": 1000,
        "ending_regularization": 10,
        "relative_weighting": 3,
        "initial_relative_weighting": 0.05,
        "save_init_splits": 0,
        "verbosity": 0,
        "use_normals": 1,
        "normals_strength": 5.0,
        "procrustes": 1,
        "procrustes_scaling": 1,
        "procrustes_rotation_translation": 1
    }
    num_particles = [256, 256]

    # If running a tiny test, reduce some parameters
    if args.tiny_test:
        num_particles = [32, 32]
        parameter_dictionary["iterations_per_split"] = 25
        parameter_dictionary["optimization_iterations"] = 25

    # setting the argument to singlescale for the output filename
    args.use_single_scale = True
    args.option_set = args.option_set.replace("multiscale", "singlescale")
    # Add param dictionary to spreadsheet
    for key in parameter_dictionary:
        parameters.set(key, sw.Variant([parameter_dictionary[key]]))
    parameters.set("number_of_particles", sw.Variant(num_particles))
    project.set_parameters("optimize", parameters)

    spreadsheet_file = output_directory + "hip_multiple_domain_" + args.option_set + ".swproj"
    project.save(spreadsheet_file)

    # Run optimization
    optimize_cmd = ('shapeworks optimize --progress --name ' + spreadsheet_file).split()
    subprocess.check_call(optimize_cmd)

    # # If tiny test or verify, check results and exit
    sw.utils.check_results(args, spreadsheet_file)

    print("\nStep 4. Analysis - Launch ShapeWorksStudio")
    """
	Step 4: ANALYZE - open in studio
	For more information about the analysis step, see:
	# http://sciinstitute.github.io/ShapeWorks/workflow/analyze.html
	"""
    analyze_cmd = ('ShapeWorksStudio ' + spreadsheet_file).split()
    subprocess.check_call(analyze_cmd)
