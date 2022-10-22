import os
import shapeworks as sw
import vtk
import numpy as np
import shutil
from data_gen_utils import *
import random
MODELS_WORKING_DIR = '/home/sci/nawazish.khan/Public/Spatiotemporal-Polynomial-Regression-Experiments/'
DATASET_NAME = 'Synthetic-Dataset'
# DATASET_NAME = 'Left-Atrium-Dataset'
PROJECT_DIR = f'{MODELS_WORKING_DIR}/{DATASET_NAME}/'
SEED_VAL = 101
random.seed(SEED_VAL)
SEED_VAL = 101

def make_dir(dir_path):
    if os.path.exists(dir_path):
        shutil.rmtree(dir_path)
    os.makedirs(dir_path)

'''
Get list of full paths for files in dir
'''
def get_files(folder):
    file_list = []
    for file in os.listdir(folder):
        file_path = folder + file
        file_path = file_path.replace(" ","")
        file_list.append(file_path)
    file_list = sorted(file_list)
    return file_list

def add_ellipsoid(radii, resolution=24):
	"""
	Add an ellipsoid centered at [center] with x, y, and z principal axis radii given by
	radii = [radii] pass by generate function = [x_radius,y_radius,z_radius].
	The ellipsoids are rotated with the an angle specified by 'rotation'
	"""
	sphere = vtk.vtkSphereSource()
	sphere.SetCenter([0,0,0])
	sphere.SetThetaResolution(resolution)
	sphere.SetPhiResolution(resolution)
	sphere.SetRadius(1)
	sphere.Update()

	transform = vtk.vtkTransform()
	transform.Scale(radii)
	translateFilter = vtk.vtkTransformPolyDataFilter()
	translateFilter.SetTransform(transform)
	translateFilter.SetInputConnection(sphere.GetOutputPort())
	translateFilter.Update()

	return translateFilter
		
def generate_sequential_ellipsoid_meshes(num_samples, sequence_length, period, out_dir):
	mesh_dir = out_dir + "meshes_new/"
	make_dir(mesh_dir)
	for i in range(num_samples):
		ID_num = str(i+1).zfill(2)
		# Z radius fixed for all (diameter of 1)
		z_radius = 0.5 
		# X radius varies per sample (diameter between 0.2 and 1)
		x_radius = np.random.normal(0.6, 0.14)/2
		# Y radius varies cyclically across time (diameter between 0.2 and 1)
		for t in range(sequence_length):
			time_num = str(t+1).zfill(2)
			y_radius = (0.4*np.sin(t*(2*np.pi/period)) + 0.6)/2
			radii = [x_radius*100, y_radius*100, z_radius*100] # Scale to 100
			ellipsoid = add_ellipsoid(radii)
			vtk_filename = mesh_dir+"ID"+ID_num+"_TIME"+time_num+".vtk"
			vtk_writer = vtk.vtkPolyDataWriter()
			vtk_writer.SetInputData(ellipsoid.GetOutput())
			vtk_writer.SetFileName(vtk_filename)
			vtk_writer.Update()
	return(get_files(mesh_dir))

out_dir = PROJECT_DIR
num_samples = 30
sequence_length = 8
period = 8
mesh_files = generate_sequential_ellipsoid_meshes(num_samples, sequence_length, period, out_dir)
print('done')
# seg_files = generate_segmentations(mesh_files, out_dir, randomize_size=False, spacing=[1,1,1], allow_on_boundary=False)
# img_files = generate_images(seg_files, out_dir, blur_factor=1, foreground_mean=180, foreground_var=30, background_mean=80, background_var=30)