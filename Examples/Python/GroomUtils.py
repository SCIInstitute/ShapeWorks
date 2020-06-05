import sys
import numpy as np
import io
from termcolor import colored, cprint
import glob
import os
import subprocess
import shutil
import xml.etree.ElementTree as ET
import itk
import vtk
import vtk.util.numpy_support

from CommonUtils import *

def rename(inname, outDir, extension_addition, extension_change=''):
    """
    Takes inname path and replaces dir with outdir and adds extension before file type
    """
    initPath = os.path.dirname(inname)
    outname = inname.replace(initPath, outDir)
    current_extension = "." + inname.split(".")[-1]
    if extension_addition != '':
        outname = outname.replace(current_extension, '.' + extension_addition + current_extension)
    if extension_change != '':
        outname = outname.replace(current_extension, extension_change)
    cprint(("Input Filename : ", inname), 'cyan')
    cprint(("Output Filename : ", outname), 'yellow')
    print("######################################\n")
    return outname

def applyIsotropicResampling(outDir, inDataList, isoSpacing=1.0, isBinary=True):
    """
    This function takes in a filelist and produces the resampled files in the appropriate directory.
    """
    print("\n########### Resampling ###############")
    if not os.path.exists(outDir):
        os.makedirs(outDir)
    outDataList = []
    for i in range(len(inDataList)):
        inname = inDataList[i]
        outname = rename(inname, outDir, 'isores')
        outDataList.append(outname)
        cmd = ["shapeworks", 
               "read-image", "--name", inname]
        if isBinary:
            cmd.extend(["antialias"])
        cmd.extend(["isoresample", "--isospacing", str(isoSpacing)])  
        if isBinary:
            cmd.extend(["binarize"])
        cmd.extend(["write-image", "--name", outname])
        subprocess.check_call(cmd)
    return outDataList

def getOrigin(inname):
    infoPrefix = "_".join(inname.split("_")[:3])
    cmd = ["WriteImageInfoToText","--inFilename",inname, "--outPrefix", infoPrefix]
    subprocess.check_call(cmd)
    origin_file = open(infoPrefix + "_origin.txt", "r")
    text = origin_file.read()
    origin = text.split("\n")
    origin_file.close()
    os.remove(infoPrefix + "_origin.txt")
    os.remove(infoPrefix + "_spacing.txt")
    os.remove(infoPrefix + "_size.txt")
    return origin

