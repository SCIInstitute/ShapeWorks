import shapeworks as sw 
import glob 
import os 
import shutil
import numpy as np
import subprocess


def calculate_transforms(shapes, reference_shape):
	transforms = []
	number_of_shapes = len(shapes)

	for i in range(number_of_shapes):
		# compute rigid transformation
		rigidTransform = mesh_list[i].createTransform(reference_shape,sw.Mesh.AlignmentType.Rigid,100)  
		transforms.append(rigidTransform)
	return transforms
#---------------------------------------------------------------------------
#-----------------Copy files from test systems to ShapeWorks----------------
#---------------------------------------------------------------------------
# original_dir = "/usr/sci/data/SSM-Data/In-House/Hip/meshes/normal/" 
output_data_dir = "Data/hip/"
# if not os.path.exists(output_data_dir):
# 	os.makedirs(output_data_dir)


# pelvis_files = sorted(glob.glob(original_dir + "/*pelvis.vtk"))
# femur_files = sorted(glob.glob(original_dir + "/*femur.vtk"))

# [shutil.copy(p,output_data_dir) for p in pelvis_files ]
# [shutil.copy(f,output_data_dir) for f in femur_files ]



#---------------------------------------------------------------------------
#-----------------Multiple Domain Alignment---------------------------------
#---------------------------------------------------------------------------



dataset_name = "hip"
output_directory = "Output/hip/"
if not os.path.exists(output_directory):
	os.makedirs(output_directory)
mesh_files = sorted(glob.glob(output_data_dir + "/*.vtk"))[:6]
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


#shape index for all shapes in domain 1 
domain1_indx = domain_ids.index("femur")
#shape index for all shapes in domain 2
domain2_indx = domain_ids.index("pelvis")
"""
Grooming Step 2: Select a reference
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


ref_index = sw.find_reference_mesh_index(combined_meshes)
combined_reference = combined_meshes[ref_index].copy()
domain1_reference = mesh_list[ref_index*domains_per_shape].copy()
domain2_reference = mesh_list[ref_index*domains_per_shape+1].copy()
domain1_ref_name = mesh_names[ref_index*domains_per_shape]
domain2_ref_name = mesh_names[ref_index*domains_per_shape+1]
reference = [domain1_reference,domain2_reference]
ref_name = [domain1_ref_name,domain2_ref_name]
transforms_domain_1 = calculate_transforms(domain_1_meshes, domain1_reference)
transforms_domain_2 = calculate_transforms(domain_2_meshes, domain2_reference)
global_transform = calculate_transforms(combined_meshes, combined_reference)

# print(transforms_domain_1, transforms_domain_2, global_transform)
# """
# Grooming Step 3: Rigid alignment
# Now we can loop over all of the meshes again to find the rigid
# alignment transform 
# """

# transforms = []
# for i in range(len(domain_1_meshes)):
		
# 	# calculate the transformation 
# 	for d in range(domains_per_shape):
# 		# compute rigid transformation
# 		rigidTransform = mesh_list[i*domains_per_shape+d].createTransform(reference[d],sw.Mesh.AlignmentType.Rigid,100)
# 		name = mesh_names[i*domains_per_shape+d]
# 		print('Aligning ' + name + ' to ' + ref_name[d])    
# 		transforms.append(rigidTransform)

# # Save groomed meshes
groomed_mesh_files = sw.utils.save_meshes(groom_dir + 'meshes/', mesh_list, mesh_names, extension='vtk')


# Create project spreadsheet
project_location = output_directory + "shape_models_new/"
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
		rel_mesh_files += sw.utils.get_relative_paths([os.getcwd() + '/' + mesh_files[i*domains_per_shape+d]], project_location)
		rel_groom_files += sw.utils.get_relative_paths([os.getcwd() + '/' + groomed_mesh_files[i*domains_per_shape+d]], project_location)
		# transform.append()
	subject.set_extra_values({"alignment_1": transforms_domain_1[i].flatten()})
	subject.set_extra_values({"alignment_2": transforms_domain_2[i].flatten()})
	subject.set_extra_values({"alignment_global": global_transform[i].flatten()})
	# subject.set_groomed_transforms(transform)
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
	"recompute_regularization_interval" : 1,
	"domains_per_shape" : domains_per_shape,
	"relative_weighting" : 10, 
	"initial_relative_weighting" : 0.1,
	"procrustes_interval" : 0,
	"procrustes_scaling" : 0,
	"save_init_splits" : 0,
	"verbosity" : 0

  }
num_particles = [128,128]

# # If running a tiny test, reduce some parameters
# if args.tiny_test:
# 	num_particles = [32,32]
# 	parameter_dictionary["optimization_iterations"] = 30

# #setting the argument to singlescale for the output filename
# args.use_single_scale = True
# args.option_set = args.option_set.replace("multiscale","singlescale")
# Add param dictionary to spreadsheet
for key in parameter_dictionary:
	parameters.set(key, sw.Variant([parameter_dictionary[key]]))
parameters.set("number_of_particles" ,sw.Variant(num_particles))
project.set_parameters("optimize", parameters)

spreadsheet_file = output_directory + "shape_models/hip_new.xlsx"
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