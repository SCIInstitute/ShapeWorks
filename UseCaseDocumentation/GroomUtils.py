import numpy as np
import io
from termcolor import colored, cprint
import glob
import os
import shutil
import xml.etree.ElementTree as ET


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
		print("########### Resampling ###############")
		cprint(("Input Filename : ", inname), 'cyan')
		cprint(("Output Filename : ", outname), 'yellow')
		print("######################################")
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
		print("############## Padding ###############")
		cprint(("Input Filename : ", inname), 'cyan')
		cprint(("Output Filename : ", outname), 'yellow')
		print("######################################")
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
			print("############# COM Alignment ###############")
			cprint(("Input Segmentation Filename : ", innameSeg), 'cyan')
			cprint(("Input Image Filename : ", innameImg), 'cyan')
			cprint(("Output Segmentation Filename : ", outnameSeg), 'yellow')
			cprint(("Output Image Filename : ", outnameImg), 'yellow')
			cprint(("Output Parameter Filename : ", paramname), 'yellow')
			print("###########################################")
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
			print("############# COM Alignment ###############")
			cprint(("Input Segmentation Filename : ", inname), 'cyan')
			cprint(("Output Segmentation Filename : ", outname), 'yellow')
			cprint(("Output Parameter Filename : ", paramname), 'yellow')
			print("###########################################")
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

def create_tpSmooth_xml(xmlfilename, smoothingIterations, ref_dtnrrdfilename, ref_isonrrdfilename, ref_tpdtnrrdfilename):
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
	create_tpSmooth_xml(xmlfilename, smoothingIterations, ref_dtnrrdfilename, ref_isonrrdfilename, ref_tpdtnrrdfilename)
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
		print("############# Rigid Alignment #############")
		cprint(("Input Segmentation Filename : ", inname), 'cyan')
		cprint(("Input Reference Filename : ", refFile), 'cyan')
		cprint(("Output Segmentation Filename : ", outname), 'yellow')
		print("###########################################")
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
		create_tpSmooth_xml(xmlfilename, smoothingIterations, dtnrrdfilename, isonrrdfilename, tpdtnrrdfilename)
		create_cpp_xml(xmlfilename, xmlfilename)
		execCommand = "TopologyPreservingSmoothing " + xmlfilename
		os.system(execCommand) 	
		execCommand = "ICPRigid3DImageRegistration --targetDistanceMap " + ref_tpdtnrrdfilename + " --sourceDistanceMap " + tpdtnrrdfilename + " --sourceSegmentation " + inname + " --icpIterations " + str(icpIterations) + " --visualizeResult 0 --solutionSegmentation " + outname
		os.system(execCommand)

	return outDataList

def applyCropping(parentDir, inDataListSeg, inDataListImg, paddingSize=10, processRaw=False):
	"""
	Author: Riddhish Bhalodia
	Date: 8th August 2019

	This function takes in a filelist and crops them according to the largest
	bounding box which it discovers
	Input Parameters:
	Output Parameters:
	"""
	outDir = parentDir + '/cropped/'
	if not os.path.exists(outDir):
		os.makedirs(outDir)
	# first create a txtfile with all the scan names in it.
	txtfile = outDir + "dataList.txt"

	with open(txtfile, 'w') as filehandle:
		for listitem in inDataListSeg:
			filehandle.write('%s\n' % listitem)
	
	outPrefix = outDir + "largest_bounding_box"
	execCommand = "FindLargestBoundingBox --paddingSize " + str(paddingSize) + " --inFilename " + txtfile + " --outPrefix " + outPrefix
	os.system(execCommand) 
	# read all the bounding box files for cropping
	bb0 = np.loadtxt(outPrefix + "_bb0.txt")
	bb1 = np.loadtxt(outPrefix + "_bb1.txt")
	bb2 = np.loadtxt(outPrefix + "_bb2.txt")
	smI0 = np.loadtxt(outPrefix + "_smallestIndex0.txt")
	smI1 = np.loadtxt(outPrefix + "_smallestIndex1.txt")
	smI2 = np.loadtxt(outPrefix + "_smallestIndex2.txt")

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
			outnameSeg = outnameSeg.replace('.nrrd', '.cropped.nrrd')
			outDataListSeg.append(outnameSeg)
			sptImg = innameImg.rsplit('/', 1)
			initPath = sptImg[0] + '/'
			filename = sptImg[1]
			outnameImg = innameImg.replace(initPath, outDir)
			outnameImg = outnameImg.replace('.nrrd', '.cropped.nrrd')
			outDataListImg.append(outnameImg)
			print(" ")
			print("############## Cropping ##############")
			cprint(("Input Segmentation Filename : ", innameSeg), 'cyan')
			cprint(("Input Image Filename : ", innameImg), 'cyan')
			cprint(("Output Segmentation Filename : ", outnameSeg), 'yellow')
			cprint(("Output Image Filename : ", outnameImg), 'yellow')
			print("######################################")
			print(" ")
			execCommand = "CropImages --inFilename " + innameSeg + " --outFilename " + outnameSeg + " --bbX " + str(bb0) + " --bbY " + str(bb1) + " --bbZ " + str(bb2) + " --startingIndexX " + str(smI0) + " --startingIndexY " + str(smI1) + " --startingIndexZ " + str(smI2) + " --MRIinFilename " + innameImg + " --MRIoutFilename " + outnameImg
			os.system(execCommand)

		return [outDataListSeg, outDataListImg]
	else:
		outDataList = []
		for i in range(len(inDataListSeg)):
			inname = inDataListSeg[i]
			spt = inname.rsplit('/', 1)
			initPath = spt[0] + '/'
			filename = spt[1]
			outname = inname.replace(initPath, outDir)
			outname = outname.replace('.nrrd', '.cropped.nrrd')
			outDataList.append(outname)
			print(" ")
			print("############## Cropping ##############")
			cprint(("Input Filename : ", inname), 'cyan')
			cprint(("Output Filename : ", outname), 'yellow')
			print("######################################")
			print(" ")
			execCommand = "CropImages --inFilename " + inname + " --outFilename " + outname + " --bbX " + str(bb0) + " --bbY " + str(bb1) + " --bbZ " + str(bb2) + " --startingIndexX " + str(smI0) + " --startingIndexY " + str(smI1) + " --startingIndexZ " + str(smI2) 
			os.system(execCommand)

		return outDataList

