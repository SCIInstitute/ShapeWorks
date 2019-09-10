import numpy as np
import io
import glob
import os
import shutil
import xml.etree.ElementTree as ET
from termcolor import colored, cprint

from CommonUtils import *

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

def launchShapeWorksView2(parentDir, pointFileList):

	xmlfilename = os.path.join(parentDir , 'shapeworksview2.xml')
	create_View2_xml(xmlfilename, pointFileList)
	create_cpp_xml(xmlfilename, xmlfilename)
	execCommand = "ShapeWorksView2 " + xmlfilename
	os.system(execCommand)