def center(outDir, inDataList):
    if not os.path.exists(outDir):
        os.makedirs(outDir)
    outDataList = []
    for i in range(len(inDataList)):
        # center
        inname = inDataList[i]
        print("\n########### Centering ###############")
        outname = rename(inname, outDir, 'center')
        outDataList.append(outname)
        cmd = ["shapeworks", "read-image", "--name", inname]
        cmd.extend(["recenter-image"])
        cmd.extend(["write-image", "--name", outname])
        print("Calling cmd:\n"+" ".join(cmd))
        subprocess.check_call(cmd)
        # Get translation
        original_origin = getOrigin(inname)
        new_origin = getOrigin(outname)
        translation = []
        for dim in range(0,3):
            translation.append(float(original_origin[dim]) - float(new_origin[dim]))
        # Write translation
        translation_file = outname[:-4] + "translation.txt"
        out_trans = open(translation_file, "w+")
        out_trans.write(str(translation).replace('[','').replace(']','').replace(',',''))
        out_trans.close()
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
        cmd = ["shapeworks", "read-image", "--name", inname]
        cmd.extend(["pad" , "--padding" , str(padSize) , "--value" , str(padValue)])
        cmd.extend(["write-image", "--name", outname])
        print("Calling cmd:\n"+" ".join(cmd))
        subprocess.check_call(cmd)
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
        rawoutDir = os.path.join(outDir, 'images')
        if not os.path.exists(rawoutDir):
            os.makedirs(rawoutDir)
        binaryoutDir = os.path.join(outDir, 'segmentations')
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
    x = y = z = 0
    for i in range(len(inDataList)):
        dim = itk.GetArrayFromImage(itk.imread(inDataList[i])).shape
        if dim[0] > x:
            x = dim[0]
        if dim[1] > y:
            y = dim[1]
        if dim[2] > z:
            z = dim[2]

    COM = np.zeros((x, y, z))
    for i in range(len(inDataList)):
        tmp = itk.GetArrayFromImage(itk.imread(inDataList[i]))
        COM += np.pad(tmp, (((x - tmp.shape[0]) // 2, (x - tmp.shape[0]) - (x - tmp.shape[0]) // 2),
                            ((y - tmp.shape[1]) // 2, (y - tmp.shape[1]) - (y - tmp.shape[1]) // 2),
                            ((z - tmp.shape[2]) // 2, (z - tmp.shape[2]) - (z - tmp.shape[2]) // 2)))
    COM /= len(inDataList)
    dist = np.inf
    idx = 0
    for i in range(len(inDataList)):
        tmp = itk.GetArrayFromImage(itk.imread(inDataList[i]))
        tmp_dist = np.linalg.norm(
            COM - np.pad(tmp, (((x - tmp.shape[0]) // 2, (x - tmp.shape[0]) - (x - tmp.shape[0]) // 2),
                               ((y - tmp.shape[1]) // 2, (y - tmp.shape[1]) - (y - tmp.shape[1]) // 2),
                               ((z - tmp.shape[2]) // 2, (z - tmp.shape[2]) - (z - tmp.shape[2]) // 2))))
        if tmp_dist < dist:
            idx = i
            dist = tmp_dist

    print(" ")
    print("############# Reference File #############")
    cprint(("The reference file for rigid alignment is found"), 'cyan')
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
    outDir = os.path.join(parentDir, 'aligned')
    transoutDir = os.path.join(outDir, 'transformations')

    if not os.path.exists(outDir):
        os.makedirs(outDir)
    if not os.path.exists(transoutDir):
        os.makedirs(transoutDir)

    # identify the reference scan
    refDir = os.path.join(outDir, 'reference')
    if not os.path.exists(refDir):
        os.makedirs(refDir)
    initPath = os.path.dirname(refFile)
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
    execCommand = ["shapeworks", "read-image", "--name", refFile, "antialias", "--numiterations", str(antialiasIterations), "write-image", "--name", ref_dtnrrdfilename]
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
        rawoutDir = os.path.join(outDir, 'images')
        binaryoutDir = os.path.join(outDir, 'segmentations')

        if not os.path.exists(rawoutDir):
            os.makedirs(rawoutDir)

        if not os.path.exists(binaryoutDir):
            os.makedirs(binaryoutDir)

        outRawDataList=[]
        outSegDataList=[]
        for i in range(len(inDataListSeg)):
            seginname = inDataListSeg[i]
            initPath = os.path.dirname(seginname)
            filename = os.path.basename(seginname)
            segoutname = seginname.replace(initPath, binaryoutDir)
            segoutname = segoutname.replace('.nrrd', '.aligned.nrrd')
            transoutname = seginname.replace(initPath, transoutDir)
            transformation = transoutname.replace('.nrrd', '.transformationMatrix.txt')
            outSegDataList.append(segoutname)

            rawinname = inDataListImg[i]
            initPath = os.path.dirname(rawinname)
            filename = os.path.basename(rawinname)
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
            execCommand = ["shapeworks", "read-image", "--name", seginname, "antialias", "--numiterations", str(antialiasIterations), "write-image", "--name", dtnrrdfilename]
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
            initPath = os.path.dirname(inname)
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
            execCommand = ["shapeworks", "read-image", "--name", inname, "antialias", "--numiterations", str(antialiasIterations), "write-image", "--name", dtnrrdfilename]
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
    outDir = os.path.join(parentDir, 'cropped')
    if not os.path.exists(outDir):
        os.makedirs(outDir)
    cropinfoDir = os.path.join(outDir, 'crop_info')
    if not os.path.exists(cropinfoDir):
        os.makedirs(cropinfoDir)
    # first create a txtfile with all the scan names in it.
    txtfile = os.path.join(cropinfoDir, "_dataList.txt")
    with open(txtfile, 'w') as filehandle:
        for listitem in inDataListSeg:
            filehandle.write('%s\n' % listitem)
    outPrefix = os.path.join(cropinfoDir, "largest_bounding_box")
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
        rawoutDir = os.path.join(outDir, 'images')
        binaryoutDir = os.path.join(outDir, 'segmentations')
        if not os.path.exists(rawoutDir):
            os.makedirs(rawoutDir)
        if not os.path.exists(binaryoutDir):
            os.makedirs(binaryoutDir)
        outDataListSeg = []
        outDataListImg = []
        for i in range(len(inDataListSeg)):
            innameSeg = inDataListSeg[i]
            innameImg = inDataListImg[i]
            initPath = os.path.dirname(innameSeg)
            outnameSeg = innameSeg.replace(initPath, binaryoutDir)
            outnameSeg = outnameSeg.replace('.nrrd', '.cropped.nrrd')
            outDataListSeg.append(outnameSeg)
            initPath = os.path.dirname(innameImg)
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
            initPath = os.path.dirname(inname)
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
    outDir = os.path.join(parentDir, 'groom_and_meshes')
    if not os.path.exists(outDir):
        os.makedirs(outDir)

    finalDTDir = os.path.join(parentDir, 'distance_transforms')
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
        execCommand = ["shapeworks", "read-image", "--name", inname, "antialias", "--numiterations", str(antialiasIterations), "write-image", "--name", dtnrrdfilename]
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
def anatomyPairsToSingles(outDir, seg_list, img_list, reference_side):
    if not os.path.exists(outDir):
        os.makedirs(outDir)
    outSegDir = os.path.join(outDir, "segmentations")
    if not os.path.exists(outSegDir):
        os.mkdir(outSegDir)
    outImgDir = os.path.join(outDir, "images")
    if not os.path.exists(outImgDir):
        os.mkdir(outImgDir)
    imageList = []
    meshList = []
    for img in img_list:
        img_name = os.path.basename(img)
        prefix = img_name.split("_")[0]
        if reference_side == 'right':
            ref = 'R'
            flip = 'L'
        elif reference_side == 'left':
            ref = 'L'
            flip = 'R'
        else:
            print("Error: reference side must be 'left' or 'right'.")
        # check if ref exists
        ref_prefix = prefix + "_" + ref
        flip_prefix = prefix + "_" + flip
        ref_seg = 'None'
        flip_seg = 'None'
        for seg in seg_list:
            if ref_prefix in seg:
                ref_seg = seg
            elif flip_prefix in seg:
                flip_seg = seg
        # if we have ref seg, copy image and seg over with appropriate name
        if ref_seg != 'None':
            seg_out = ref_seg.replace(os.path.dirname(ref_seg), outSegDir)
            meshList.append(seg_out)
            shutil.copy(ref_seg, seg_out)
            img_out = img.replace(os.path.dirname(img), outImgDir)
            img_out = img_out.replace(prefix, ref_prefix)
            imageList.append(img_out)
            shutil.copy(img, img_out)
        # if we have a seg for the non-ref side, reflect it
        if flip_seg != 'None':
            print("\n############## Reflecting ###############")
            img_out = rename(img, outImgDir, 'reflect').replace(prefix, flip_prefix)
            imageList.append(img_out)
            centerFilename = os.path.join(outDir, prefix + "_origin.txt")
            execCommand = ["ReflectVolumes", "--inFilename", img, "--outFilename", img_out, "--centerFilename", centerFilename, "--inputDirection", "0"]
            subprocess.check_call(execCommand)
            print("\n############## Reflecting ###############")
            seg_out = rename(flip_seg, outSegDir, 'reflect')
            meshList.append(seg_out)
            execCommand = ["ReflectMesh", "--inFilename", flip_seg, "--outFilename", seg_out, "--reflectCenterFilename", centerFilename, "--inputDirection", "0", "--meshFormat", flip_seg.split(".")[-1]]
            subprocess.check_call(execCommand)        
    return meshList, imageList


# rasterization for meshes to DT
def MeshesToVolumes(outDir, meshList, imgList):
    segList= []
    if not os.path.exists(outDir):
        os.mkdir(outDir)
    for mesh in meshList:
        mesh_name = os.path.basename(mesh)
        extension = mesh_name.split(".")[-1]
        prefix = mesh_name.split("_")[0] + "_" + mesh_name.split("_")[1]
        # change to ply if needed
        if extension == "vtk":
            mesh_vtk = mesh
            mesh = mesh[:-4] + ".ply"
            execCommand = ["vtk2ply", mesh_vtk, mesh]
            subprocess.check_call(execCommand)
        if extension == "stl":
            mesh_vtk = mesh
            mesh = mesh[:-4] + ".ply"
            execCommand = ["stl2ply", mesh_vtk, mesh]
            subprocess.check_call(execCommand) 
        # get image
        for image_file in imgList:
            if prefix in image_file:
                image = image_file
         # write origin, size, and spacing info to text file
        infoPrefix = os.path.join(outDir, prefix)
        execCommand = ["WriteImageInfoToText","--inFilename",image, "--outPrefix", infoPrefix]
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
        # write xml file
        xmlfilename=infoPrefix + "_GenerateBinaryAndDT.xml"
        if os.path.exists(xmlfilename):
            os.remove(xmlfilename)
        xml = open(xmlfilename, "a")
        xml.write("<?xml version=\"1.0\" ?>\n")
        xml.write("<mesh>\n")
        xml.write(mesh+"\n")
        xml.write("</mesh>\n")
        # write origin, size, and spacing data
        for key,value in data.items():
            index = 0
            for dim in ["x","y","z"]:
                xml.write("<" + key + "_" + dim + ">" + str(value[index]) + "</" + key + "_" + dim + ">\n")
                index += 1
        xml.close()
        print("########### Turning  Mesh To Volume ##############")
        segFile = rename(mesh, outDir, "", ".nrrd")
        # call generate binary and DT
        execCommand = ["GenerateBinaryAndDTImagesFromMeshes", xmlfilename]
        subprocess.check_call(execCommand)
         # save output volume
        output_volume = mesh.replace(".ply", ".rasterized_sp" + str(spacingX) + ".nrrd")
        shutil.move(output_volume, segFile)
        segList.append(segFile)
        #save output DT
        output_DT =  mesh.replace(".ply", ".DT_sp" + str(spacingX) + ".nrrd")
        dtFile = segFile.replace(".nrrd", "_DT.nrrd")
        shutil.move(output_DT, dtFile)                    
    return segList


def ClipBinaryVolumes(outDir, segList, cutting_plane_points):
    if not os.path.exists(outDir):
        os.makedirs(outDir)
    outListSeg = []
    for seg in segList:
        print("\n############## Clipping ##############")
        seg_out = rename(seg, outDir, "clipped")
        outListSeg.append(seg_out)
        # write xml file
        xmlfilename= seg_out.replace(".nrrd",".xml")
        if os.path.exists(xmlfilename):
            os.remove(xmlfilename)
        xml = open(xmlfilename, "a")
        xml.write("<?xml version=\"1.0\" ?>\n")
        xml.write("<num_shapes>1</num_shapes>\n")
        xml.write("<inputs>\n")
        xml.write(seg+"\n")
        xml.write("</inputs>\n")
        xml.write("<outputs>\n")
        xml.write(seg_out+"\n")
        xml.write("</outputs>\n")
        points = str(cutting_plane_points)[1:-1].replace(",","")
        xml.write("<cutting_planes> " + points +" </cutting_planes>")
        xml.close()
        execCommand = ["ClipVolume", xmlfilename]
        subprocess.check_call(execCommand)
    return outListSeg

def SelectCuttingPlane(input_file):
    ## Get vtk format
    file_format = input_file.split(".")[-1]
    input_vtk = input_file.replace(file_format, "vtk")
    if file_format == "nrrd":
        print("\nCreating mesh from: " + input_file)
        print("\nSaving as: " + input_vtk)
        xml_filename = os.path.join(os.path.dirname(input_file), "cutting_plane_nrrd2vtk.xml")
        create_meshfromDT_xml(xml_filename, input_file, input_vtk)
        execCommand = ["MeshFromDistanceTransforms", xml_filename]
        subprocess.check_call(execCommand)
        print("Calling cmd:\n"+" ".join(execCommand))
    elif file_format == "ply":
        execCommand = ["ply2vtk", input_file, input_vtk]
        subprocess.check_call(execCommand)
        print("Calling cmd:\n"+" ".join(execCommand))
    elif file_format == "stl":
        execCommand = ["stl2vtk", input_file, input_vtk]
        subprocess.check_call(execCommand)
        print("Calling cmd:\n"+" ".join(execCommand))
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
    # fix 0 edge case
    if o1 == 0 and o2 == 0:
        o1 = -1
        o2 = -1
    return np.array([[o1, o2, o3], [1, -1, pt1_z], [-1, 1, pt2_z]])
