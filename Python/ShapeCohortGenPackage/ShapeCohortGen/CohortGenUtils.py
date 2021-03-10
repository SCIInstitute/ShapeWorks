import os
import itk
import numpy as np
import scipy
import sys
import shutil
from termcolor import colored, cprint
from shapeworks import *


'''
Make folder
'''
def make_dir(dir_path):
	if os.path.exists(dir_path):
		shutil.rmtree(dir_path)
	os.makedirs(dir_path)







'''
rename
'''
def rename(inname, outDir, extension_addition, extension_change=''):
    """
    Takes inname path and replaces dir with outdir and adds extension before file type
    """
    initPath = os.path.dirname(inname)
    outname = inname.replace(initPath, outDir)
    current_extension = "." + inname.split(".")[-1]
    if extension_addition != '':
        outname = outname.replace(current_extension, '.' + extension_addition + current_extension)
    if extension_change != '':
        outname = outname.replace(current_extension, extension_change)
    cprint(("Input filename: " + inname), 'cyan')
    cprint(("Output filename: " + outname), 'yellow')
    return outname





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

'''
Get files with specific extensions
'''
def get_file_with_ext(file_list,extension):
	extList =[]
	for file in file_list:
		ext = file.split(".")[-1]
		if(ext==extension):
			extList.append(file)
	extList = sorted(extList)
	return extList

'''
Takes inname path and replaces dir with outdir and adds extension before file type
'''
def rename(inname, outDir, extension_addition, extension_change=''):
	initPath = os.path.dirname(inname)
	outname = inname.replace(initPath, outDir)
	current_extension = "." + inname.split(".")[-1]
	if extension_addition != '':
		outname = outname.replace(current_extension, '.' + extension_addition + current_extension)
	if extension_change != '':
		outname = outname.replace(current_extension, extension_change)
	return outname

'''
Generate segmentations form mesh liost
'''
def generate_segmentations(meshList, out_dir, randomize_size, spacing, allow_on_boundary):
	segDir = out_dir + "segmentations/"
	make_dir(segDir)
	PLYmeshList = get_file_with_ext(meshList,'ply')
	# get dims tht fit all meshes
	bb = MeshUtils.boundingBox(PLYmeshList)
	fit_all_origin = [bb.min[0], bb.min[1], bb.min[2]]
	bb_dims = bb.max-bb.min
	fit_all_dims = [bb_dims[0], bb_dims[1], bb_dims[2]]
	# randomly select 20% meshes for boundary touching samples
	numMeshes = len(PLYmeshList)
	meshIndexArray = np.array(list(range(numMeshes)))
	subSampleSize = int(0.2*numMeshes)
	randomBoundarySamples = np.random.choice(meshIndexArray,subSampleSize,replace=False)
	# loop through meshes and turn to images
	segList = []
	meshIndex = 0
	for mesh_ in PLYmeshList:
		print("Generating seg " + str(meshIndex + 1) + " out of " + str(len(PLYmeshList)))
		segFile = rename(mesh_, segDir, "", ".nrrd")
		segList.append(segFile)
		mesh = Mesh(mesh_)
		# If the meshIndex is in the randomly selected samples, get the origin and size 
		# of that mesh so that the segmentation image touch the boundary
		if allow_on_boundary and (meshIndex in randomBoundarySamples):
			bb = mesh.boundingBox()
			origin = [bb.min[0], bb.min[1], bb.min[2]]
			dims = [bb.max[0]*2, bb.max[1]*2, bb.max[2]*2]
			pad = np.zeros(3)
		else:
			origin = fit_all_origin
			dims = fit_all_dims
			# If randomize size, add random padding to x, y, and z dims
			if randomize_size:
				pad = np.random.randint(5, high=15, size=3)
			else:
				pad = np.full(3, 5)
		origin = list(np.array(origin) - pad)
		dims = list((np.array(dims) + (2*pad)).astype(int))
		image = mesh.toImage(spacing, dims, origin)
		image.write(segFile, 0)
		meshIndex += 1
	return segList

