# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Statistical Shape Modeling with ShapeWorks
====================================================================

In this example we provide a full pipeline with an example dataset of axis 
aligned ellipsoid meshes.
"""

from zipfile import ZipFile
import os
import sys
import csv
import argparse

from GroomUtils import *
from OptimizeUtils import *
from AnalyzeUtils import *


def dictToXML(params, filename):
    with open(filename, "w") as f:
        for key in params:
            tag = '<' + key + '>'
            closer = '</' + key + '>\n'
            f.write(tag + str(params[key]) + closer)


def Run_Pipeline(args):
    datasetName = 'femur'
    parentDir = 'TestFemurMesh/'
    prepDir = parentDir + 'PrepOutput/'
    vtkDir = prepDir + 'vtk/'
    if not os.path.exists(vtkDir):
        os.makedirs(vtkDir)
    plyDir = prepDir + 'ply/'
    if not os.path.exists(plyDir):
        os.makedirs(plyDir)
    alignedMeshesDir = prepDir + 'aligned/'
    if not os.path.exists(alignedMeshesDir):
        os.makedirs(alignedMeshesDir)


    meshFiles = sorted(glob.glob(parentDir + datasetName + "/meshes/*.ply"))
    imageFiles = sorted(glob.glob(parentDir + datasetName + "/distance_transforms/*.nrrd"))

    # Only do this when want to run on all meshes
    #reflectedFiles_mesh, reflectedFile_img = anatomyPairsToSingles('TestFemur/reflected', meshFiles, imageFiles, 'left')


    #### Step 1, Get only left side femurs
    leftMeshFiles = []
    leftImageFiles = []
    for f, f2 in zip(meshFiles, imageFiles):
        # n09_L is a bad example, it is squished or something
        if 'n09_L_femur.ply' in f:
            continue
        name = os.path.basename(f)
        side = name.split("_")[1]
        if side == 'L':
            leftMeshFiles.append(f)
            leftImageFiles.append(f2)
    print(leftMeshFiles)


    #### Step 2, Convert to .vtk
    vtkMeshFiles = []
    for meshFile in leftMeshFiles:
        basename = os.path.basename(meshFile)
        name, ext = os.path.splitext(basename)
        targetName = vtkDir + name + '.vtk'
        subprocess.check_call(['ply2vtk.exe', meshFile, targetName])   
        vtkMeshFiles.append(targetName)


    #### Step 3, align meshes
    referenceMesh = vtkMeshFiles[0]
    vtkMeshFiles = vtkMeshFiles[1:]
    alignedMeshes = []
    
    refFileName = alignedMeshesDir + os.path.basename(referenceMesh)
    shutil.copy(referenceMesh, refFileName)
    alignedMeshes.append(refFileName)
    for meshFile in vtkMeshFiles:
        name = os.path.basename(meshFile)
        targetName = alignedMeshesDir + name
        params = {
            'source_mesh': meshFile,
            'target_mesh': referenceMesh,
            'out_mesh': targetName,
            'out_transform': alignedMeshesDir + name + '.transform',
            'mode': 'affine'
        }
        dictToXML(params, 'params.txt')
        execCommand = ["ICPRigid3DMeshRegistration.exe", "params.txt"]
        subprocess.check_call(execCommand)  
        alignedMeshes.append(targetName)      


    #### Step 4, Convert back to .ply
    plyMeshFiles = []
    for meshFile in alignedMeshes:
        basename = os.path.basename(meshFile)
        name, ext = os.path.splitext(basename)
        targetName = plyDir + name + '.ply'
        subprocess.check_call(['vtk2ply.exe', meshFile, targetName])   
        plyMeshFiles.append(targetName)




    meshFiles = plyMeshFiles






    # exit()

    # meshFiles = sorted(glob.glob("TestFemur/femur/meshes2/*.ply"))
    # imageFiles = sorted(glob.glob("TestFemur/femur/distance_transforms2/*.nrrd"))


    pointDir = 'TestFemur/PointFiles/'
    if not os.path.exists(pointDir):
        os.makedirs(pointDir)

    parameterDictionary = {
        "use_normals": 0,
        "normal_weight": 0.0,
        "checkpointing_interval" : 200,
        "keep_checkpoints" : 0,
        "iterations_per_split" : 500,
        "optimization_iterations" : 500,
        "starting_regularization" : 100,
        "ending_regularization" : 0.1,
        "recompute_regularization_interval" : 2,
        "domains_per_shape" : 1,
        "domain_type" : 'mesh',
        "relative_weighting" : 10,
        "initial_relative_weighting" : 0.01,
        "procrustes_interval" : 0,
        "procrustes_scaling" : 0,
        "save_init_splits" : 0,
        "debug_projection" : 0,
        "verbosity" : 3
        }
    if int(args.use_single_scale) != 0:
        parameterDictionary["number_of_particles"] = 1024

        [localPointFiles, worldPointFiles] = runShapeWorksOptimize_SingleScale(pointDir, meshFiles, parameterDictionary)

    else:
        parameterDictionary["starting_particles"] = 32
        parameterDictionary["number_of_levels"] = 3

        [localPointFiles, worldPointFiles] = runShapeWorksOptimize_MultiScale(pointDir, meshFiles, parameterDictionary)

    if args.tiny_test:
        print("Done with tiny test")
        exit()

    print("\nStep 5. Analysis - Launch ShapeWorksStudio - sparse correspondence model.\n")
    if args.interactive != 0:
        input("Press Enter to continue")

    # Image files are passed because Studio does not support viewing meshes yet.
    launchShapeWorksStudio(pointDir, leftImageFiles, localPointFiles, worldPointFiles)
