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

'''
Generates super shapes and saves implicit and mesh form
Mesh is not in aligment with implicit and should just be used for visualization
'''
def get_shapes(m, num_samples, blur_factor, start_id, outDir, nGridPointsPerDimension=200):
	do_mesh = False
	# make folders
	implicitDir = outDir + "implicit/"
	make_dir(implicitDir)
	if do_mesh:
		meshDir= outDir + "mesh/"
		make_dir(meshDir)
	# for implicit resolution
	unitCubeSize = 2 
	# make params csv
	outParams = open(outDir + "parameters.csv", 'w+')
	outParams.write('ID,m,n1,n2,n3,a,b,blur\n')
	for i in range(num_samples):
		print("Generating shape " + str(i+1) + " out of " + str(num_samples))
		name = "id" + get_id_str(i+start_id) + "_ss" + str(m)
		# shape params
		n1 = np.random.uniform(0.75,1)
		n2 = np.random.uniform(0.75,1)
		n3 = n2
		a = 1
		b = 1
		outParams.write(str(i)+','+str(m)+','+str(n1)+','+str(n2)+','+str(n3)+','+str(a)+','+str(b)+','+str(blur_factor)+'\n')
		# get mesh
		if do_mesh:
			X, Y, Z, triIndices = super_formula_3D(m, n1, n2, n3, a, b, 100000)
			verts = np.column_stack((X,Y,Z))
			shapeMesh = trimesh.Trimesh(vertices=verts, faces=triIndices)
			shapeMesh.export(meshDir + name + ".stl")
		# get image
		grid = supershape_3D_implicit(unitCubeSize, nGridPointsPerDimension, a, b, m, n1, n2, n3)
		np_grid = np.float32(grid)
		itk_im_view = itk.image_view_from_array(np_grid)
		itk.imwrite(itk_im_view, implicitDir + name + ".nrrd")
	outParams.close()
	return implicitDir

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
def get_segmentation(implicitDir, outDir):
	segDir = outDir + 'segmentations/'
	make_dir(segDir)
	index = 0
	for file in os.listdir(implicitDir):
		print("Generating segmentation " + str(index))
		name = file.replace('.nrrd', '_seg.nrrd')
		binarize(implicitDir + file, segDir + name)
		index += 1
	return get_files(segDir)

'''
get_segmentation helper
'''
def binarize(image, bin_image):
	cmd = ["shapeworks", "read-image", "--name", image]
	cmd.extend(["binarize"])
	cmd.extend(["write-image", "--name", bin_image, "--compressed", "0"])
	try:
		subprocess.check_call(cmd)
	except:
		cmd = ["shapeworks", "read-image", "--name", image]
		cmd.extend(["binarize"])
		cmd.extend(["write-image", "--name", bin_image])
		subprocess.check_call(cmd)

'''
Get distance transforms of binary images
'''
def get_distance_transforms(seg_folder, out_dir):
	segmentations = get_files(seg_folder)
	mesh_dir = os.path.join(out_dir, 'groom')
	make_dir(mesh_dir)
	dt_dir = os.path.join(out_dir, 'distance_transforms')
	make_dir(dt_dir)
	antialiasIterations=20
	smoothingIterations=1
	alpha=10.5
	beta=10.0
	scaling=20.0
	isoValue=0
	for i in range(len(segmentations)):
		print("Generating distance transform " + str(i))
		inname = segmentations[i]
		outname = inname.replace(os.path.dirname(inname), mesh_dir)
		dtnrrdfilename = outname.replace('.nrrd', '.DT.nrrd')
		tpdtnrrdfilename = outname.replace('.nrrd', '.tpSmoothDT.nrrd')
		isonrrdfilename = outname.replace('.nrrd', '.ISO.nrrd')
		finalnm = tpdtnrrdfilename.replace(mesh_dir, dt_dir)
		cmd = ["shapeworks",
			   "read-image", "--name", inname,
			   "extract-label", "--label", str(1.0),
			   "close-holes",
			   "write-image", "--name", inname,
			   "antialias", "--iterations", str(antialiasIterations),
			   "compute-dt", "--isovalue", str(isoValue),
			   "write-image", "--name", dtnrrdfilename,
			   "curvature", "--iterations", str(smoothingIterations),
			   "write-image", "--name", tpdtnrrdfilename,
			   "topo-preserving-smooth", "--scaling", str(scaling), "--alpha", str(alpha), "--beta", str(beta),
			   "write-image", "--name", isonrrdfilename]
		subprocess.check_call(cmd)
		shutil.copy(tpdtnrrdfilename, dt_dir)
	return dt_dir

'''
Generates image by blurring and adding noise to segmentation
'''
def get_image(blur_factor, segDir, outDir, intensityDiff=False):
	imgDir = outDir + 'images/'
	make_dir(imgDir)
	index = 0
	for file in os.listdir(segDir):
		print("Generating image " + str(index))
		name = file.replace('_seg.nrrd', '_blur' + str(blur_factor) + '.nrrd')
		itk_bin = itk.imread(segDir + file, itk.F)
		img_array = itk.array_from_image(itk_bin)
		img_array = blur(img_array, blur_factor)
		if intensityDiff:
			img_array = apply_noise_diff(img_array, 40)
		else:
			img_array = apply_noise(img_array, 30)
		img_array = np.float32(img_array)
		itk_img_view = itk.image_view_from_array(img_array)
		itk.imwrite(itk_img_view, imgDir + name)
		index += 1

'''
get_image helper
'''
def blur(img, size):
	blur = scipy.ndimage.filters.gaussian_filter(img, size)
	return blur

'''
get_image helper
'''
def apply_noise(img, var):
	img = img*80
	img = img + np.ones(img.shape)*100
	mean = 0
	row,col,ch= img.shape
	sigma = var**0.5
	gauss = np.random.normal(0,sigma,(row,col,ch))
	gauss = gauss.reshape(row,col,ch)
	noisy = img + gauss
	return noisy

'''
get_image helper
'''
def apply_noise_diff(img, var):
	img = img*50
	img = img + np.ones(img.shape)*70
	mean = 0
	row,col,ch= img.shape
	sigma = var**0.5
	gauss = np.random.normal(0,sigma,(row,col,ch))
	gauss = gauss.reshape(row,col,ch)
	noisy = img + gauss
	return noisy

'''
Generates shape in implicit form
Binarizes to make segmentation, blurs and adds noise to make image
'''
def generate(m, num_samples, blur_factor, start_id, outDir,nGridPointsPerDimension=200, intensityDiff=False):
	implicitDir = get_shapes(m, num_samples, blur_factor, start_id, outDir, nGridPointsPerDimension)
	segDir = get_segmentation(implicitDir, outDir)
	dtDir = get_distance_transforms(segDir, outDir)
	imgDir = get_image(blur_factor, segDir, outDir,intensityDiff)

'''
Make folder
'''
def make_dir(dirPath):
    if not os.path.exists(dirPath):
        os.makedirs(dirPath)

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
