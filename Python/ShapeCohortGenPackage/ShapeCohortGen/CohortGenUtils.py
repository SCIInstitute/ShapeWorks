import os
import itk
import numpy as np
import scipy
import subprocess
import shutil
import sys
sys.path.append("../../../Examples/Python/")
import GroomUtils

'''
Make folder
'''
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
Delete files with specific extensions
'''
def clean_dir(folder,extension):
	for filename in os.listdir(folder):
		
		if(extension in filename):
			os.remove(folder+"/"+filename)

'''
Generates image by blurring and adding noise to segmentation
'''
def generate_images(segs, outDir, blur_factor, foreground_mean, foreground_var, background_mean, background_var):
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
	meshDir = out_dir + "meshes/"
	make_dir(segDir)
		#Get Mesh lists
	PLYmeshList = get_file_with_ext(meshList,'ply')
	VTKMeshList = get_file_with_ext(meshList,'vtk')
	#get the origin and size based on all the meshes in the VTKMeshList
	origin, size = getMeshInfo(meshDir, VTKMeshList, spacing)
	all_mesh_data = {}
	all_mesh_data["origin"] = origin
	all_mesh_data["size"] = size
	all_mesh_data["spacing"] = spacing

	segList= []
	meshIndex=0
	numMeshes = len(VTKMeshList)
	meshIndexArray = np.array(list(range(numMeshes)))
	#Randomly select 20% meshes for boundary touching samples
	subSampleSize = int(0.2*numMeshes)
	randomSamples = np.random.choice(meshIndexArray,subSampleSize,replace=False)

	#Use the PLY format of meshes to generate Segmentation images
	for mesh in PLYmeshList:
		mesh_name = os.path.basename(mesh)
		prefix = mesh_name.split(".")[0] 
		print("prefix is " , prefix)

		# write xml file
		infoPrefix = os.path.join(segDir, prefix)
		xmlfilename = infoPrefix + "_GenerateBinaryAndDT.xml"
		if os.path.exists(xmlfilename):
			os.remove(xmlfilename)
		xml = open(xmlfilename, "a")
		xml.write("<?xml version=\"1.0\" ?>\n")
		xml.write("<mesh>\n")
		xml.write(mesh+"\n")
		xml.write("</mesh>\n")

		#If the meshIndex is in the randomly selected samples, get the origin and size 
		# of that mesh so that the segmentation image touch the boundary
		if(meshIndex in randomSamples) and allow_on_boundary:
			m_origin, m_size = getMeshInfo(meshDir, [VTKMeshList[meshIndex]], spacing)
			m_data = {}
			m_data["origin"] = m_origin
			m_data["size"] = m_size 
			m_data["spacing"] = spacing
			data = m_data
		#Else use the origin and size calculated using the entire cohort of meshes
		else:
			data = all_mesh_data

		meshIndex+=1

		#List of image axes
		axes = np.array(["x","y","z"])
		#Randomly select two axes which will be padded
		selectedAxes = np.random.choice(axes,2,replace = False)

		# Write origin, size, and spacing data
		for key,value in data.items():
			index = 0
			for dim in ["x","y","z"]:
				#If the dim is in the random selection of axes, add padding
				if dim in selectedAxes and key=="size" and randomize_size:
					#Random padding
					e = np.random.randint(20,30,size=1)
					val = str(value[index] + e[0] )
				else:
					val = str(value[index])
				xml.write("<" + key + "_" + dim + ">" + str(val) + "</" + key + "_" + dim + ">\n")
				index += 1
		xml.close()
		print("########### Turning Mesh To Volume ##############")
		segFile = GroomUtils.rename(mesh, segDir, "", ".nrrd")
		# call generate binary and DT
		execCommand = ["GenerateBinaryAndDTImagesFromMeshes", xmlfilename]
		subprocess.check_call(execCommand)
		os.remove(xmlfilename)
		# save output volume
		output_volume = mesh.replace(".ply", ".rasterized_sp" + str(spacing[0]) + ".nrrd")
		shutil.move(output_volume, segFile)
		segList.append(segFile)
	# Clean the mesh directory of all the unwanted .nrrd files
	clean_dir(meshDir,'nrrd')
	#clean_dir(meshDir,'vtk')
	return segList