'''
Generates image by blurring and adding noise to segmentation
'''
def generate_images(segs, outDir, blur_factor, foreground_mean, foreground_var, background_mean, background_var):
	imgDir = outDir + 'images/'
	make_dir(imgDir)
	index = 1
	for seg in segs:
		print("Generating image " + str(index) + " out of " + str(len(segs)))
		name = seg.replace('segmentations/','images/').replace('_seg.nrrd', '_blur' + str(blur_factor) + '.nrrd')
		itk_bin = itk.imread(seg, itk.F)
		img_array = itk.array_from_image(itk_bin)
		img_array = blur(img_array, blur_factor)
		img_array = apply_noise(img_array, foreground_mean, foreground_var, background_mean, background_var)
		img_array = np.float32(img_array)
		itk_img_view = itk.image_view_from_array(img_array)
		itk_img_view.SetOrigin(itk_bin.GetOrigin())
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
	background_indices = np.where(img < 0.5)
	foreground_indices = np.where(img > 0.5)
	img = img*(foreground_mean-background_mean)
	img = img + np.ones(img.shape)*background_mean
	foreground_noise = np.random.normal(0, foreground_var**0.5, img.shape)
	foreground_noise[background_indices] = 0
	background_noise = np.random.normal(0, background_var**0.5, img.shape)
	background_noise[foreground_indices] = 0
	noisy_img = img + foreground_noise + background_noise
	return noisy_img

def getMeshInfo(outDir, meshList, spacing):
	# Get VTK meshes
	meshListStr = ''
	for index in range(len(meshList)):
		VTKmesh = meshList[index]
		# VTKmesh = mesh.replace("ply" , "vtk")
		# subprocess.check_call(["ply2vtk", mesh, VTKmesh])
		meshListStr += VTKmesh + '\n'
	# Write XML
	xmlfilename = outDir + "MeshInfo.xml"
	out_origin = outDir + "origin.txt"
	out_size = outDir + "size.txt"
	xml = open(xmlfilename, "a")
	xml.write("<?xml version=\"1.0\" ?>\n")
	xml.write("<mesh>\n")
	xml.write(meshListStr+"\n")
	xml.write("</mesh>\n")
	xml.write("<spacing_x>\n" + str(spacing[0]) + "\n</spacing_x>\n")
	xml.write("<spacing_y>\n" + str(spacing[1]) + "\n</spacing_y>\n")
	xml.write("<spacing_z>\n" + str(spacing[2]) + "\n</spacing_z>\n")
	xml.write("<out_origin_filename>\n" + out_origin + "\n</out_origin_filename>\n")
	xml.write("<out_size_filename>\n" + out_size + "\n</out_size_filename>\n")
	xml.close()
	# Get origin and size
	execCommand = ["ComputeRasterizationVolumeOriginAndSize", xmlfilename]
	subprocess.check_call(execCommand)
	os.remove(xmlfilename)
	origin_file = open(out_origin, 'r')
	origin = np.array(origin_file.read().split()).astype(int)
	size_file = open(out_size, 'r')
	size = np.array(size_file.read().split()).astype(int)
	os.remove(out_origin)
	os.remove(out_size)
	return origin, size


def generate_segmentations(meshList, out_dir, randomize_size, spacing, allow_on_boundary):
	segDir = out_dir + "segmentations/"
	make_dir(segDir)
	PLYmeshList = get_file_with_ext(meshList,'ply')
	# get dims tht fit all meshes
	bb = MeshUtils.boundingBox(PLYmeshList)
	fit_all_origin = [bb.min[0], bb.min[1], bb.min[2]]
	bb_dims = bb.max-bb.min
	fit_all_dims = [bb_dims[0], bb_dims[1], bb_dims[2]]
	# randomly select 20% meshes for boundary touching samples
	numMeshes = len(PLYmeshList)
	meshIndexArray = np.array(list(range(numMeshes)))
	subSampleSize = int(0.2*numMeshes)
	randomBoundarySamples = np.random.choice(meshIndexArray,subSampleSize,replace=False)
	# loop through meshes and turn to images
	segList = []
	meshIndex = 0
	for mesh_ in PLYmeshList:
		print("Generating seg " + str(meshIndex + 1) + " out of " + str(len(PLYmeshList)))
		segFile = rename(mesh_, segDir, "", ".nrrd")
		segList.append(segFile)
		mesh = Mesh(mesh_)
		# If the meshIndex is in the randomly selected samples, get the origin and size 
		# of that mesh so that the segmentation image touch the boundary
		if allow_on_boundary and (meshIndex in randomBoundarySamples):
			bb = mesh.boundingBox()
			origin = [bb.min[0], bb.min[1], bb.min[2]]
			dims = [bb.max[0]*2, bb.max[1]*2, bb.max[2]*2]
			pad = np.zeros(3)
		else:
			origin = fit_all_origin
			dims = fit_all_dims
			# If randomize size, add random padding to x, y, and z dims
			if randomize_size:
				pad = np.random.randint(5, high=15, size=3)
			else:
				pad = np.full(3, 5)
		origin = list(np.array(origin) - pad)
		dims = list((np.array(dims) + (2*pad)).astype(int))
		image = mesh.toImage(spacing, dims, origin)
		image.write(segFile, 0)
		meshIndex += 1
	return segList
