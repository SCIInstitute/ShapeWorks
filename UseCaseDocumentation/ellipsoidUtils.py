import numpy as np
from zipfile import ZipFile
import io
import glob
import os

def extract_zip_createFiles(DATA_FLAG):
	"""
	Extract the zipfile into proper directory and create necessary supporting
	files
	"""
	parentDir="TestEllipsoids/"
	filename="Ellipsoids.zip"
	if not os.path.exists(parentDir):
		os.makedirs(parentDir)
	# extract the zipfile
	with ZipFile(filename, 'r') as zipObj:
		zipObj.extractall(path=parentDir)

	if DATA_FLAG:
		lst = sorted(glob.glob("TestEllipsoids/Ellipsoids_UnPrepped/*.nrrd"))
	else:
		lst = sorted(glob.glob("TestEllipsoids/Ellipsoids_Prepped/*.nrrd"))

	return lst

def applyIsotropicResampling(parentDir, inDataListSeg, inDataListRaw):
	"""
	This function takes in a filelist and produces the resampled files in the
	appropriate directory 
	"""