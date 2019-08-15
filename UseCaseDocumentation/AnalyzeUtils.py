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