#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Sep 10 14:32:27 2019

@author: shireen
"""

import numpy as np
import io
import glob
import os
import shutil
import xml.etree.ElementTree as ET
from termcolor import colored, cprint


def create_cpp_xml(filename, outputfilename):
	'''
		This creates a xml for cpp Shape warp binary
	'''
	opening_tag = "<"
	ending_tag = "</"
	closing_tag = ">"
	newline = "\n"
	tree = ET.parse(str(filename))
	root = tree.getroot()
	children = {}
	for child in root:
		children[child.tag] = child.text
	tags = children.keys()
	xml_text = ""
	for tag in tags:
		xml_text += opening_tag+tag+closing_tag+children[tag]+ending_tag+tag+closing_tag+newline+newline
	file = open(outputfilename,"w")
	file.write(xml_text)
	file.close()
    