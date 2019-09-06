import numpy as np
import io
import glob
import os
import shutil
import xml.etree.ElementTree as ET
from termcolor import colored, cprint

def create_View2_xml(xmlfilename, pointFileList):
	root = ET.Element('sample')
	input_points = ET.SubElement(root, 'point_files')
	input_points.text = "\n"
	for i in range(len(pointFileList)):
		t1 = input_points.text
		t1 = t1 + pointFileList[i] + '\n'
		input_points.text = t1

	data = ET.tostring(root, encoding='unicode')
	file = open(xmlfilename, "w+")
	file.write(data)

def create_cpp_xml(filename, outputfilename):
	'''
		This creates a xml for cpp Shape warp binary
	'''
	opening_tag = "<"
	ending_tag = "</"
	closing_tag = ">"
	tree = ET.parse(str(filename))
	root = tree.getroot()
	children = {}
	for child in root:
		children[child.tag] = child.text
	tags = children.keys()
	xml_text = ""
	for tag in tags:
		xml_text += opening_tag+tag+closing_tag+children[tag]+ending_tag+tag+closing_tag
	file = open(outputfilename,"w")
	file.write(xml_text)
	file.close()

def launchShapeWorksView2(parentDir, pointFileList):

	xmlfilename = parentDir + '/shapeworksview2.xml'
	create_View2_xml(xmlfilename, pointFileList)
	create_cpp_xml(xmlfilename, xmlfilename)
	execCommand = "ShapeWorksView2 " + xmlfilename
	os.system(execCommand)
    
    
def create_ReconstructMeanSurface_xml(xmlfilename, parameterDictionary, distance_transform_files, local_point_files, world_point_files=None):

	root = ET.Element('sample')
	for key in parameterDictionary:
		curparam = ET.SubElement(root, key)
		curparam.text = str(parameterDictionary[key]) + " \n"

	distance_transform = ET.SubElement(root, 'distance_transform_files')
	distance_transform.text = "\n"
	for i in range(len(distance_transform_files)):
		t1 = distance_transform.text
		t1 = t1 + distance_transform_files[i] + '\n'
		distance_transform.text = t1

	local_point = ET.SubElement(root, 'local_point_files')
	local_point.text = "\n"
	for i in range(len(local_point_files)):
		t1 = local_point.text
		t1 = t1 + local_point_files[i] + '\n'
		local_point.text = t1

	if world_point_files is not None:
		world_point = ET.SubElement(root, 'world_point_files')
		world_point.text = "\n"
		for i in range(len(world_point_files)):
			t1 = world_point.text
			t1 = t1 + world_point_files[i] + '\n'
			world_point.text = t1

	data = ET.tostring(root, encoding='unicode')
	file = open(xmlfilename, "w+")
	file.write(data)    
    
def create_ReconstructSurface_xml(xmlfilename, parameterDictionary, local_point_files):

	root = ET.Element('sample')
	for key in parameterDictionary:
		curparam = ET.SubElement(root, key)
		curparam.text = str(parameterDictionary[key]) + " \n"

	local_point = ET.SubElement(root, 'local_point_files')
	local_point.text = "\n"
	for i in range(len(local_point_files)):
		t1 = local_point.text
		t1 = t1 + local_point_files[i] + '\n'
		local_point.text = t1

	data = ET.tostring(root, encoding='unicode')
	file = open(xmlfilename, "w+")
	file.write(data)    
    
def create_ReconstructSamplesAlongPCAModes_xml(xmlfilename, parameterDictionary, world_point_files):

	root = ET.Element('sample')
	for key in parameterDictionary:
		curparam = ET.SubElement(root, key)
		curparam.text = str(parameterDictionary[key]) + " \n"

	world_point = ET.SubElement(root, 'world_point_files')
	world_point.text = "\n"
	for i in range(len(world_point_files)):
		t1 = world_point.text
		t1 = t1 + world_point_files[i] + '\n'
		world_point.text = t1

	data = ET.tostring(root, encoding='unicode')
	file = open(xmlfilename, "w+")
	file.write(data)    
    
def runReconstructMeanSurface(dtFiles, localPointFiles, worldPointFiles, parameterDictionary):
   numP = parameterDictionary['number_of_particles']
   outDir = os.path.dirname(parameterDictionary["out_prefix"])
   parameterFile = outDir + "reconstruct_mean_surface_" + str(numP) + '.xml'
   
   create_ReconstructMeanSurface_xml(parameterFile, parameterDictionary, dtFiles, localPointFiles, worldPointFiles)
   create_cpp_xml(parameterFile, parameterFile)
    
   execCommand = "ReconstructMeanSurface " + parameterFile
   os.system(execCommand)
    
    