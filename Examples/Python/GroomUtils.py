import sys
import numpy as np
import io
from termcolor import colored, cprint
import glob
import os
import subprocess
import shutil
import xml.etree.ElementTree as ET
import SimpleITK as sitk
import vtk
import vtk.util.numpy_support

from CommonUtils import *

def rename(inname, outDir, extension):
    """
    Takes inname path and replaces dir with outdir and adds extension before file type
    """
    spt = inname.rsplit(os.sep, 1)
    initPath = spt[0]
    outname = inname.replace(initPath, outDir)
    current_extension = "." + inname.split(".")[-1]
    outname = outname.replace(current_extension, '.' + extension + current_extension)
    cprint(("Input Filename : ", inname), 'cyan')
    cprint(("Output Filename : ", outname), 'yellow')
    print("######################################\n")
    return outname

def applyIsotropicResampling(outDir, inDataList, isoSpacing=1.0, recenter=True, isBinary=True):
    """
    This function takes in a filelist and produces the resampled files in the appropriate directory.
    """
    if not os.path.exists(outDir):
        os.makedirs(outDir)
    outDataList = []
    for i in range(len(inDataList)):
        inname = inDataList[i]
        print("\n########### Resampling ###############")
        outname = rename(inname, outDir, 'isores')
        outDataList.append(outname)
        cmd = ["shapeworks", "readimage", "--name", inname, "resample", "--isospacing", str(isoSpacing), "--isbinary", str(isBinary)]
        if recenter:
            cmd.extend(["recenterimage"])
        if isBinary:
            cmd.extend(["binarize"])
        cmd.extend(["writeimage", "--name", outname])
        subprocess.check_call(cmd)
    return outDataList

def applyPadding(outDir, inDataList, padSize, padValue=0):
    """
    This function takes in a filelist and produces the padded files in the appropriate directory.
    """
    if not os.path.exists(outDir):
        os.makedirs(outDir)
    outDataList = []
    for i in range(len(inDataList)):
        inname = inDataList[i]
        print("\n########### Padding ###############")
        outname = rename(inname, outDir, 'pad')
        outDataList.append(outname)
        execCommand = ["PadVolumeWithConstant", "--inFilename", inname, "--outFilename", outname, "--paddingSize", str(padSize), "--paddingValue", str(padValue)]
        subprocess.check_call(execCommand)
    return outDataList

def applyCOMAlignment(outDir, inDataListSeg, raw=[]):
    """
    This function takes in a filelist and produces the center of mass aligned
    files in the appropriate directory. If inDataListImg is provided,
    then it also applys the same transformation on the corresponding list of
    raw files (MRI/CT ...)
    """
    if not os.path.exists(outDir):
        os.makedirs(outDir)
    if raw:
        inDataListImg=raw
        rawoutDir = outDir + '/images'
        if not os.path.exists(rawoutDir):
            os.makedirs(rawoutDir)
        binaryoutDir = outDir + '/segmentations'
        if not os.path.exists(binaryoutDir):
            os.makedirs(binaryoutDir)
        outDataListSeg = []
        outDataListImg = []
        for i in range(len(inDataListSeg)):
            print("\n############# COM Alignment ###############")
            innameSeg = inDataListSeg[i]
            outnameSeg = rename(innameSeg, binaryoutDir, 'com')
            paramname = outnameSeg.replace('.nrrd', '.txt')
            outDataListSeg.append(outnameSeg)
            innameImg = inDataListImg[i]
            outnameImg = rename(innameImg, rawoutDir, 'com')
            outDataListImg.append(outnameImg)
            execCommand = ["TranslateShapeToImageOrigin", "--inFilename", innameSeg, "--outFilename", outnameSeg, "--useCenterOfMass",  "1", 
            "--parameterFilename", paramname, "--MRIinFilename", innameImg, "--MRIoutFilename", outnameImg]
            subprocess.check_call(execCommand)
        return [outDataListSeg, outDataListImg]
    else:
        outDataListSeg = []
        for i in range(len(inDataListSeg)):
            print("\n############# COM Alignment ###############")
            inname = inDataListSeg[i]
            outname = rename(inname, outDir, 'com')
            paramname = outname.replace('.nrrd', '.txt')
            outDataListSeg.append(outname)
            execCommand = ["TranslateShapeToImageOrigin", "--inFilename", inname, "--outFilename", outname, "--useCenterOfMass", "1",
            "--parameterFilename", paramname]
            subprocess.check_call(execCommand)
        return outDataListSeg


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

