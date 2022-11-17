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

	for i in range(number_of_shapes):
		# compute rigid transformation
		rigidTransform = shapes[i].createTransform(reference_shape,sw.Mesh.AlignmentType.Rigid,100)  
		transforms.append(rigidTransform)
	return transforms
def Run_Pipeline(args):
	print("\nStep 1. Extract Data\n")
	"""
	Step 1: EXTRACT DATA

	We define dataset_name which determines which dataset to download from 
	the portal and the directory to save output from the use case in. 
	""" 
	# output_data_dir = "Data/hip/"
	# if not os.path.exists(output_data_dir):
	#   os.makedirs(output_data_dir)


	# pelvis_files = sorted(glob.glob(original_dir + "/*pelvis.vtk"))
	# femur_files = sorted(glob.glob(original_dir + "/*femur.vtk"))

	# [shutil.copy(p,output_data_dir) for p in pelvis_files ]
	# [shutil.copy(f,output_data_dir) for f in femur_files ]

	dataset_name = "hip"
	output_directory = "Output/hip/"
	if not os.path.exists(output_directory):
		os.makedirs(output_directory)

	
	# If running a tiny_test, then download subset of the data
	if args.tiny_test:
		sw.data.download_subset(
			args.use_case, dataset_name, output_directory)
		mesh_files = sorted(glob.glob(output_directory +
									 dataset_name + "/meshes/*.vtk"))[:6]
	# Else download the entire dataset
	else:
		sw.data.download_and_unzip_dataset(dataset_name, output_directory)
		mesh_files = sorted(glob.glob(output_directory +
									 dataset_name + "/meshes/*L*.vtk"))
		plane_files = sorted(glob.glob(output_directory +
							dataset_name + "/constraints/*L*.json"))
		if args.use_subsample:
			inputMeshes =[sw.Mesh(filename) for filename in mesh_files]
			sample_idx = sw.data.sample_meshes(inputMeshes, int(args.num_subsample),domains_per_shape=2)
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
	# list of shape meshes
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


	#shape index for all shapes in domain 1 
	domain1_indx = domain_ids.index("femur")
	#shape index for all shapes in domain 2
	domain2_indx = domain_ids.index("pelvis")

	# Set reference side (arbitrary)
	ref_side = "L" # chosen so reflection happens in tiny test 
	reflections = [] 
	center_translations = []
	for mesh, name in zip(mesh_list, mesh_names):
		"""
		Grooming step 2: Apply clipping for finsing alignment transform
		"""
		# Load plane
		for plane_file in plane_files:
			if name in plane_file:
				corresponding_plane_file = plane_file
		with open(corresponding_plane_file) as json_file:
			file = json.load(json_file)
			if(file!= None):
				plane = file['planes'][0]['points']
				# Clip mesh
				print("Clipping: " + name)
				mesh.clip(plane[0], plane[1], plane[2])

		"""
		Grooming Step 3: Get reflection transform - We have left and 
		right femurs, so we reflect the non-reference side meshes 
		so that all of the femurs can be aligned.
		"""
		reflection = np.eye(4) # Identity
		if ref_side in name:
			print("Creating reflection transform for: " + name)
			reflection[0][0] = -1 # Reflect across X
			mesh.applyTransform(reflection)
		reflections.append(reflection)



	"""
	Grooming Step 2: Select a reference
	This step requires loading all of the meshes at once so the shape
	closest to the mean can be found and selected as the reference. 
	"""
	domains_per_shape = 2
	num_samples = int(len(mesh_list)/domains_per_shape)
	domain_1_meshes = []
	domain_2_meshes = []
	domain1_reflections = []
	domain2_reflections = []
	# create combined meshes and also generate independent domain mesh list
	combined_meshes = []
	for i in range(num_samples):
		domain_1_meshes.append(mesh_list[i*domains_per_shape])
		domain_2_meshes.append(mesh_list[i*domains_per_shape+1])
		combined_mesh = mesh_list[i*domains_per_shape].copy()
		combined_mesh += mesh_list[i*domains_per_shape+1]
		combined_meshes.append(combined_mesh)

		domain1_reflections.append(reflections[i*domains_per_shape])
		domain2_reflections.append(reflections[i*domains_per_shape + 1])



	ref_index = sw.find_reference_mesh_index(combined_meshes)
	combined_reference = combined_meshes[ref_index].copy()
	domain1_reference = mesh_list[ref_index*domains_per_shape].copy()
	domain2_reference = mesh_list[ref_index*domains_per_shape+1].copy()
	domain1_ref_name = mesh_names[ref_index*domains_per_shape]
	domain2_ref_name = mesh_names[ref_index*domains_per_shape+1]
	reference = [domain1_reference,domain2_reference]
	ref_name = [domain1_ref_name,domain2_ref_name]
	print("Reference shape found: ", domain2_ref_name, domain2_ref_name)


	# """
	# Grooming Step 3: Rigid alignment
	# We calculate three kinds of alignments
	# """

	# Alginment matrix for domain 1 shapes using domain 1 reference 
	rigid_domain_1 = calculate_transforms(domain_1_meshes, domain1_reference)
	# Alginment matrix for domain 2 shapes using domain 2 reference 
	rigid_domain_2 = calculate_transforms(domain_2_meshes, domain2_reference)
	# Alginment matrix for all shapes using the combined reference mesh denoting the global alignment option
	global_transform = calculate_transforms(combined_meshes, combined_reference)

	# Combine transforms to pass to optimizer
	transforms_domain_1 = []
	transforms_domain_2 = []
	global_transforms = []
	for i in range(num_samples):
		transforms_domain_1.append(np.matmul(domain1_reflections[i],rigid_domain_1[i]))
		transforms_domain_2.append(np.matmul(domain2_reflections[i],rigid_domain_2[i]))
		global_transforms.append(np.matmul(domain1_reflections[i],global_transform[i]))

	# Save groomed meshes
	groomed_mesh_files = sw.utils.save_meshes(groom_dir + 'meshes/', mesh_list, mesh_names, extension='vtk')

	print("\nStep 3. Optimize - Particle Based Optimization\n")
	"""
	Step 3: OPTIMIZE - Particle Based Optimization

	Now we can run optimization directly on the meshes.
	For more details on the plethora of parameters for shapeworks please refer 
	to docs/workflow/optimze.md
	http://sciinstitute.github.io/ShapeWorks/workflow/optimize.html
	"""
	# Create project spreadsheet
	project_location = output_directory + "shape_models/"
	if not os.path.exists(project_location):
		os.makedirs(project_location)
	# Set subjects
	subjects = []

	for i in range(len(domain_1_meshes)):
		subject = sw.Subject()
		subject.set_number_of_domains(domains_per_shape)
		rel_mesh_files = []
		rel_groom_files = []
		rel_plane_files=[]
		transform = []
		for d in range(domains_per_shape):
			rel_mesh_files += sw.utils.get_relative_paths([os.getcwd() + '/' + mesh_files[i*domains_per_shape+d]], project_location)
			rel_groom_files += sw.utils.get_relative_paths([os.getcwd() + '/' + groomed_mesh_files[i*domains_per_shape+d]], project_location)
			if(d==0):
				transform.append(transforms_domain_1[i].flatten())
			else:
				transform.append(transforms_domain_2[i].flatten())
			rel_plane_files += sw.utils.get_relative_paths([os.getcwd() + '/' + plane_files[i*domains_per_shape+d]], project_location)
		subject.set_constraints_filenames(rel_plane_files)
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
		"checkpointing_interval" : 200,
		"keep_checkpoints" : 0,
		"iterations_per_split" : 3000,
		"optimization_iterations" : 5000,
		"starting_regularization" :1000,
		"ending_regularization" : 10,
		"recompute_regularization_interval" : 10,
		"domains_per_shape" : domains_per_shape,
		"relative_weighting" : 10, 
		"initial_relative_weighting" : 0.1,
		"save_init_splits" : 0,
		"verbosity" : 0,
		"use_normals": 1,
		"normal_weights": 5
	  }
	num_particles = [128,256]

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
	parameters.set("domain_type",sw.Variant('mesh'))
	project.set_parameters("optimize", parameters)

	spreadsheet_file = output_directory + "shape_models/hip_multiple_domain.xlsx"
	project.save(spreadsheet_file)

	# Run optimization
	optimize_cmd = ('shapeworks optimize --name ' + spreadsheet_file).split()
	subprocess.check_call(optimize_cmd)

	# # If tiny test or verify, check results and exit
	# sw.utils.check_results(args, spreadsheet_file)

	print("\nStep 4. Analysis - Launch ShapeWorksStudio")
	"""
	Step 4: ANALYZE - open in studio
	For more information about the analysis step, see:
	# http://sciinstitute.github.io/ShapeWorks/workflow/analyze.html
	"""
	analyze_cmd = ('ShapeWorksStudio ' + spreadsheet_file).split()
	subprocess.check_call(analyze_cmd)