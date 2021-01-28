# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Statistical Shape Modeling with ShapeWorks
====================================================================

In this example we provide a full pipeline with an example dataset of axis 
aligned ellipsoid images with multiple domains
"""
import os
from GroomUtils import *
from OptimizeUtils import *
from AnalyzeUtils import *
import CommonUtils

"""
Helper function to combine the particle files output after optimize 
is run so that the multiple domains can be view together
"""

def concatenate_particle_files(file_type,domains_per_shape,outputDirectory,shapeFolder):

    # """ Get all files of either the world co-ordinates or local """
    fileList= []
    for file in glob.glob(outputDirectory+"*"+file_type+"*"):
        fileList.append(file)

    num_shapes = int(len(fileList)/domains_per_shape)
    
    #""" Initialize the combined files name list """
    outfile_list = ''

    #""" New folder for the concatinated files """
    combinedDirectory = outputDirectory+"combined/"
    if not os.path.exists(combinedDirectory):
        os.makedirs(combinedDirectory)


    #""" Concate the files """
    for i in range(num_shapes):
        output_filename = combinedDirectory + "joint_" + str(i).zfill(2) + "_" + file_type+".particles"
        outfile_list += output_filename+"\n"
        with open(output_filename,'w',encoding='ascii') as out:
            for j in ['d1','d2']:
                
                in_file = glob.glob(outputDirectory + "*"+str(i).zfill(2) + "_" + j +"*"+file_type+"*")[0]
                print(in_file)
                with open(in_file,'r',encoding='ascii') as inp:
                
                    shutil.copyfileobj(inp,out)

    #""" Save the output file list in the xml file which will be passed to the Studio
    xmlfilename = shapeFolder + "multi_domain_"+file_type+".xml"
    xml = open(xmlfilename,"a")
    xml.write("<?xml version=\"1.0\" ?>\n")
    xml.write("<domains_per_shape>\n" + str(domains_per_shape) + "\n</domains_per_shape>\n")
    xml.write("<point_files>\n" + outfile_list +"\n</point_files>")
    xml.close()

def Run_Pipeline(args):
    

    print("\nStep 1. Extract Data\n")
    if int(args.interactive) != 0:
        input("Press Enter to continue")

    datasetName = "ellipsoid_md_dist"
    outputDirectory = "Output/ellipsoid_md_dist/"
    if not os.path.exists(outputDirectory):
        os.makedirs(outputDirectory)
    CommonUtils.download_and_unzip_dataset(datasetName, outputDirectory)

    
    imgFiles = sorted(glob.glob(outputDirectory + datasetName + "/segmentations/*.nrrd"))
    meshFiles = sorted(glob.glob(outputDirectory + datasetName + "/meshes/*.ply"))
    print(meshFiles)
    
    # groomDir = outputDirectory + "groomed/"
    # # """Apply centering"""
    # centeredFiles = center(groomDir + "centered/segmentations", imgFiles)

    # # """ Apply padding"""
    # paddedFiles = applyPadding(groomDir + "padded/segmentations",centeredFiles,20)

    # #"""Get the distance transforms """
    # dtFiles = applyDistanceTransforms(groomDir, paddedFiles)
  
    pointDir = outputDirectory + 'shape_models/'
    # if not os.path.exists(pointDir):
    #     os.makedirs(pointDir)

    parameterDictionary = {
        "number_of_particles" : [512,512],
        "use_normals": [0,0],
        "normal_weight": [1.0,1.0],
        "checkpointing_interval" : 200,
        "keep_checkpoints" : 0,
        "iterations_per_split" : 500,
        "optimization_iterations" : 500,
        "starting_regularization" :100,
        "ending_regularization" : 0.5,
        "recompute_regularization_interval" : 2,
        "domains_per_shape" : 2,
        "domain_type" : 'mesh',
        "relative_weighting" : 10, # 1 for segmentation images
        "initial_relative_weighting" : 0.1,
        "procrustes_interval" : 0,
        "procrustes_scaling" : 0,
        "save_init_splits" : 0,
        "verbosity" : 3

      }

    if args.tiny_test:
        parameterDictionary["number_of_particles"] = 32
        parameterDictionary["optimization_iterations"] = 25

 
    """
    Now we execute a single scale particle optimization function.
    """
    print("CALLING OPTIMIZATION CODE")
    # [localPointFiles, worldPointFiles] = runShapeWorksOptimize(pointDir, meshFiles, parameterDictionary)
    runShapeWorksOptimize(pointDir, meshFiles, parameterDictionary)



    
    # launchShapeWorksStudio(pointDir, meshFiles, localPointFiles, worldPointFiles)
    particleFolder = pointDir+"512_512/"
    file_type = "local"
    concatenate_particle_files(file_type,2,particleFolder, pointDir)
    command = "ShapeWorksStudio "+ pointDir+ "multi_domain_"+file_type+".xml"
    os.system(command)