def FindReferenceImage(inDataList):
    """
    This find the median file between all the input files
    """
    IMG = []
    DIM = []
    for i in range(len(inDataList)):
        tmp = sitk.GetArrayFromImage(sitk.ReadImage(inDataList[i]))
        IMG.append(tmp)
        DIM.append(tmp.shape)
    ref_dim = np.max(DIM, axis =0)
    for i in range(len(inDataList)):
        IMG[i] = np.pad(IMG[i], ((0,ref_dim[0]-DIM[i][0]),(0,ref_dim[1]-DIM[i][1]), (0,ref_dim[2]-DIM[i][2])), mode ='constant', constant_values = 0)
    COM = np.sum(np.asarray(IMG), axis=0) / len(inDataList)
    mean = sitk.GetImageFromArray(COM)
    writer = sitk.ImageFileWriter()
    writer.SetFileName("mean.nrrd")
    writer.Execute(mean)
    idx = np.argmin(np.sqrt(np.sum((np.asarray(IMG) - COM) ** 2, axis=(1, 2, 3))))
    print(" ")
    print("############# Reference File #############")
    cprint(("The reference file for rigid alignment is found"), 'green')
    cprint(("Output Median Filename : ", inDataList[idx]), 'yellow')
    print("###########################################")
    print(" ")
    return inDataList[idx]

