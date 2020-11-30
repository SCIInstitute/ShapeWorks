import vtk
import numpy as np
from CohortGenUtils import *
def addEllipsoid(center, radii, rotation, resolution=24):

	"""
	Add an ellipsoid centered at [center] with x, y, and z principal axis radii given by
	radii = [x_scale, y_scale, z_scale]
	"""
	sphere = vtk.vtkSphereSource()
	sphere.SetCenter([0,0,0])
	sphere.SetThetaResolution(resolution)
	sphere.SetPhiResolution(resolution)
	sphere.SetRadius(1.0)
	sphere.Update()

	transform = vtk.vtkTransform()
	transform.RotateY(rotation)
	transform.Translate(center)
	transform.Scale(radii)


	transformFilter = vtk.vtkTransformPolyDataFilter()
	transformFilter.SetTransform(transform)
	transformFilter.SetInputConnection(sphere.GetOutputPort())
	transformFilter.Update()
	return transformFilter

def generate_ellipsoids(filename,meshDir):
	vtkFileName = meshDir+"ellipsoid_"+filename+".vtk"
	plyFileName = meshDir+"ellipsoid_"+filename+".ply"
	center_loc = list(np.random.randint(low = 0,high=50,size=3))
	x_radius = np.random.randint(low =15,high=25,size =1)
	y_radius = np.random.randint(low =5,high=15,size =1)
	z_radius = np.random.randint(low =5,high=15,size =1)

	radii = [x_radius[0],y_radius[0],z_radius[0]]
	ellipsoid = addEllipsoid(center_loc,radii,0)
	vtk_writer = vtk.vtkPolyDataWriter()
	vtk_writer.SetInputData(ellipsoid.GetOutput())
	vtk_writer.SetFileName(vtkFileName)
	vtk_writer.Update()

	ply_writer = vtk.vtkPLYWriter()
	ply_writer.SetInputData(ellipsoid.GetOutput())
	ply_writer.SetFileName(plyFileName)
	ply_writer.Update()
	

def get_meshes(num_samples,out_dir):
	meshDir = out_dir + "meshes/"
	make_dir(meshDir)
	for i in range(num_samples):
		filename = str(i).zfill(2)
		generate_ellipsoids(filename,meshDir)
	return get_files(meshDir)



