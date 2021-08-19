import vtk
import numpy as np
from ShapeCohortGen.CohortGenUtils import *

def addEllipsoid(center, radii, rotation, rotation_axis='Y',resolution=24):
	"""
	Add an ellipsoid centered at [center] with x, y, and z principal axis radii given by
	radii = [radii] pass by generate function = [x_radius,y_radius,z_radius].
	The ellipsoids are rotated with the an angle specified by 'rotation'
	"""
	sphere = vtk.vtkSphereSource()
	sphere.SetCenter([0,0,0])
	sphere.SetThetaResolution(resolution)
	sphere.SetPhiResolution(resolution)
	sphere.SetRadius(1.0)
	sphere.Update()

	transform = vtk.vtkTransform()
	if(rotation_axis=='X'):
		transform.RotateX(rotation)
	elif(rotation_axis=='Y'):
		transform.RotateY(rotation)
	else:
		transform.RotateZ(rotation)

	transform.Scale(radii)
	

	rotation_scaleFilter = vtk.vtkTransformPolyDataFilter()
	rotation_scaleFilter.SetTransform(transform)
	rotation_scaleFilter.SetInputConnection(sphere.GetOutputPort())
	rotation_scaleFilter.Update()

	transform = vtk.vtkTransform()
	transform.Translate(center)

	translateFilter = vtk.vtkTransformPolyDataFilter()
	translateFilter.SetTransform(transform)
	translateFilter.SetInputConnection(rotation_scaleFilter.GetOutputPort())
	translateFilter.Update()

	return translateFilter

def generate_ellipsoids(filename, meshDir, randomize_center, randomize_rotation, randomize_x_radius, randomize_y_radius, randomize_z_radius):
	vtkFileName = meshDir+"ellipsoid_"+filename+".vtk"
	if randomize_center:
		center_loc = list(np.random.randint(low = 0,high=50,size=3))
	else:
		center_loc = [0,0,0]
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
	if randomize_rotation:
		rotation = np.random.randint(low=0,high=180,size=1)[0]
	else:
		rotation = 0
	ellipsoid = addEllipsoid(center_loc,radii,rotation)

	vtk_writer = vtk.vtkPolyDataWriter()
	vtk_writer.SetInputData(ellipsoid.GetOutput())
	vtk_writer.SetFileName(vtkFileName)
	vtk_writer.Update()
	
	
def generate(num_samples,out_dir,randomize_center, randomize_rotation, randomize_x_radius, randomize_y_radius, randomize_z_radius):
	meshDir = out_dir + "meshes/"
	make_dir(meshDir)
	for i in range(num_samples):
		filename = str(i).zfill(2)
		generate_ellipsoids(filename, meshDir, randomize_center, randomize_rotation, randomize_x_radius, randomize_y_radius, randomize_z_radius)
	return get_files(meshDir)