def applyRigidAlignment(parentDir, inDataListSeg, inDataListImg, refFile, antialiasIterations=20,
                        smoothingIterations=1, isoValue=0, icpIterations=10, processRaw = False):
    """
    This function takes in a filelists(binary and raw) and produces rigid aligned
    files in the appropriate directory. If the process_raw flag is set True,
    then it also applys the same transformation on the corresponding list of
    raw files (MRI/CT ...)
    """
    outDir = parentDir + '/aligned'
    transoutDir = outDir + '/transformations'

    if not os.path.exists(outDir):
        os.makedirs(outDir)
    if not os.path.exists(transoutDir):
        os.makedirs(transoutDir)

    # identify the reference scan
    refDir = outDir + '/reference'
    if not os.path.exists(refDir):
        os.makedirs(refDir)
    spt = refFile.rsplit(os.sep, 1)
    initPath = spt[0]
    newRefFile = refFile.replace(initPath, refDir)

    ref_dtnrrdfilename = newRefFile.replace('.nrrd', '.DT.nrrd')
    ref_tpdtnrrdfilename = newRefFile.replace('.nrrd', '.tpSmoothDT.nrrd')
    ref_isonrrdfilename = newRefFile.replace('.nrrd', '.ISO.nrrd')
    ref_binnrrdfilename = newRefFile.replace('.nrrd', '.BIN.nrrd')

    # reference image processing
    execCommand = ["ExtractGivenLabelImage", "--inFilename", refFile, "--outFilename", refFile, "--labelVal", " 1"]
    subprocess.check_call(execCommand)
    execCommand = ["CloseHoles",  "--inFilename", refFile, "--outFilename", refFile]
    subprocess.check_call(execCommand)
    execCommand = ["shapeworks", "readimage", "--name", refFile, "antialias", "--numiterations", str(antialiasIterations), "writeimage", "--name", ref_dtnrrdfilename]
    subprocess.check_call(execCommand)

    execCommand = ["FastMarching",  "--inFilename", ref_dtnrrdfilename, "--outFilename", ref_dtnrrdfilename, "--isoValue", str(
        isoValue)]
    subprocess.check_call(execCommand)

    xmlfilename = newRefFile.replace('.nrrd', '.tpSmoothDT.xml')
    create_tpSmooth_xml(xmlfilename, smoothingIterations, ref_dtnrrdfilename, ref_isonrrdfilename, ref_tpdtnrrdfilename)
    create_cpp_xml(xmlfilename, xmlfilename)
    execCommand = ["TopologyPreservingSmoothing", xmlfilename]
    subprocess.check_call(execCommand)
    execCommand = ["ThresholdImages", "--inFilename", ref_tpdtnrrdfilename, "--outFilename", ref_binnrrdfilename, "--lowerThresholdLevel", "-0.000001"]
    subprocess.check_call(execCommand)


    if processRaw:
        rawoutDir = outDir + '/images'
        binaryoutDir = outDir + '/segmentations'

        if not os.path.exists(rawoutDir):
            os.makedirs(rawoutDir)

        if not os.path.exists(binaryoutDir):
            os.makedirs(binaryoutDir)

        outRawDataList=[]
        outSegDataList=[]
        for i in range(len(inDataListSeg)):
            seginname = inDataListSeg[i]
            spt = seginname.rsplit(os.sep, 1)
            initPath = spt[0]
            filename = spt[1]
            segoutname = seginname.replace(initPath, binaryoutDir)
            segoutname = segoutname.replace('.nrrd', '.aligned.nrrd')
            transoutname = seginname.replace(initPath, transoutDir)
            transformation = transoutname.replace('.nrrd', '.transformationMatrix.txt')
            outSegDataList.append(segoutname)

            rawinname = inDataListImg[i]
            sptR = rawinname.rsplit(os.sep, 1)
            initPath = sptR[0] + os.sep
            filename = sptR[1]
            rawoutname = rawinname.replace(initPath, rawoutDir)
            rawoutname = rawoutname.replace('.nrrd', '.aligned.nrrd')
            outRawDataList.append(rawoutname)

            dtnrrdfilename = segoutname.replace('.aligned.nrrd', '.aligned.DT.nrrd')
            tpdtnrrdfilename = segoutname.replace('.aligned.nrrd', '.aligned.tpSmoothDT.nrrd')
            isonrrdfilename = segoutname.replace('.aligned.nrrd', '.aligned.ISO.nrrd')
            binnrrdfilename = segoutname.replace('.aligned.nrrd', '.aligned.BIN.nrrd')
            print(" ")
            print("############# Rigid Alignment #############")
            cprint(("Input Segmentation Filename : ", seginname), 'cyan')
            cprint(("Input Reference Filename : ", refFile), 'cyan')
            cprint(("Input Raw Filename : ", rawinname), 'cyan')
            cprint(("Output Segmentation Filename : ", segoutname), 'yellow')
            cprint(("Output Raw Filename : ", rawoutname), 'yellow')
            cprint(("Output Transformation Matrix : ", transformation), 'yellow')
            print("###########################################")
            print(" ")
            execCommand = ["ExtractGivenLabelImage", "--inFilename", seginname, "--outFilename", seginname, "--labelVal", "1"]
            subprocess.check_call(execCommand)
            execCommand = ["CloseHoles", "--inFilename", seginname, "--outFilename", seginname]
            subprocess.check_call(execCommand)
            execCommand = ["shapeworks", "readimage", "--name", seginname, "antialias", "--numiterations", str(antialiasIterations), "writeimage", "--name", dtnrrdfilename]
            subprocess.check_call(execCommand)
            execCommand = ["FastMarching", "--inFilename", dtnrrdfilename, "--outFilename", dtnrrdfilename, "--isoValue", str(
                isoValue)]
            subprocess.check_call(execCommand)

            xmlfilename = segoutname.replace('.aligned.nrrd', '.aligned.tpSmoothDT.xml')
            create_tpSmooth_xml(xmlfilename, smoothingIterations, dtnrrdfilename, isonrrdfilename, tpdtnrrdfilename)
            create_cpp_xml(xmlfilename, xmlfilename)
            execCommand = ["TopologyPreservingSmoothing", xmlfilename]
            subprocess.check_call(execCommand )
            execCommand = ["ICPRigid3DImageRegistration", "--targetDistanceMap", ref_tpdtnrrdfilename, "--sourceDistanceMap", tpdtnrrdfilename, "--sourceSegmentation", seginname, "--sourceRaw", rawinname, "--icpIterations", str(
                icpIterations), "--visualizeResult",  "0",  "--solutionSegmentation", segoutname, "--solutionRaw", rawoutname, "--solutionTransformation", transformation]
            subprocess.check_call(execCommand )

        return  [outSegDataList, outRawDataList]

    else:

        outDataList = []
        for i in range(len(inDataListSeg)):
            inname = inDataListSeg[i]
            spt = inname.rsplit(os.sep, 1)
            initPath = spt[0]
            #filename = spt[1]
            outname = inname.replace(initPath, outDir)
            outname = outname.replace('.nrrd', '.aligned.nrrd')
            transoutname = inname.replace(initPath, transoutDir)
            transformation = transoutname.replace('.nrrd', '.tarnsormationMatrix.txt')
            outDataList.append(outname)

            dtnrrdfilename = outname.replace('.aligned.nrrd', '.aligned.DT.nrrd')
            tpdtnrrdfilename = outname.replace('.aligned.nrrd', '.aligned.tpSmoothDT.nrrd')
            isonrrdfilename = outname.replace('.aligned.nrrd', '.aligned.ISO.nrrd')
            binnrrdfilename = outname.replace('.aligned.nrrd', '.aligned.BIN.nrrd')
            print(" ")
            print("############# Rigid Alignment #############")
            cprint(("Input Segmentation Filename : ", inname), 'cyan')
            cprint(("Input Reference Filename : ", refFile), 'cyan')
            cprint(("Output Segmentation Filename : ", outname), 'yellow')
            cprint(("Output Transformation Matrix : ", transformation), 'yellow')
            print("###########################################")
            print(" ")
            execCommand = ["ExtractGivenLabelImage", "--inFilename", inname, "--outFilename", inname, "--labelVal",  "1"]
            subprocess.check_call(execCommand )
            execCommand = ["CloseHoles", "--inFilename", inname, "--outFilename", inname]
            subprocess.check_call(execCommand )
            execCommand = ["shapeworks", "readimage", "--name", inname, "antialias", "--numiterations", str(antialiasIterations), "writeimage", "--name", dtnrrdfilename]
            subprocess.check_call(execCommand )
            execCommand = ["FastMarching", "--inFilename", dtnrrdfilename, "--outFilename", dtnrrdfilename, "--isoValue", str(
                isoValue)]
            subprocess.check_call(execCommand )

            xmlfilename = outname.replace('.aligned.nrrd', '.aligned.tpSmoothDT.xml')
            create_tpSmooth_xml(xmlfilename, smoothingIterations, dtnrrdfilename, isonrrdfilename, tpdtnrrdfilename)
            create_cpp_xml(xmlfilename, xmlfilename)
            execCommand = ["TopologyPreservingSmoothing", xmlfilename]
            subprocess.check_call(execCommand )
            execCommand = ["ICPRigid3DImageRegistration", "--targetDistanceMap", ref_tpdtnrrdfilename, "--sourceDistanceMap", tpdtnrrdfilename, "--sourceSegmentation", inname, "--icpIterations", str(
                icpIterations), "--visualizeResult",  "0",  "--solutionSegmentation", outname, "--solutionTransformation", transformation]
            subprocess.check_call(execCommand )

        return outDataList

