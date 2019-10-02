# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Statistical Shape Modeling with ShapeWorks
====================================================================

In this example, we provide unpre-processed left atrial dataset,
which consists of the LGE scan and its segmentation across some number of subjects.
Since the data set is not pre-processed, it requires all re-processing steps;
resampling, padding, alignment, etc. To run the ShapeWorks particle-based optimization,
the processed segmentation files are converted to the signed distance transform.
This example is set to serve as a test case for users who want to process the raw(gray-sclae)
images as well as their binary segmentation images.
In this example, the raw and segmentation data are sharing the same functions for pre-processing,
such as the same transformation matrices for the center of mass and rigid alignment or same bounding box for cropping.


First import the necessary modules
"""

import numpy as np
import matplotlib.pyplot as plt
from zipfile import ZipFile
import os
import csv
import argparse
import glob
import time
from CINEGroomUtils import *

"""
## GROOM : Data Pre-processing
For the unprepped data the first few steps are
-- Isotropic resampling
-- Padding
-- Center of Mass Alignment
-- Rigid Alignment
-- Largest Bounding Box and Cropping
"""


binpath = "../build/shapeworks-build/binary"
installpath = "../install"
os.environ["PATH"] = binpath + ":" + os.environ["PATH"]
os.environ["LD_LIBRARY_PATH"]= installpath + "/lib:" + installpath + "/lib64"


start = time.time()

PrepOutputDir = "/usr/sci/projects/Cardiac4DShape/DATA/Human_Data_Processed/Human_Data_Particles/PrepOutput/"



parentDir="/usr/sci/projects/Cardiac4DShape/DATA/Human_Data_Processed/"

excludeDirs = ['logs', 'Human_Data_Particles', 'LAFGH_P24','BORDJD49_PRE','LAFGH_P14','BORDBL57_POST','BORDJD57_POST','BORDCG56_POST']

subjectList = os.listdir(parentDir)

for subject in subjectList:
    if subject not in excludeDirs:
        # get the list of data for the grooming
        dir = parentDir + subject +'/'
        fileList = [dir+'mra.isores.r-aligned.crop.nrrd']
        for folder in os.listdir(dir):
            if folder.upper() == 'CINE':
                cineDir = dir + 'CINE/'
                for time in os.listdir(cineDir):
                    if time.endswith('isores.crop.nrrd'):
                        fileList.append(cineDir+time)



        resampledFiles = applyIsotropicResampling(fileList, 1)

        paddedFiles = applyPadding(resampledFiles)

        # read the translation information from the subjects txt file
        comDir = PrepOutputDir + 'com_aligned/segmentations/'
        for com in os.listdir(comDir):
            if com.startswith(subject) and com.endswith('.txt'):
                text_file = open(comDir + com, "r")
                lines = text_file.readlines()
                for line in lines:
                    if line.startswith('translation'):
                        line = line.replace('translation:', '')
                        line = line.replace('\n', '')
                        line = line.split(" ")
                        COM_translation = [float(line[j]) for j in range(len(line))]


        comFiles = applyCOMAlignment(paddedFiles, COM_translation)

        # read the transfprmation matrix of rigid Alignment for the subject

        alignedTMDir = PrepOutputDir + 'aligned/transformations/'
        alignedDir = PrepOutputDir + 'aligned/segmentations/'
        transformation = []
        for matrix in os.listdir(alignedTMDir):
            if matrix.startswith(subject) and matrix.endswith('.txt'):
                text_file = open(alignedTMDir + matrix, "r")
                lines = text_file.readlines()
                for i in range(7, 10):
                    line = lines[i].split()
                    line = [float(line[j].strip()) for j in range(len(line))]
                    transformation.append(line)

        for aligned_file in os.listdir(alignedDir):
            if aligned_file.startswith(subject) and aligned_file.endswith('.aligned.nrrd'):
                refFile = alignedDir + aligned_file


        rigidFiles = applyRigidAlignment(comFiles, refFile, transformation )

        cropinfoDir = PrepOutputDir + 'cropped/crop_info/'
        croppedFiles = applyCropping(rigidFiles, cropinfoDir)

end = time.time()

print("the execution time is: " +str(end-start) + " seconds")





