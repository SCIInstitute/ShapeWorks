import os
import math
import shutil
import numpy as np
import matplotlib.tri as mtri
# import trimesh
from numpy.linalg import inv
import itk
import subprocess
import matplotlib as plt
import scipy
from scipy.stats import chi2
import shapeworks as sw
from CohortGenUtils import *
'''
Generates super shapes and saves implicit and mesh form
Mesh is not in aligment with implicit and should just be used for visualization
'''
def get_shapes(m, num_samples, start_id, outDir, nGridPointsPerDimension=200):
	# make folders
	implicitDir = outDir + "distance_transforms/"
	make_dir(implicitDir)
	# for implicit resolution
	unitCubeSize = 2 
	# make params csv
	outParams = open(outDir + "parameters.csv", 'w+')
	outParams.write('ID,m,n1,n2,n3,a,b,blur\n')
	for i in range(num_samples):
		print("Generating shape " + str(i+1) + " out of " + str(num_samples))
		name = "id" + get_id_str(i+start_id) + "_ss" + str(m)
		# shape params
		n1 = np.random.uniform(0.5,1.5)
		n2 = np.random.uniform(0.5,1.5)
		n3 = n2
		a = 1
		b = 1
		outParams.write(str(i)+','+str(m)+','+str(n1)+','+str(n2)+','+str(n3)+','+str(a)+','+str(b)+'\n')
		# get dt
		grid = supershape_3D_implicit(unitCubeSize, nGridPointsPerDimension, a, b, m, n1, n2, n3)
		np_grid = np.float32(grid)
		itk_im_view = itk.image_view_from_array(np_grid)
		itk.imwrite(itk_im_view, implicitDir + name + ".nrrd")
	outParams.close()
	return get_files(implicitDir)

'''
get_shapes helper
'''
def get_id_str(num):
	string = str(num)
	while len(string) < 4:
		string = '0' + string
	return(string)

'''
get_shapes helper
'''
def super_formula_3D(m, n1, n2, n3, a, b, numPoints):
	numPointsRoot = round(math.sqrt(numPoints))
	theta = np.linspace(-math.pi, math.pi, endpoint=True, num=numPointsRoot)
	phi = np.linspace(-math.pi / 2.0, math.pi/2.0, endpoint=True, num=numPointsRoot)
	theta, phi = np.meshgrid(theta, phi)
	theta, phi = theta.flatten(), phi.flatten()
	r1 = super_formula_2D(m, n1, n2, n3, a, b, theta)
	r2 = super_formula_2D(m, n1, n2, n3, a, b, phi)
	x = r1 * r2 * np.cos(theta) * np.cos(phi)
	y = r1 * r2 * np.sin(theta) * np.cos(phi)
	z = r2 * np.sin(phi)
	tri = mtri.Triangulation(theta, phi)
	return x, y, z, tri.triangles

'''
get_shapes helper
'''
def supershape_3D_implicit(unitCubeSize, nGridPointsPerDimension, a, b, m, n1, n2, n3):
	x = np.linspace(-unitCubeSize, unitCubeSize, nGridPointsPerDimension)
	y = np.linspace(-unitCubeSize, unitCubeSize, nGridPointsPerDimension)
	z = np.linspace(-unitCubeSize, unitCubeSize, nGridPointsPerDimension)
	[x,y,z] = np.meshgrid(x,y,z)
	theta = np.arctan2(y, x)
	r_theta = super_formula_2D(m, n1, n2, n3, a, b, theta)
	phi1 = np.arctan(z * r_theta * np.sin(theta)/ y)
	phi2 = np.arctan(z * r_theta * np.cos(theta)/ x)
	r_phi = super_formula_2D(m, n1, n2, n3, a, b, phi2)
	fullGrid = 1 - ( x**2 + y**2 + (r_theta**2) * (z**2) ) / ( (r_theta**2) * (r_phi**2) )
	return fullGrid

'''
get_shapes helper
'''
def super_formula_2D(m, n1, n2, n3, a, b, theta):
	r = abs((1 / a) * np.cos(m * theta / 4.0))**n2  +  abs((1 / b) * np.sin(m * theta / 4.0))**n3
	return r**(-1 / n1)

'''
Generates segmentation from implicit form by binarizing
'''
def get_segmentations(implicit_shapes, outDir):
	segDir = outDir + 'segmentations/'
	make_dir(segDir)
	index = 1
	for dt in implicit_shapes:
		print("Generating segmentation " + str(index))
		seg_name = dt.replace('distance_transforms/','segmentations/').replace('.nrrd', '_seg.nrrd')
		img = sw.Image(dt)
		img.binarize(0).write(seg_name,0)
		index += 1
	return get_files(segDir)

'''
Generates image by blurring and adding noise to segmentation
'''
def get_images(segs, outDir, blur_factor, foreground_mean, foreground_var, background_mean, background_var):
	imgDir = outDir + 'images/'
	make_dir(imgDir)
	index = 1
	for seg in segs:
		print("Generating image " + str(index))
		name = seg.replace('segmentations/','images/').replace('_seg.nrrd', '_blur' + str(blur_factor) + '.nrrd')
		itk_bin = itk.imread(seg, itk.F)
		img_array = itk.array_from_image(itk_bin)
		img_array = blur(img_array, blur_factor)
		img_array = apply_noise(img_array, foreground_mean, foreground_var, background_mean, background_var)
		img_array = np.float32(img_array)
		itk_img_view = itk.image_view_from_array(img_array)
		itk.imwrite(itk_img_view, name)
		index += 1
	return get_files(imgDir)

'''
get_image helper
'''
def blur(img, size):
	blur = scipy.ndimage.filters.gaussian_filter(img, size)
	return blur

'''
get_image helper
'''
def apply_noise(img, foreground_mean, foreground_var, background_mean, background_var):
	img = img*(foreground_mean-background_mean)
	img = img + np.ones(img.shape)*background_mean
	background_indices = np.where(img < 0.5)
	foreground_indices = np.where(img > 0.5)
	foreground_noise = np.random.normal(0, foreground_var**0.5, img.shape)
	foreground_noise[background_indices] = 0
	background_noise = np.random.normal(0, background_var**0.5, img.shape)
	background_noise[foreground_indices] = 0
	noisy_img = img + foreground_noise + background_noise
	return noisy_img


