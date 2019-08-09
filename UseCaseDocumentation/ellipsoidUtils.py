import numpy as np
from zipfile import ZipFile
import io
from colorama import Fore
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

def applyIsotropicResampling(parentDir, inDataList, isoSpacing, isCenterOn=True, isBinaryImage=True):
	"""
	Author: Riddhish Bhalodia
	Date: 8th August 2019

	This function takes in a filelist and produces the resampled files in the
	appropriate directory.
	Input Parameters:
	Output Parameters:
	"""
	outDir = parentDir + '/resampled/'
	if not os.path.exists(outDir):
		os.makedirs(outDir)
	outDataList = []
	for i in range(len(inDataList)):
		inname = inDataList[i]
		spt = inname.rsplit('/', 1)
		initPath = spt[0] + '/'
		filename = spt[1]
		outname = inname.replace(initPath, outDir)
		outname = outname.replace('.nrrd', '.isores.nrrd')
		outDataList.append(outname)
		print(" ")
		print(Fore.WHITE, "########### Resampling ###############")
		print(Fore.CYAN, "Input Filename : ", inname)
		print(Fore.YELLOW, "Output Filename : ", outname)
		print(Fore.WHITE, "######################################")
		print(" ")
		if isBinaryImage:
			if isCenterOn:
				execCommand = "ResampleVolumesToBeIsotropic --inFilename " + inname + " --outFilename " + outname + " --isoSpacing " + str(isoSpacing) + " --isCenterImageOn 1 --isBinaryImage 1"
				os.system(execCommand)
			else:
				execCommand = "ResampleVolumesToBeIsotropic --inFilename " + inname + " --outFilename " + outname + " --isoSpacing " + str(isoSpacing) + " --isCenterImageOn 0 --isBinaryImage 1"
				os.system(execCommand)
		else:
			if isCenterOn:
				execCommand = "ResampleVolumesToBeIsotropic --inFilename " + inname + " --outFilename " + outname + " --isoSpacing " + str(isoSpacing) + " --isCenterImageOn 1"
				os.system(execCommand)
			else:
				execCommand = "ResampleVolumesToBeIsotropic --inFilename " + inname + " --outFilename " + outname + " --isoSpacing " + str(isoSpacing) + " --isCenterImageOn 0"
				os.system(execCommand)

	return outDataList

def applyPadding(parentDir, inDataList, padSize, padValue=0):
	"""
	Author: Riddhish Bhalodia
	Date: 8th August 2019

	This function takes in a filelist and produces the padded files in the
	appropriate directory.
	Input Parameters:
	Output Parameters:
	"""
	outDir = parentDir + '/padded/'
	if not os.path.exists(outDir):
		os.makedirs(outDir)
	outDataList = []
	for i in range(len(inDataList)):
		inname = inDataList[i]
		spt = inname.rsplit('/', 1)
		initPath = spt[0] + '/'
		filename = spt[1]
		outname = inname.replace(initPath, outDir)
		outname = outname.replace('.nrrd', '.pad.nrrd')
		outDataList.append(outname)
		print(" ")
		print(Fore.WHITE, "############## Padding ###############")
		print(Fore.CYAN, "Input Filename : ", inname)
		print(Fore.YELLOW, "Output Filename : ", outname)
		print(Fore.WHITE, "######################################")
		print(" ")
		execCommand = "PadVolumeWithConstant --inFilename " + inname + " --outFilename " + outname + " --paddingSize " + str(padSize) + " --paddingValue " + str(padValue)
		os.system(execCommand)

	return outDataList

def applyCOMAlignment(parentDir, inDataListSeg, inDataListImg, processRaw=False):
	"""
	Author: Riddhish Bhalodia
	Date: 8th August 2019

	This function takes in a filelist and produces the center of mass aligned
	files in the appropriate directory. If the process_raw flag is set True, 
	then it also applys the same transformation on the corresponding list of 
	raw files (MRI/CT ...)

	Input Parameters:
	Output Parameters:
	"""
	outDir = parentDir + '/com_aligned/'
	if not os.path.exists(outDir):
		os.makedirs(outDir)
	
	if processRaw:
		outDataListSeg = []
		outDataListImg = []
		for i in range(len(inDataListSeg)):
			innameSeg = inDataListSeg[i]
			innameImg = inDataListImg[i]
			sptSeg = innameSeg.rsplit('/', 1)
			initPath = sptSeg[0] + '/'
			filename = sptSeg[1]
			outnameSeg = innameSeg.replace(initPath, outDir)
			outnameSeg = outnameSeg.replace('.nrrd', '.com.nrrd')
			paramname = outnameSeg.replace('.nrrd', '.txt')
			outDataListSeg.append(outnameSeg)
			sptImg = innameImg.rsplit('/', 1)
			initPath = sptImg[0] + '/'
			filename = sptImg[1]
			outnameImg = innameImg.replace(initPath, outDir)
			outnameImg = outnameImg.replace('.nrrd', '.com.nrrd')
			outDataListImg.append(outnameImg)
			print(" ")
			print(Fore.WHITE, "############# COM Alignment ###############")
			print(Fore.CYAN, "Input Segmentation Filename : ", innameSeg)
			print(Fore.CYAN, "Input Image Filename : ", innameImg)
			print(Fore.YELLOW, "Output Segmentation Filename : ", outnameSeg)
			print(Fore.YELLOW, "Output Image Filename : ", outnameImg)
			print(Fore.YELLOW, "Output Parameter Filename : ", paramname)
			print(Fore.WHITE, "###########################################")
			print(" ")
			execCommand = "TranslateShapeToImageOrigin --inFilename " + innameSeg + " --outFilename " + outnameSeg + " --useCenterOfMass 1 --parameterFilename " + paramname + " --MRIinFilename " + innameImg + " --MRIoutFilename " + outnameImg
			os.system(execCommand)

		return [outDataListSeg, outDataListImg]
	else:
		outDataListSeg = []
		for i in range(len(inDataListSeg)):
			inname = inDataListSeg[i]
			spt = inname.rsplit('/', 1)
			initPath = spt[0] + '/'
			filename = spt[1]
			outname = inname.replace(initPath, outDir)
			outname = outname.replace('.nrrd', '.com.nrrd')
			paramname = outname.replace('.nrrd', '.txt')
			outDataListSeg.append(outname)
			print(" ")
			print(Fore.WHITE, "############# COM Alignment ###############")
			print(Fore.CYAN, "Input Segmentation Filename : ", inname)
			print(Fore.YELLOW, "Output Segmentation Filename : ", outname)
			print(Fore.YELLOW, "Output Parameter Filename : ", paramname)
			print(Fore.WHITE,"###########################################")
			print(" ")
			execCommand = "TranslateShapeToImageOrigin --inFilename " + inname + " --outFilename " + outname + " --useCenterOfMass 1 --parameterFilename " + paramname 
			os.system(execCommand)

		return outDataListSeg

def applyRigidAlignment(parentDir, inDataList, refID):
	"""
	Author: Riddhish Bhalodia
	Date: 8th August 2019

	This function takes in a filelist and produces the center of mass aligned
	files in the appropriate directory. If the process_raw flag is set True, 
	then it also applys the same transformation on the corresponding list of 
	raw files (MRI/CT ...)

	Input Parameters:
	Output Parameters:
	"""
	outDir = parentDir + '/aligned/'
	if not os.path.exists(outDir):
		os.makedirs(outDir)