def applyCropping(parentDir, inDataListSeg, inDataListImg, paddingSize=10, processRaw=False):
    """
    This function takes in a filelist and crops them according to the largest
    bounding box which it discovers
    """
    outDir = parentDir + '/cropped'
    if not os.path.exists(outDir):
        os.makedirs(outDir)
    cropinfoDir = outDir + '/crop_info'
    if not os.path.exists(cropinfoDir):
        os.makedirs(cropinfoDir)
    # first create a txtfile with all the scan names in it.
    txtfile = cropinfoDir + "_dataList.txt"
    with open(txtfile, 'w') as filehandle:
        for listitem in inDataListSeg:
            filehandle.write('%s\n' % listitem)
    outPrefix = cropinfoDir + "largest_bounding_box"
    execCommand = ["FindLargestBoundingBox", "--paddingSize", str(
        paddingSize), "--inFilename", txtfile, "--outPrefix", outPrefix]
    subprocess.check_call(execCommand )
    # read all the bounding box files for cropping
    bb0 = np.loadtxt(outPrefix + "_bb0.txt")
    bb1 = np.loadtxt(outPrefix + "_bb1.txt")
    bb2 = np.loadtxt(outPrefix + "_bb2.txt")
    smI0 = np.loadtxt(outPrefix + "_smallestIndex0.txt")
    smI1 = np.loadtxt(outPrefix + "_smallestIndex1.txt")
    smI2 = np.loadtxt(outPrefix + "_smallestIndex2.txt")
    if processRaw:
        rawoutDir = outDir + '/images'
        binaryoutDir = outDir + '/segmentations'
        if not os.path.exists(rawoutDir):
            os.makedirs(rawoutDir)
        if not os.path.exists(binaryoutDir):
            os.makedirs(binaryoutDir)
        outDataListSeg = []
        outDataListImg = []
        for i in range(len(inDataListSeg)):
            innameSeg = inDataListSeg[i]
            innameImg = inDataListImg[i]
            sptSeg = innameSeg.rsplit(os.sep, 1)
            initPath = sptSeg[0] + os.sep
            #filename = sptSeg[1]
            outnameSeg = innameSeg.replace(initPath, binaryoutDir)
            outnameSeg = outnameSeg.replace('.nrrd', '.cropped.nrrd')
            outDataListSeg.append(outnameSeg)
            sptImg = innameImg.rsplit(os.sep, 1)
            initPath = sptImg[0] + os.sep
            #filename = sptImg[1]
            outnameImg = innameImg.replace(initPath, rawoutDir)
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
            execCommand = ["CropImages", "--inFilename", innameSeg, "--outFilename", outnameSeg, "--bbX", str(
                bb0), "--bbY", str(bb1), "--bbZ", str(bb2), "--startingIndexX", str(
                smI0), "--startingIndexY", str(smI1), "--startingIndexZ", str(
                smI2), "--MRIinFilename", innameImg, "--MRIoutFilename", outnameImg]
            subprocess.check_call(execCommand )
        return [outDataListSeg, outDataListImg]
    else:
        outDataList = []
        for i in range(len(inDataListSeg)):
            inname = inDataListSeg[i]
            spt = inname.rsplit(os.sep, 1)
            initPath = spt[0]
            #filename = spt[1]
            outname = inname.replace(initPath, outDir)
            outname = outname.replace('.nrrd', '.cropped.nrrd')
            outDataList.append(outname)
            print(" ")
            print("############## Cropping ##############")
            cprint(("Input Filename : ", inname), 'cyan')
            cprint(("Output Filename : ", outname), 'yellow')
            print("######################################")
            print(" ")
            execCommand = ["CropImages", "--inFilename", inname, "--outFilename", outname, "--bbX", str(
                bb0), "--bbY", str(bb1), "--bbZ", str(bb2), "--startingIndexX", str(
                smI0), "--startingIndexY", str(smI1), "--startingIndexZ", str(smI2)]
            subprocess.check_call(execCommand )
        return outDataList