def create_meshfromDT_xml(xmlfilename, tpdtnrrdfilename, vtkfilename):
	root = ET.Element('sample')
	lsSmootherIterations = ET.SubElement(root, 'lsSmootherIterations')
	lsSmootherIterations.text = "\n 1.0 \n"
	targetReduction = ET.SubElement(root, 'targetReduction')
	targetReduction.text = "\n  0.0001 \n"
	featureAngle = ET.SubElement(root, 'featureAngle')
	featureAngle.text = "\n 30.0 \n"
	preserveTopology = ET.SubElement(root, 'preserveTopology')
	preserveTopology.text = "\n 1 \n"
	inputs = ET.SubElement(root, 'inputs')
	inputs.text = "\n " + tpdtnrrdfilename + " \n"
	outputs = ET.SubElement(root, 'outputs')
	outputs.text = "\n " + vtkfilename + " \n"

	data = ET.tostring(root, encoding='unicode')
	file = open(xmlfilename, "w+")
	file.write(data)

def applyDistanceTransforms(parentDir, inDataList,antialiasIterations=20, smoothingIterations=1, isoValue=0, percentage=50):
	outDir = parentDir + '/groom_and_meshes/'
	if not os.path.exists(outDir):
		os.makedirs(outDir)

	finalDTDir = parentDir + '/DistanceTransforms/'
	if not os.path.exists(finalDTDir):
		os.makedirs(finalDTDir)

	outDataList = []
	for i in range(len(inDataList)):
		inname = inDataList[i]
		spt = inname.rsplit('/', 1)
		initPath = spt[0] + '/'
		filename = spt[1]
		outname = inname.replace(initPath, outDir)

		dtnrrdfilename = outname.replace('.nrrd', '.DT.nrrd')
		tpdtnrrdfilename = outname.replace('.nrrd', '.tpSmoothDT.nrrd')
		isonrrdfilename = outname.replace('.nrrd', '.ISO.nrrd')
		vtkfilename = outname.replace('.nrrd', '.tpSmoothDT.vtk')
		vtkfilename_preview = outname.replace('.nrrd', '.tpSmoothDT.preview' + str(percentage) + ".vtk")
		finalnm = tpdtnrrdfilename.replace(outDir, finalDTDir)
		outDataList.append(finalnm)


		execCommand = "ExtractGivenLabelImage --inFilename " + inname + " --outFilename " + inname + " --labelVal 1"
		os.system(execCommand)
		execCommand = "CloseHoles --inFilename " + inname + " --outFilename " + inname 
		os.system(execCommand)
		execCommand = "AntiAliasing --inFilename " + inname + " --outFilename " + dtnrrdfilename + " --numIterations " + str(antialiasIterations) 
		os.system(execCommand)
		execCommand = "FastMarching --inFilename " + dtnrrdfilename + " --outFilename " + dtnrrdfilename + " --isoValue " + str(isoValue) 
		os.system(execCommand)
		
		xmlfilename=outname.replace('.nrrd', '.tpSmoothDT.xml')
		create_tpSmooth_xml(xmlfilename, smoothingIterations, dtnrrdfilename, isonrrdfilename, tpdtnrrdfilename)
		create_cpp_xml(xmlfilename, xmlfilename)
		execCommand = "TopologyPreservingSmoothing " + xmlfilename
		os.system(execCommand) 	

		# xmlfilename=outname.replace('.nrrd', '.MeshFromDT.xml')
		# create_meshfromDT_xml(xmlfilename, tpdtnrrdfilename, vtkfilename)
		# create_cpp_xml(xmlfilename, xmlfilename)
		# execCommand = "MeshFromDistanceTransforms " + xmlfilename
		# os.system(execCommand)
		# decimal = percentage/100.0
		# execCommand = "PreviewCmd --inFile " + vtkfilename + " --outFile " + vtkfilename_preview + " --decimationPercentage " + str(decimal)
		# os.system(execCommand)
		# this at the end

		shutil.copy(tpdtnrrdfilename, finalDTDir)
	return outDataList
