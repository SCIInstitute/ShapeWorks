import numpy as np
import io
from termcolor import colored, cprint
import glob
import os
import shutil
import xml.etree.ElementTree as ET
import SimpleITK as sitk

from CommonUtils import *

def applyIsotropicResampling(inDataListImg, isoSpacing, isCenterOn=True, isBinaryImage=False):
    """
    Authors: Atefeh Ghanaatikashani
    Date: 1st October 2019

    This function takes in a filelist of raw images if CINE time points and produces the resampled files in the
    appropriate directory.

    """


    #process images files

    outDataListImg = []
    for i in range(len(inDataListImg)):
        inname = inDataListImg[i]
        outname = inname
        outname = outname.replace('.nrrd', '.isores.nrrd')
        outDataListImg.append(outname)
        print(" ")
        print("########### Resampling ###############")
        cprint(("Input Filename : ", inname), 'cyan')
        cprint(("Output Filename : ", outname), 'yellow')
        print("######################################")
        print(" ")
        if isBinaryImage:
            if isCenterOn:
                execCommand = "ResampleVolumesToBeIsotropic --inFilename " + inname + " --outFilename " + outname + " --isoSpacing " + str(
                    isoSpacing) + " --isCenterImageOn 1 --isBinaryImage 1"
                os.system(execCommand)
            else:
                execCommand = "ResampleVolumesToBeIsotropic --inFilename " + inname + " --outFilename " + outname + " --isoSpacing " + str(
                    isoSpacing) + " --isCenterImageOn 0 --isBinaryImage 1"
                os.system(execCommand)
        else:
            if isCenterOn:
                execCommand = "ResampleVolumesToBeIsotropic --inFilename " + inname + " --outFilename " + outname + " --isoSpacing " + str(
                    isoSpacing) + " --isCenterImageOn 1"
                os.system(execCommand)
            else:
                execCommand = "ResampleVolumesToBeIsotropic --inFilename " + inname + " --outFilename " + outname + " --isoSpacing " + str(
                    isoSpacing) + " --isCenterImageOn 0"
                os.system(execCommand)

    return outDataListImg



def applyPadding(inDataListImg, padSize = 15, padValue=0):
    """
    Authors: Atefeh Ghanaatikashani
    Date: 1st October 2019

    This function takes in a filelist of raw CINE Time points and produces the padded files in the
    appropriate directory.

    """
    outDataListImg = []
    for i in range(len(inDataListImg)):
        inname = inDataListImg[i]
        outname = inname
        outname = outname.replace('.nrrd', '.pad.nrrd')
        outDataListImg.append(outname)
        print(" ")
        print("############## Padding ###############")
        cprint(("Input Filename : ", inname), 'cyan')
        cprint(("Output Filename : ", outname), 'yellow')
        print("######################################")
        print(" ")
        execCommand = "PadVolumeWithConstant --inFilename " + inname + " --outFilename " + outname + " --paddingSize " + str(
            padSize) + " --paddingValue " + str(padValue)
        os.system(execCommand)

    return outDataListImg



def applyCOMAlignment(inDataListImg, translation):
    """
    Authors: Atefeh Ghanaatikashani
    Date: 1st October 2019

    This function takes in a filelist of CINE time points and translation of the LGE  and produces the translated CINE images
    with same translation
    Input Parameters:
    Output Parameters:
    """

    outDataListImg = []
    for i in range(len(inDataListImg)):
        innameImg = inDataListImg[i]
        outnameImg = innameImg
        outnameImg = outnameImg.replace('.nrrd', '.com.nrrd')
        outDataListImg.append(outnameImg)
        print(" ")
        print("############# COM Alignment ###############")
        cprint(("Input Image Filename : ", innameImg), 'cyan')
        cprint(("Output Image Filename : ", outnameImg), 'yellow')
        print("###########################################")
        print(" ")
        execCommand = "TranslateImages --MRIinFilename " + innameImg + " --MRIoutFilename " + outnameImg + " --translationX " + str(translation[0])+ " --translationY " + str(translation[1])+ " --translationZ " + str(translation[2])
        os.system(execCommand)

    return outDataListImg