def create_meshfromDT_xml(xmlfilename, tpdtnrrdfilename, vtkfilename):
    file = open(xmlfilename, "w+")
    file.write("<lsSmootherIterations>\n1.0\n</lsSmootherIterations>")
    file.write("<targetReduction>\n0.0001\n</targetReduction>")
    file.write("<featureAngle>\n30.0\n</featureAngle>")
    file.write("<preserveTopology>\n1\n</preserveTopology>")
    file.write("<inputs>\n" + str(tpdtnrrdfilename) +"\n</inputs>")
    file.write("<outputs>\n"+str(vtkfilename) + "\n</outputs>")
    file.close()

def applyDistanceTransforms(parentDir, inDataList,antialiasIterations=20, smoothingIterations=1, isoValue=0, percentage=50):
    outDir = parentDir + '/groom_and_meshes'
    if not os.path.exists(outDir):
        os.makedirs(outDir)

    finalDTDir = parentDir + '/distance_transforms'
    if not os.path.exists(finalDTDir):
        os.makedirs(finalDTDir)

    outDataList = []
    for i in range(len(inDataList)):
        inname = inDataList[i]
        initPath = os.path.dirname(inDataList[i])
        outname = inname.replace(initPath, outDir)

        dtnrrdfilename = outname.replace('.nrrd', '.DT.nrrd')
        tpdtnrrdfilename = outname.replace('.nrrd', '.tpSmoothDT.nrrd')
        isonrrdfilename = outname.replace('.nrrd', '.ISO.nrrd')
        vtkfilename = outname.replace('.nrrd', '.tpSmoothDT.vtk')
        vtkfilename_preview = outname.replace('.nrrd', '.tpSmoothDT.preview' + str(percentage) + ".vtk")
        finalnm = tpdtnrrdfilename.replace(outDir, finalDTDir)
        outDataList.append(finalnm)


        execCommand = ["ExtractGivenLabelImage", "--inFilename", inname, "--outFilename", inname, "--labelVal", "1"]
        subprocess.check_call(execCommand )
        execCommand = ["CloseHoles",  "--inFilename", inname, "--outFilename", inname ]
        subprocess.check_call(execCommand )
        execCommand = ["shapeworks", "readimage", "--name", inname, "antialias", "--numiterations", str(antialiasIterations), "writeimage", "--name", dtnrrdfilename]
        subprocess.check_call(execCommand )
        execCommand = ["FastMarching", "--inFilename", dtnrrdfilename, "--outFilename", dtnrrdfilename, "--isoValue", str(isoValue) ]
        subprocess.check_call(execCommand )
        
        xmlfilename=outname.replace('.nrrd', '.tpSmoothDT.xml')
        create_tpSmooth_xml(xmlfilename, smoothingIterations, dtnrrdfilename, isonrrdfilename, tpdtnrrdfilename)
        create_cpp_xml(xmlfilename, xmlfilename)
        execCommand = ["TopologyPreservingSmoothing", xmlfilename]
        subprocess.check_call(execCommand )
        shutil.copy(tpdtnrrdfilename, finalDTDir)
    return outDataList


