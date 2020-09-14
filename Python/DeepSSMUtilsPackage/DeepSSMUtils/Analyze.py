# Jadie Adams
import os
import re
import subprocess
import numpy as np
import shutil
import vtk
from vtk.numpy_interface import dataset_adapter as dsa
from vtk.util import numpy_support
import matplotlib as mpl
mpl.use('agg')
import matplotlib.pyplot as plt 
import GroomUtils

def getDistance(out_dir, DT_dir, prediction_dir, mean_prefix):
	# Step 1: Get list of test names
	test_names = []
	for file in os.listdir(prediction_dir):
		test_names.append(getPrefix(file))
	# Step 2: Get original meshes from test distance transforms
	print("\n\nGetting original meshes from distance transforms...")
	DT_files = []
	for file in os.listdir(DT_dir):
		prefix = getPrefix(file)
		if prefix in test_names:
			DT_files.append(DT_dir + '/'+ file)
	orig_mesh_list = sorted(GetMeshFromDT(DT_files, out_dir + "OriginalMeshes/"))
	# Step 3: Get predicted meshes from predicted particles
	print("\n\nGetting meshes from predicted particles...")
	particle_files = []
	for file in os.listdir(prediction_dir):
		particle_files.append(prediction_dir + file)
	pred_mesh_list = sorted(GetMeshFromParticles(particle_files, out_dir + "PredcitedMeshes/", mean_prefix))
	# Step 4: Get distance between original and predicted mesh
	print("\n\nGetting distance between original and predicted meshes...")
	orig_dist_mesh_list, pred_dist_mesh_list = surface2surfaceDist(orig_mesh_list, pred_mesh_list, out_dir)
	# Step 5: Get aggregate values regarding distance
	avg_distance = AnalyzeDiffs(orig_dist_mesh_list, pred_dist_mesh_list, out_dir)
	print("Done.\n")
	return avg_distance

def getPrefix(path):
	file_name = os.path.basename(path)
	file_name = file_name.replace("predicted_","")
	prefix = "_".join(file_name.split("_")[0:2])
	prefix = prefix.split(".")[0]
	return prefix

def GetMeshFromDT(DT_list, mesh_dir):
	if not os.path.exists(mesh_dir):
		os.makedirs(mesh_dir)
	mesh_files = []
	xml_filename = mesh_dir + "temp.xml"
	for input_file in DT_list:
		output_vtk = mesh_dir + "original_" + getPrefix(input_file) + ".vtk"
		GroomUtils.create_meshfromDT_xml(xml_filename, input_file, output_vtk)
		execCommand = ["MeshFromDistanceTransforms", xml_filename]
		subprocess.check_call(execCommand)
		mesh_files.append(output_vtk)
	return mesh_files

def GetMeshFromParticles(particle_list, mesh_dir, mean_prefix):
	if not os.path.exists(mesh_dir):
		os.makedirs(mesh_dir)
	mesh_files = []
	xmlfilename = mesh_dir + "temp.xml"
	createMeshFromParticlesXML(xmlfilename, mesh_dir, mean_prefix, particle_list)
	execCommand = ["ReconstructSurface", xmlfilename]
	subprocess.check_call(execCommand)
	for file in os.listdir(mesh_dir):
		if ".vtk" in file:
			mesh_files.append(mesh_dir + file)
	return mesh_files

def createMeshFromParticlesXML(xmlfilename, out_dir, mean_prefix, filelist):
	file = open(xmlfilename, "w+")
	file.write("<local_point_files>\n")
	for filename in filelist:
		file.write(str(filename)+"\n")
	file.write("</local_point_files>\n")
	file.write("<out_prefix>\n" + str(out_dir) + "\n</out_prefix>\n")
	file.write("<mean_prefix>\n")
	file.write(str(mean_prefix)+ "\n")
	file.write("</mean_prefix>\n")
	file.write("<display>\nFalse\n</display>\n")
	file.close()

def surface2surfaceDist(orig_list, pred_list, out_dir):
	orig_out_dir = out_dir + "OriginalMeshesWithDistance/"
	if not os.path.exists(orig_out_dir):
		os.makedirs(orig_out_dir)
	pred_out_dir = out_dir + "PredictedMeshesWithDistance/"
	if not os.path.exists(pred_out_dir):
		os.makedirs(pred_out_dir)
	orig_out_list = []
	pred_out_list = []
	for index in range(len(orig_list)):
		orig = orig_list[index]
		pred = pred_list[index]
		name = os.path.basename(orig).replace("original_", "")
		orig_out = orig_out_dir + name
		pred_out = pred_out_dir + name
		orig_out_list.append(orig_out)
		pred_out_list.append(pred_out)
		indicesO = orig_out.replace(".vtk",".indices")
		indicesP = pred_out.replace(".vtk",".indices")
		execCommand = ["HausdorffDistance2",
			"-a", orig,
			"-b", pred,
			"-A", orig_out,
			"-B", pred_out,
			"-Ax", indicesO,
			"-Bx", indicesP,
			"-t", "1"
		]
		subprocess.check_call(execCommand)
	return orig_out_list, pred_out_list

def AnalyzeDiffs(orig_files, pred_files, out_dir):
	diffs = []
	for index in range(len(orig_files)):
		orig_diff = readHdist(orig_files[index])
		pred_diff = readHdist(pred_files[index])
		diff = (orig_diff + pred_diff)/2
		diffs.append(diff)
	diffs = np.array(diffs)
	np.save(out_dir + 'surface-distances.npy', diffs)
	mean = np.mean(diffs)
	print("Mean surface-to-surface disatnce: " + str(mean))
	std = np.std(diffs)
	print("Standard deviation: " + str(std))
	minn = np.min(diffs)
	print("Min: " + str(minn))
	median_index = np.argsort(diffs)[len(diffs)//2]
	median = diffs[median_index]
	print("Median: " + str(median))
	maxx = np.max(diffs)
	print("Max: "+ str(maxx))
	return mean

def readHdist(vtk_file):
	with open(vtk_file) as f:
		lines = f.readlines()
	dist = float(lines[6])
	return dist