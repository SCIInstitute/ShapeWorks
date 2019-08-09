import numpy as np
from zipfile import ZipFile
import io
from colorama import Fore
import glob
import os
import xml.etree.ElementTree as ET

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

def create_python_xml(xmlfilename, smoothingIterations, ref_dtnrrdfilename, ref_isonrrdfilename, ref_tpdtnrrdfilename):
	root = ET.Element('sample')
	propogationScale = ET.SubElement(root, 'propagationScale')
	propogationScale.text = "\n 20.0 \n"
	alpha = ET.SubElement(root, 'alpha')
	alpha.text = "\n  10.5 \n"
	beta = ET.SubElement(root, 'beta')
	beta.text = "\n 10.0 \n"
	isoVal = ET.SubElement(root, 'isoValue')
	isoVal.text = "\n 0.0 \n"
	smoothing_iterations = ET.SubElement(root, 'smoothing_iterations')
	smoothing_iterations.text = "\n " + str(smoothingIterations) + " \n"
	verbose = ET.SubElement(root, 'verbose')
	verbose.text = "\n 1 \n"
	inputs = ET.SubElement(root, 'inputs')
	inputs.text = "\n " + ref_dtnrrdfilename + " \n"
	outputs = ET.SubElement(root, 'outputs')
	outputs.text = "\n " + ref_isonrrdfilename + " \n"
	dtFiles = ET.SubElement(root, 'dtFiles')
	dtFiles.text = "\n " + ref_tpdtnrrdfilename + " \n"

	data = ET.tostring(root, encoding='unicode')
	file = open(xmlfilename, "w+")
	file.write(data)

def applyRigidAlignment(parentDir, inDataList, refFile, antialiasIterations=20, smoothingIterations=1, isoValue=0, icpIterations=10):
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
	# identify the reference scan
	refDir = outDir + 'reference/'
	if not os.path.exists(refDir):
		os.makedirs(refDir)
	spt = refFile.rsplit('/', 1)
	initPath = spt[0] + '/'
	newRefFile = refFile.replace(initPath, refDir)
	
	ref_dtnrrdfilename=newRefFile.replace('.nrrd', '.DT.nrrd')
	ref_tpdtnrrdfilename=newRefFile.replace('.nrrd', '.tpSmoothDT.nrrd')
	ref_isonrrdfilename=newRefFile.replace('.nrrd', '.ISO.nrrd')
	ref_binnrrdfilename=newRefFile.replace('.nrrd', '.BIN.nrrd')
	
	# reference image processing
	execCommand = "ExtractGivenLabelImage --inFilename " + refFile + " --outFilename " + refFile + " --labelVal 1"
	os.system(execCommand)
	execCommand = "CloseHoles --inFilename " + refFile + " --outFilename " + refFile 
	os.system(execCommand)
	execCommand = "AntiAliasing --inFilename " + refFile + " --outFilename " + ref_dtnrrdfilename + " --numIterations " + str(antialiasIterations) 
	os.system(execCommand)
	execCommand = "FastMarching --inFilename " + ref_dtnrrdfilename + " --outFilename " + ref_dtnrrdfilename + " --isoValue " + str(isoValue) 
	os.system(execCommand)

	xmlfilename=newRefFile.replace('.nrrd', '.tpSmoothDT.xml')
	create_python_xml(xmlfilename, smoothingIterations, ref_dtnrrdfilename, ref_isonrrdfilename, ref_tpdtnrrdfilename)
	create_cpp_xml(xmlfilename, xmlfilename)
	execCommand = "TopologyPreservingSmoothing " + xmlfilename
	os.system(execCommand) 
	execCommand = "ThresholdImages --inFilename " + ref_tpdtnrrdfilename + " --outFilename " + ref_binnrrdfilename + " --lowerThresholdLevel -0.000001" 
	os.system(execCommand)

	outDataList = []
	for i in range(len(inDataList)):
		inname = inDataList[i]
		spt = inname.rsplit('/', 1)
		initPath = spt[0] + '/'
		filename = spt[1]
		outname = inname.replace(initPath, outDir)
		outname = outname.replace('.nrrd', '.aligned.nrrd')
		outDataList.append(outname)

		dtnrrdfilename=outname.replace('.aligned.nrrd', '.aligned.DT.nrrd')
		tpdtnrrdfilename=outname.replace('.aligned.nrrd', '.aligned.tpSmoothDT.nrrd')
		isonrrdfilename=outname.replace('.aligned.nrrd', '.aligned.ISO.nrrd')
		binnrrdfilename=outname.replace('.aligned.nrrd', '.aligned.BIN.nrrd')
		print(" ")
		print(Fore.WHITE, "############# Rigid Alignment #############")
		print(Fore.CYAN, "Input Segmentation Filename : ", inname)
		print(Fore.CYAN, "Input Reference Filename : ", refFile)
		print(Fore.YELLOW, "Output Segmentation Filename : ", outname)
		print(Fore.WHITE,"###########################################")
		print(" ")
		execCommand = "ExtractGivenLabelImage --inFilename " + inname + " --outFilename " + inname + " --labelVal 1"
		os.system(execCommand)
		execCommand = "CloseHoles --inFilename " + inname + " --outFilename " + inname 
		os.system(execCommand)
		execCommand = "AntiAliasing --inFilename " + inname + " --outFilename " + dtnrrdfilename + " --numIterations " + str(antialiasIterations) 
		os.system(execCommand)
		execCommand = "FastMarching --inFilename " + dtnrrdfilename + " --outFilename " + dtnrrdfilename + " --isoValue " + str(isoValue) 
		os.system(execCommand)

		xmlfilename=outname.replace('.aligned.nrrd', '.aligned.tpSmoothDT.xml')
		create_python_xml(xmlfilename, smoothingIterations, dtnrrdfilename, isonrrdfilename, tpdtnrrdfilename)
		create_cpp_xml(xmlfilename, xmlfilename)
		execCommand = "TopologyPreservingSmoothing " + xmlfilename
		os.system(execCommand) 	
		execCommand = "ICPRigid3DImageRegistration --targetDistanceMap " + ref_tpdtnrrdfilename + " --sourceDistanceMap " + tpdtnrrdfilename + " --sourceSegmentation " + inname + " --icpIterations " + str(icpIterations) + " --visualizeResult 0 --solutionSegmentation " + outname
		os.system(execCommand)

	return outDataList

def applyCropping(parentDir, inDataList, paddingSize):
	