### Mesh Grooming 

# Refelcts images and meshes to reference side
def anatomyPairsToSingles(parentDir, inputDir, img_suffix, left_suffix, right_suffix, mesh_extension, reference_side):
    outDir = parentDir + "reflected/"
    if not os.path.exists(outDir):
        os.mkdir(outDir)
    for file in os.listdir(inputDir):
        if img_suffix in file:
            subject_id = file.split(img_suffix)[0]
            imgFile = inputDir + file
            leftFilename = inputDir + subject_id + left_suffix + "."+ mesh_extension
            rightFilename = inputDir + subject_id + right_suffix + "." + mesh_extension
            leftFilename_out = outDir + subject_id + left_suffix + "."+ mesh_extension
            rightFilename_out = outDir + subject_id + right_suffix + "." + mesh_extension
            leftimgFilename = outDir + subject_id + left_suffix + "_" + img_suffix + ".nrrd"
            rightimgFilename = outDir + subject_id + right_suffix + "_" + img_suffix + ".nrrd"
            centerFilename = outDir + file.split(".nrrd")[0] + "_origin.txt"
            if reference_side == "right":
                if os.path.exists(rightFilename):
                    shutil.copy(imgFile, rightimgFilename)
                    shutil.copy(rightFilename, rightFilename_out)
                # if left exists, reflect the mesh and image
                if os.path.exists(leftFilename):

                    print("\n############## Reflecting ###############")
                    cprint(("Input Volume : ", imgFile), 'cyan')
                    cprint(("Output Volume : ", leftimgFilename), 'yellow')
                    cprint(("Mesh : ", leftFilename), 'cyan')
                    print("#######################################\n")
                    execCommand = ["ReflectVolumes", "--inFilename", imgFile, "--outFilename", leftimgFilename, "--centerFilename", centerFilename, "--inputDirection", "0"]
                    subprocess.check_call(execCommand)
                    execCommand = ["ReflectMesh", "--inFilename", leftFilename, "--outFilename", leftFilename_out, "--reflectCenterFilename", centerFilename, "--inputDirection", "0", "--meshFormat", mesh_extension]
                    subprocess.check_call(execCommand)
            else:
                if os.path.exists(leftFilename):
                    shutil.copy(imgFile, leftimgFilename)
                    shutil.copy(leftFilename, leftFilename_out)
                # if right exists, reflect the mesh and image
                if os.path.exists(rightFilename):
                    print("############## Reflecting ###############")
                    cprint(("Input Volume : ", imgFile), 'cyan')
                    cprint(("Output Volume : ", rightimgFilename), 'yellow')
                    cprint(("Mesh : ", rightFilename), 'cyan')
                    print("######################################")
                    execCommand = ["ReflectVolumes", "--inFilename", imgFile, "--outFilename", rightimgFilename, "--centerFilename", centerFilename, "--inputDirection", "0"]
                    subprocess.check_call(execCommand)
                    execCommand = ["ReflectMesh", "--inFilename", rightFilename, "--outFilename", rightFilename_out, "--reflectCenterFilename", centerFilename, "--inputDirection", "0", "--meshFormat", mesh_extension]
                    subprocess.check_call(execCommand)
        # get file lists
        imgList = []
        meshList = []
        for file in os.listdir(outDir):
            if ".nrrd" in file:
                imgList.append(outDir + file)
            if left_suffix +"." + mesh_extension in file or right_suffix +"." + mesh_extension in file:
                meshList.append(outDir + file)
    return imgList, meshList

