# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Statistical Shape Modeling with ShapeWorks
====================================================================

First import the necessary modules
"""

import numpy as np
# import matplotlib.pyplot as plt
from zipfile import ZipFile
import os
import csv
import argparse
import glob

from GroomUtils import *
from OptimizeUtils import *
from AnalyzeUtils import *

parser = argparse.ArgumentParser(description='Example ShapeWorks Pipeline')
parser.add_argument("--interactive", help="Run in interactive mode", action="store_true")
parser.add_argument("--start_with_prepped_data", help="Start with already prepped data", action="store_true")
parser.add_argument("--start_with_image_and_segmentation_data", help = "use images and segmentations data for preprocessing", action="store_true")
parser.add_argument("--use_single_scale", help="Single scale or multi scale optimization", action="store_true")
args = parser.parse_args()


"""
Most of the following steps even though wrapped in python functions are using
the underlying c++ code, for which we need to call the source paths to the
binaries. This step should be common for any use of a function in ShapeWorks.
__This requires the full ShapeWorks to be correctly built/downloaded!__'

These following commands set the temporary environment variables to point to
shapeworks binaries and set the necessary library paths
"""

binpath = "../build/shapeworks-build/binary"
installpath = "../install"
os.environ["PATH"] = binpath + ":" + os.environ["PATH"]
os.environ["LD_LIBRARY_PATH"]= installpath + "/lib:" + installpath + "/lib64"


filename="femurs234.zip"

if args.start_with_image_and_segmentation_data:
    parentDir="Test_femur/"

    if not os.path.exists(parentDir):
            os.makedirs(parentDir)
    # extract the zipfile
    with ZipFile(filename, 'r') as zipObj:
        zipObj.extractall(path=parentDir)

    if not args.start_with_prepped_data:
            print("\nStep 1. Extract Data\n")
            if args.interactive:
                    input("Press Enter to continue")
            """
            ## GROOM : Data Pre-processing
            The steps are:
            -- Reflect right femurs*
            -- Decimate meshes*
            -- Smooth meshes?*
            -- Meshes to volumes*
            -- Threhold DT*
            -- Remove leakage
            -- Isotropic resampling
            -- Padding
            -- Center of Mass Alignment
            -- Rigid Alignment
            -- Clip binary volumes*
            -- Largets Bounding Box and Cropping
            -- Groom and extract meshes*
            """

            prepDir = './Test_femur/'

            print("\nStep 2. Groom - Data Pre-processing\n")
            if args.interactive:
                    input("Press Enter to continue")

            # set name specific variables
            img_suffix = "1x_hip"
            mesh_suffix = "femur"
            left_suffix = "L_femur"
            right_suffix = "R_femur"
            mesh_extension = "ply"

            # @TODO manually select cutting plane
            cp_x1=81.53 
            cp_y1=79 
            cp_z1=71.38 
            cp_x2=104.87 
            cp_y2=98.94 
            cp_z2=71.38
            cp_x3=78.92 
            cp_y3=88.12 
            cp_z3=71.38 

            
            # """
            # We have left and right femurs, so we will reflect the image if neccesary
            # so that we have an image for every mesh
            # """
            # reference_side = "left"
            # [fileList_img, fileList_mesh] = anatomyPairsToSingles(parentDir, img_suffix, left_suffix, right_suffix, mesh_extension, reference_side)

            # """
            # convert meshes to binary segmentations and distance transform
            # """
            # [fileList_imgL, fileList_segL] = MeshesToVolumes(parentDir, fileList_img, fileList_Mesh, img_suffix, left_suffix, mesh_extension)
            # [fileList_imgR, fileList_segR] = MeshesToVolumes(parentDir, fileList_img, fileList_Mesh, img_suffix, right_suffix, mesh_extension)
            # fileList_img = fileList_imgL + fileList_imgR
            # fileList_seg = fileList_segL + fileList_segR

            # """ps
            # Apply isotropic resampling

            # For detailed explainations of parameters for resampling volumes, go to
            # ... link

            # the segmentation and images are resampled independently and the result files are saved in two different directories.
            # """

            # [resampledFiles_segmentations, resampledFiles_images] = applyIsotropicResampling(parentDir, fileList_seg ,fileList_img, 1, processRaw = True)

            # """
            # Apply padding

            # For detailed explainations of parameters for padding volumes, go to
            # ... link

            # Both the segmentation and raw images are padded.
            # """

            # [paddedFiles_segmentations,  paddedFiles_images] = applyPadding(parentDir, resampledFiles_segmentations,resampledFiles_images, 10, processRaw = True)


            # """
            # Apply center of mass alignment

            # For detailed explainations of parameters for center of mass(COM) alignment of volumes, go to
            # ... link

            # This function can handle both cases(processing only segmentation data or raw and segmentation data at the same time).
            # There is parameter that you can change to switch between cases. processRaw = True, processes raw and binary images with shared parameters.
            # """
            # [comFiles_segmentations, comFiles_images] = applyCOMAlignment( parentDir, paddedFiles_segmentations, paddedFiles_images , processRaw=True)

            # """
            # Apply rigid alignment

            # For detailed explainations of parameters for rigid alignment of volumes, go to
            # ... link

            # This function can handle both cases(processing only segmentation data or raw and segmentation data at the same time).
            # There is parameter that you can change to switch between cases. processRaw = True, processes raw and binary images with shared parameters.
            # processRaw = False, applies the center of mass alignment only on segemnattion data.
            # This function uses the same transfrmation matrix for alignment of raw and segmentation files.
            # Rigid alignment needs a reference file to align all the input files, FindMedianImage function defines the median file as the reference.
            # """
            # medianFile = FindMedianImage(comFiles_segmentations)

            # [rigidFiles_segmentations, rigidFiles_images] = applyRigidAlignment(parentDir, comFiles_segmentations, comFiles_images , medianFile, processRaw = True)
            rigidFiles_segmentations = []
            for file in os.listdir(parentDir + 'aligned/segmentations/'):
                rigidFiles_segmentations.append(parentDir + 'aligned/segmentations/' + file)
            rigidFiles_images = []
            for file in os.listdir(parentDir + 'aligned/images/'):
                rigidFiles_images.append(parentDir + 'aligned/images/' + file)
            print(rigidFiles_segmentations)
            input(rigidFiles_images)

            # TODO - implement crop volumes
            clippedFiles_segmentations = ClipBinaryVolumes(parentDir, rigidFiles_segmentations, cp_x1, cp_x2, cp_x3, cp_y1, cp_y2, cp_y3, cp_z1, cp_z2, cp_z3)

            """
            Compute largest bounding box and apply cropping
            processRaw = True, processes raw and binary images with shared parameters.
            processRaw = False, applies the center of mass alignment only on segemnattion data.
            The function uses the same bounding box to crop the raw and segemnattion data.
            """
            [croppedFiles_segmentations, croppedFiles_images] = applyCropping(parentDir, clippedFiles_segmentations,  rigidFiles_images, processRaw=True)
