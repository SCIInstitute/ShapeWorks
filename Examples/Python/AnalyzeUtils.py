import numpy as np
import io
import glob
import os
import subprocess
import shutil
import xml.etree.ElementTree as ET
from termcolor import colored, cprint

from CommonUtils import *


def create_View2_xml(xmlfilename, dtFiles, localPointFiles, worldPointFiles):
    root = ET.Element('sample')
    input_points = ET.SubElement(root, 'point_files')
    input_points.text = "\n"
    for i in range(len(worldPointFiles)):
        t1 = input_points.text
        t1 = t1 + worldPointFiles[i] + '\n'
        input_points.text = t1

    input_points = ET.SubElement(root, 'world_point_files')
    input_points.text = "\n"
    for i in range(len(worldPointFiles)):
        t1 = input_points.text
        t1 = t1 + worldPointFiles[i] + '\n'
        input_points.text = t1

    distance_transform = ET.SubElement(root, 'distance_transform_files')
    distance_transform.text = "\n"
    for i in range(len(dtFiles)):
        t1 = distance_transform.text
        t1 = t1 + dtFiles[i] + '\n'
        distance_transform.text = t1

    local_point = ET.SubElement(root, 'local_point_files')
    local_point.text = "\n"
    for i in range(len(localPointFiles)):
        t1 = local_point.text
        t1 = t1 + localPointFiles[i] + '\n'
        local_point.text = t1

                
    data = ET.tostring(root, encoding='unicode')
    file = open(xmlfilename, "w+")
    file.write(data)


def launchShapeWorksStudio(parentDir, dtFiles, localPointFiles, worldPointFiles):
    xmlfilename = parentDir + '/shapeworksview2.xml'
    create_View2_xml(xmlfilename, dtFiles, localPointFiles, worldPointFiles)
    create_cpp_xml(xmlfilename, xmlfilename)
    execCommand = ["ShapeWorksStudio" , xmlfilename ]
    subprocess.check_call(execCommand )
    
