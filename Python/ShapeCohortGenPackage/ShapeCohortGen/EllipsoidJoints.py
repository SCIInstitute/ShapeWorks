import vtk
import numpy as np
from ShapeCohortGen.CohortGenUtils import *
from Ellipsoids import *



def generate_ellipsoid_joint(num_samples, meshDir, randomize_center,separation):

	# If randomize center is selected
	# Select center for ellipsoid 1
	if randomize_center:
		center_loc1 = list(np.random.randint(low = 0,high=50,size=3))
	else:
		center_loc = [0,0,0]

	# Randomize the radiis for the ellipsoids
	x_radius = np.random.randint(low =15,high=25,size =1)
	y_radius = np.random.randint(low =5,high=15,size =1)
	z_radius = np.random.randint(low =5,high=15,size =1)
	radii = [x_radius[0],y_radius[0],z_radius[0]]
	location_offset = [0,y_radius*2 + separation,0]
	#Rotation increments
	increments = int(180/num_samples)
	for i in range(num_samples):
		filename = str(i).zfill(2)

		# Initialize file names for two domains
		vtkFileName1 = meshDir+"ellipsoid_joint_"+filename+"_d1.vtk"
		vtkFileName1 = meshDir+"ellipsoid_joint_"+filename+"_d1.vtk"
		plyFileName2 = meshDir+"ellipsoid_joint_"+filename+"_d2.ply"
		plyFileName2 = meshDir+"ellipsoid_joint_"+filename+"_d2.ply"
		
		ellipsoid_joint1 = addEllipsoid(center_loc,radii,0,joint=True)

		ply_writer = vtk.vtkPLYWriter()
		ply_writer.SetInputData(ellipsoid_joint1.GetOutput())
		ply_writer.SetFileName(plyFileName1)
		ply_writer.Update()

		vtk_writer = vtk.vtkPolyDataWriter()
		vtk_writer.SetInputData(ellipsoid_joint1.GetOutput())
		vtk_writer.SetFileName(vtkFileName1)
		vtk_writer.Update()

		rotation = i*increments
		ellipsoid_joint2 = addEllipsoid(center_loc + location_offset,radii,rotation,joint=True)

		ply_writer = vtk.vtkPLYWriter()
		ply_writer.SetInputData(ellipsoid_joint2.GetOutput())
		ply_writer.SetFileName(plyFileName2)
		ply_writer.Update()

		vtk_writer = vtk.vtkPolyDataWriter()
		vtk_writer.SetInputData(ellipsoid_joint2.GetOutput())
		vtk_writer.SetFileName(vtkFileName2)
		vtk_writer.Update()




def generate(num_samples,out_dir,randomize_center,separation):
	meshDir = out_dir + "meshes/"
	make_dir(meshDir)
	
	generate_ellipsoid_joint(num_samples, meshDir, randomize_center,separation)
	return get_files(meshDir)