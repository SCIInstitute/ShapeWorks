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
    plyDir = prepDir + 'ply/'
    alignedMeshesDir = prepDir + 'aligned/'


    meshFiles = sorted(glob.glob(parentDir + datasetName + "/meshes/*.ply"))
    imageFiles = sorted(glob.glob(parentDir + datasetName + "/distance_transforms/*.nrrd"))

    # Only do this when want to run on all meshes
    #reflectedFiles_mesh, reflectedFile_img = anatomyPairsToSingles('TestFemur/reflected', meshFiles, imageFiles, 'left')


    #### Step 1, Get only left side femurs
    leftMeshFiles = []
    leftImageFiles = []
    for f, f2 in zip(meshFiles, imageFiles):
        # n09_L is a bad example, it is squished or something
        if 'n09_L_femur.ply' in f or 'n11_L_femur.ply' in f:
            continue
        name = os.path.basename(f)
        side = name.split("_")[1]
        if side == 'L':
            leftMeshFiles.append(f)
            leftImageFiles.append(f2)


    #### Step 2, Convert to .vtk
    if not os.path.exists(vtkDir):
        os.makedirs(vtkDir)
        vtkMeshFiles = []
        for meshFile in leftMeshFiles:
            basename = os.path.basename(meshFile)
            name, ext = os.path.splitext(basename)
            targetName = vtkDir + name + '.vtk'
            subprocess.check_call(['ply2vtk.exe', meshFile, targetName])   
            vtkMeshFiles.append(targetName)
    else:
        vtkMeshFiles = sorted(glob.glob(vtkDir + "*.vtk"))


    #### Step 3, align meshes
    if not os.path.exists(alignedMeshesDir):
        os.makedirs(alignedMeshesDir)
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
            paramFile = alignedMeshesDir + 'ICPRigid3DMeshRegistration_params.txt'
            dictToXML(params, paramFile)
            execCommand = ["ICPRigid3DMeshRegistration.exe", paramFile]
            subprocess.check_call(execCommand)
            alignedMeshes.append(targetName)
    else:
        alignedMeshes = sorted(glob.glob(alignedMeshesDir + "*.vtk"))



    #### Step 4, Convert back to .ply
    if not os.path.exists(plyDir):
        os.makedirs(plyDir)
        plyMeshFiles = []
        for meshFile in alignedMeshes:
            basename = os.path.basename(meshFile)
            name, ext = os.path.splitext(basename)
            targetName = plyDir + name + '.ply'
            subprocess.check_call(['vtk2ply.exe', meshFile, targetName])   
            plyMeshFiles.append(targetName)
        meshFiles = plyMeshFiles
    else:
        meshFiles = sorted(glob.glob(plyDir + "*.ply"))


    meshFiles = meshFiles[:10]
    leftImageFiles = leftImageFiles[:10]

    pointDir = 'TestFemurMesh/PointFiles/'
    if not os.path.exists(pointDir):
        os.makedirs(pointDir)

    parameterDictionary = {
        "use_normals": 0,
        "normal_weight": 0.0,
        "checkpointing_interval" : 1000,
        "keep_checkpoints" : 0,
        "iterations_per_split" : 600,
        "optimization_iterations" : 0,
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
        "verbosity" : 1
        }
    if int(args.use_single_scale) != 0:
        parameterDictionary["number_of_particles"] = 512

        [localPointFiles, worldPointFiles] = runShapeWorksOptimize_SingleScale(pointDir, meshFiles, parameterDictionary)

    else:
        parameterDictionary["starting_particles"] = 32
        parameterDictionary["number_of_levels"] = 3

        [localPointFiles, worldPointFiles] = runShapeWorksOptimize_MultiScale(pointDir, meshFiles, parameterDictionary)

    print(leftImageFiles)
    print(localPointFiles)
    print(worldPointFiles)
    # Image files are passed because Studio does not support viewing meshes yet.
    launchShapeWorksStudio(pointDir, leftImageFiles, localPointFiles, worldPointFiles)
