import vtk
import numpy as np
from operator import add
from ShapeCohortGen.CohortGenUtils import *
from ShapeCohortGen.Ellipsoids import *



def generate_ellipsoid_joint(num_samples, meshDir, randomize_center, randomize_x_radius, randomize_y_radius, randomize_z_radius,mode_size,mode_rotation,separation=0):

	# randomize radii
	if randomize_x_radius:
		x_radius = np.random.randint(low =15,high=25,size =1)[0]
	else:
		x_radius = 20
	if randomize_y_radius:
		y_radius = np.random.randint(low =5,high=15,size =1)[0]
	else:
		y_radius = 10
	if randomize_z_radius:
		z_radius = np.random.randint(low =5,high=15,size =1)[0]
	else:
		z_radius = 12
	radii = [x_radius,y_radius,z_radius]

	# if the mode of variation is rotation, then the second ellipsoid will be rotated uniformly
	if(mode_rotation==True):
		apply_rotation_flag = 1
	else:
		apply_rotation_flag = 0

	#Rotation increments
	increments = int(180/num_samples)

	radii_1 = radii
	for i in range(num_samples):
		filename = str(i).zfill(2)

		# Initialize file names for two domains
		vtkFileName1 = meshDir+"ellipsoid_joint_"+filename+"_d1.vtk"
		vtkFileName2 = meshDir+"ellipsoid_joint_"+filename+"_d2.vtk"


		# If randomize center is selected
		# Select center for each joint
		if randomize_center:
			center_loc_1 = list(np.random.randint(low = 0,high=50,size=3))
		else:
			center_loc_1 = [0,0,0]
			

		# if mode of variation is size, then the radii of the second ellipsoid are randomized
		if(mode_size == True):
			radii_2 = [np.random.randint(low =15,high=25,size =1)[0],\
					   np.random.randint(low =5,high=15,size =1)[0],\
					   np.random.randint(low =5,high=15,size =1)[0]]
		# if size is not the selected mode of variation, both ellipsoids will have same radii
		else:
			radii_2 = radii_1


		#ellipsoid 1 
		ellipsoid_joint1 = addEllipsoid(center_loc_1,radii_1,0,rotation_axis='Y')

		#Write to .vtk file
		vtk_writer = vtk.vtkPolyDataWriter()
		vtk_writer.SetInputData(ellipsoid_joint1.GetOutput())
		vtk_writer.SetFileName(vtkFileName1)
		vtk_writer.Update()

		# if mode rotation is not selected, this will be zero
		rotation = i*increments*apply_rotation_flag
		# change the center of ellipsoid 2 such that it is above ellipsoid 1 
		# distance between the two surfaces = separation
		location_offset = [0,radii_1[1] + radii_2[1] + separation,0]
		center_loc_2 = list( map(add, center_loc_1, location_offset) )

		#ellipsoid 2
		ellipsoid_joint2 = addEllipsoid(center_loc_2,radii_2,rotation,rotation_axis='Y')

		#write to vtk
		vtk_writer = vtk.vtkPolyDataWriter()
		vtk_writer.SetInputData(ellipsoid_joint2.GetOutput())
		vtk_writer.SetFileName(vtkFileName2)
		vtk_writer.Update()




def generate(num_samples,out_dir,randomize_center, randomize_x_radius, randomize_y_radius, randomize_z_radius,mode_size,mode_rotation,separation):
	if(mode_size==False and mode_rotation == False):
		raise Exception("The joint cohort generation should have atleast one mode of variation. Please turn on either mode_size or mode_rotation or both")

	meshDir = out_dir + "meshes/"
	make_dir(meshDir)
	
	generate_ellipsoid_joint(num_samples, meshDir,randomize_center, randomize_x_radius, randomize_y_radius, randomize_z_radius,mode_size,mode_rotation,separation)
	return get_files(meshDir)