# rasterization for meshes to DT
def MeshesToVolumes(parentDir, imgList, meshList, img_suffix, mesh_suffix, mesh_extension):
    imgList = []
    segList= []
    outImg = parentDir + "images/"
    if not os.path.exists(outImg):
        os.mkdir(outImg)
    outSeg = parentDir + "segmentations/"
    if not os.path.exists(outSeg):
        os.mkdir(outSeg)
    for file in meshList:
        if mesh_suffix in file:
            sptSeg = file.rsplit('/', 1)
            inputDir = sptSeg[0] + '/'
            file= sptSeg[1]
            subject_id = file.split(mesh_suffix)[0]
            meshFilename = inputDir + file
            # copy image over
            imgFilename = inputDir + subject_id + mesh_suffix+ "_"+ img_suffix + ".nrrd"
            imgFilename_out = outImg + subject_id + mesh_suffix+ "_"+ img_suffix + ".nrrd"
            shutil.copy(imgFilename, imgFilename_out)
            imgList.append(imgFilename_out)

            print("########### Turning  Mesh To Volume ##############")
            cprint(("Input Mesh : ", meshFilename), 'cyan')
            cprint(("Output Volume : ", outSeg + file[:-4] + ".nrrd"), 'yellow')
            print("##################################################")
            # make ply if neccesary
            if mesh_extension == "vtk":
                meshfilename_vtk = meshFilename
                meshFilename = meshFilename[:-4] + "ply"
                execCommand = ["vtk2ply", meshfilename_vtk, meshFilename]
                subprocess.check_call(execCommand)

            # write origin, size, and spacing info to text file
            infoPrefix = outSeg + subject_id + mesh_suffix + "_" + img_suffix
            execCommand = ["WriteImageInfoToText","--inFilename",imgFilename, "--outPrefix", infoPrefix]
            subprocess.check_call(execCommand)

            # get origin, size, and spacing data
            data ={}
            origin_file = open(infoPrefix + "_origin.txt", "r")
            text = origin_file.read()
            data["origin"] = text.split("\n")
            origin_file.close()
            size_file = open(infoPrefix + "_size.txt", "r")
            text = size_file.read()
            data["size"] = text.split("\n")
            size_file.close()
            spacing_file = open(infoPrefix + "_spacing.txt", "r")
            text = spacing_file.read()
            spacingX = text.split("\n")[0]
            data["spacing"] = text.split("\n")
            spacing_file.close()

            shutil.copy(meshFilename, meshFilename[:-4] + "_cp.ply")

            # write xml file
            xmlfilename=infoPrefix + "_GenerateBinaryAndDT.xml"
            if os.path.exists(xmlfilename):
                os.remove(xmlfilename)
            xml = open(xmlfilename, "a")
            xml.write("<?xml version=\"1.0\" ?>\n")
            xml.write("<mesh>\n")
            xml.write(meshFilename+"\n")
            xml.write("</mesh>\n")
            # write origin, size, and spacing data
            for key,value in data.items():
                index = 0
                for dim in ["x","y","z"]:
                    xml.write("<" + key + "_" + dim + ">" + str(value[index]) + "</" + key + "_" + dim + ">\n")
                    index += 1
            xml.close()

            # call generate binary and DT
            execCommand = ["GenerateBinaryAndDTImagesFromMeshes", xmlfilename]
            subprocess.check_call(execCommand)

            # save output volume
            output_volume = meshFilename[:-4] + ".rasterized_sp" + str(spacingX) + ".nrrd"
            segFile = outSeg + subject_id + mesh_suffix + ".nrrd"
            shutil.move(output_volume, segFile)
            segList.append(segFile)

            #save output DT
            output_DT =  meshFilename[:-4] + ".DT_sp" + str(spacingX) + ".nrrd"
            dtFile = outSeg + subject_id + mesh_suffix + "_DT.nrrd"
            shutil.move(output_DT, dtFile)
                    
    return [imgList, segList]

def ClipBinaryVolumes(parentDir, rigidFiles_segmentations, cutting_plane_points):
    outDir = os.path.join(parentDir, 'clipped_segmentations/')
    if not os.path.exists(outDir):
        os.makedirs(outDir)
    outDataListSeg = []
    for i in range(len(rigidFiles_segmentations)):
        innameSeg = rigidFiles_segmentations[i]
        if (".nrrd") in innameSeg:
            innameSeg = rigidFiles_segmentations[i]
            sptSeg = innameSeg.rsplit('/', 1)
            initPath = sptSeg[0] + '/'
            filename = sptSeg[1]
            outnameSeg = innameSeg.replace(initPath, outDir)
            outnameSeg = outnameSeg.replace('.nrrd', '.clipped.nrrd')
            outDataListSeg.append(outnameSeg)
            print(" ")
            print("############## Clipping ##############")
            cprint(("Input Segmentation Filename : ", innameSeg), 'cyan')
            cprint(("Output Segmentation Filename : ", outnameSeg), 'yellow')
            print("######################################")
            print(" ")
            # write xml file
            xmlfilename= outnameSeg[:-5] + ".xml"
            if os.path.exists(xmlfilename):
                os.remove(xmlfilename)
            xml = open(xmlfilename, "a")
            xml.write("<?xml version=\"1.0\" ?>\n")
            xml.write("<num_shapes>1</num_shapes>\n")
            xml.write("<inputs>\n")
            xml.write(innameSeg+"\n")
            xml.write("</inputs>\n")
            xml.write("<outputs>\n")
            xml.write(outnameSeg+"\n")
            xml.write("</outputs>\n")
            points = str(cutting_plane_points)[1:-1].replace(",","")
            xml.write("<cutting_planes> " + points +" </cutting_planes>")
            xml.close()
            execCommand = ["ClipVolume", xmlfilename]
            subprocess.check_call(execCommand)

    return outDataListSeg