def applyRigidAlignment(inDataListImg, refFile, tranformation):
    """
    Authors: Atefeh Ghanaatikashani
    Date: 1st October 2019

    This function takes in a filelist of CINE time points and transformation matrix of the LGE and produces the transformed CINE images

    Input Parameters:
    Output Parameters:
    """

    outRawDataList=[]
    for i in range(len(inDataListImg)):
        rawinname = inDataListImg[i]
        rawoutname = rawinname
        rawoutname = rawoutname.replace('.nrrd', '.aligned.nrrd')
        outRawDataList.append(rawoutname)

        print(" ")
        print("############# Rigid Alignment #############")
        cprint(("Input Reference Filename : ", refFile), 'cyan')
        cprint(("Input Raw Filename : ", rawinname), 'cyan')
        cprint(("Output Raw Filename : ", rawoutname), 'yellow')
        print("###########################################")
        print(" ")
        execCommand = "MatrixRigid3DImageRegistration --targetDistanceMap " + refFile + " --sourceRaw " + rawinname + " --solutionRaw " + rawoutname + " --m_00 " + str(tranformation[0][0]) + " --m_01 " + str(tranformation[0][1])+ " --m_02 " + str(tranformation[0][2]) + " --m_10 " + str(tranformation[1][0]) + " --m_11 " + str(tranformation[1][1]) + " --m_12 " + str(tranformation[1][2]) + " --m_20 " + str(tranformation[2][0]) + " --m_21 " + str(tranformation[2][1]) + " --m_22 " + str(tranformation[2][2]) + " --m_03 " + str(tranformation[0][3])+ " --m_13 " + str(tranformation[1][3])+ " --m_23 " + str(tranformation[2][3])
        os.system(execCommand)

    return  outRawDataList



def applyCropping(inDataListImg, cropinfoDir):
    """
    Authors: Atefeh Ghanaatikashani
    Date: 1st October 2019

    This function takes in a filelist of CINE time points and crops them according to the largest
    bounding box which it discovered for LGE images
    Input Parameters:
    Output Parameters:
    """

    # read all the bounding box files for cropping
    bb0 = np.loadtxt(glob.glob(cropinfoDir + "*_bb0.txt")[0])
    bb1 = np.loadtxt(glob.glob(cropinfoDir + "*_bb1.txt")[0])
    bb2 = np.loadtxt(glob.glob(cropinfoDir + "*_bb2.txt")[0])
    smI0 = np.loadtxt(glob.glob(cropinfoDir + "*_smallestIndex0.txt")[0])
    smI1 = np.loadtxt(glob.glob(cropinfoDir + "*_smallestIndex1.txt")[0])
    smI2 = np.loadtxt(glob.glob(cropinfoDir + "*_smallestIndex2.txt")[0])


    outDataListImg = []
    for i in range(len(inDataListImg)):
        innameImg = inDataListImg[i]
        outnameImg = innameImg
        outnameImg = outnameImg.replace('.nrrd', '.cropped.nrrd')
        outDataListImg.append(outnameImg)
        print(" ")
        print("############## Cropping ##############")
        cprint(("Input Image Filename : ", innameImg), 'cyan')
        cprint(("Output Image Filename : ", outnameImg), 'yellow')
        print("######################################")
        print(" ")
        execCommand = "CropRawImages --MRIinFilename " + innameImg + " --MRIoutFilename " + outnameImg + " --bbX " + str(bb0) + " --bbY " + str(bb1) + " --bbZ " + str(bb2) + " --startingIndexX " + str(
            smI0) + " --startingIndexY " + str(smI1) + " --startingIndexZ " + str(smI2)

        os.system(execCommand)

    return outDataListImg