def SelectCuttingPlane(input_file):
    ## Get vtk format
    file_format = input_file.split(".")[-1]
    input_vtk = input_file.replace(file_format, "vtk")
    if file_format == "nrrd":
        print("\nCreating mesh from: " + input_file)
        print("\nSaving as: " + input_vtk)
        xml_filename = input_file.rsplit(os.sep, 1)[0] + "/cutting_plane_nrrd2vtk.xml"
        create_meshfromDT_xml(xml_filename, input_file, input_vtk)
        execCommand = ["NewMeshFromDT", xml_filename]
        subprocess.check_call(execCommand)
    elif file_format == "ply":
        execCommand = ["ply2vtk", input_file, input_vtk]
        subprocess.check_call(execCommand)
    elif file_format == "vtk":
        pass
    else:
        print("Error, file format unrecognized: " + input_file)

    ## VTK interactive window
    print('\n Use the interactive window to select your cutting plane. When you are content with your selection, simply close the window. \n')
    # read data from file
     # read data from file
    reader = vtk.vtkPolyDataReader()
    reader.SetFileName(input_vtk)
    reader.ReadAllVectorsOn()
    reader.ReadAllScalarsOn()
    reader.Update()
    # get data
    data = reader.GetOutput()
    (xmin, xmax, ymin, ymax, zmin, zmax) = data.GetBounds()
    (xcenter, ycenter, zcenter) = data.GetCenter()
    #create mapper 
    mapper = vtk.vtkPolyDataMapper()
    mapper.SetInputData(data)
    # The actor is a grouping mechanism
    actor = vtk.vtkActor()
    actor.SetMapper(mapper)
    # create camera 
    camera = vtk.vtkCamera()
    camera.SetFocalPoint(xcenter, ycenter, zcenter)
    camera.SetPosition(100, -300, -50)
    camera.SetViewUp(0,0,1)
    # create a renderer
    renderer = vtk.vtkRenderer()
    renderer.SetActiveCamera(camera);
    renderer.SetBackground(0.2, 0.2, 0.5)
    renderer.SetBackground2(0.4, 0.4, 1.0)
    renderer.SetGradientBackground(True)
    renderer.AddActor(actor)
    # create a render_window
    render_window = vtk.vtkRenderWindow()
    render_window.AddRenderer(renderer)
    render_window.SetSize(1000,1000)
    # create a renderwindowiren
    iren = vtk.vtkRenderWindowInteractor()
    iren.SetRenderWindow(render_window)
    iren.Initialize()
    rep = vtk.vtkImplicitPlaneRepresentation()
    rep.SetPlaceFactor(1.25)
    rep.PlaceWidget(actor.GetBounds())
    rep.SetNormal(0,0,1)
    # Create a vtkImagePlaneWidget and activate it
    plane_widget = vtk.vtkImplicitPlaneWidget2()
    plane_widget.SetInteractor(iren)
    plane_widget.SetRepresentation(rep)
    plane_widget.On()
    iren.Initialize()
    iren.Start()

    # use orgin as one point and use normla to solve for two others
    (o1,o2,o3) = rep.GetOrigin()
    (n1,n2,n3) = rep.GetNormal()
    # using x = 1 and y =-1 solve for z
    pt1_z = (-n1+(n1*o1)+n2+(n2*o2)+(n3*o3))/n3
    # using x = -1 and y = 1 solve for z
    pt2_z = (n1+(n1*o1)-n2+(n2*o2)+(n3*o3))/n3
    return np.array([[o1, o2, o3], [1, -1, pt1_z], [-1, 1, pt2_z]])