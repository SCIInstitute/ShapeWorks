# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Statistical Shape Modeling with ShapeWorks
====================================================================

In this example we provide a full pipeline with an example dataset of axis 
aligned ellipsoid meshes.
"""
import os
from GroomUtils import *
from OptimizeUtils import *
from AnalyzeUtils import *
import CommonUtils
import shapeworks as sw
import numpy as np
import matplotlib.pyplot as plt
def Run_Pipeline(args):
    """
    If ellipsoid.zip is not there it will be downloaded from the ShapeWorks data portal.
    femur.zip will be saved in the /Data folder and the data will be extracted 
    in a newly created directory Output/ellipsoid_mesh.
    """
    print("\nStep 1. Extract Data\n")
    if int(args.interactive) != 0:
        input("Press Enter to continue")

    datasetName = "ellipsoid_1mode_aligned"
    outputDirectory = "Output/ellipsoid_mesh/"
    if not os.path.exists(outputDirectory):
        os.makedirs(outputDirectory)
    #If tiny_test then download subset of the data
    if args.tiny_test:
        args.use_single_scale = 1
        CommonUtils.download_subset(args.use_case,datasetName, outputDirectory)
        meshFiles = sorted(glob.glob(outputDirectory + datasetName + "/meshes/*.ply"))[:3]
    #else download the entire dataset
    else:
        CommonUtils.download_and_unzip_dataset(datasetName, outputDirectory)
        meshFiles = sorted(glob.glob(outputDirectory + datasetName + "/meshes/*.ply"))
    
    # Select data if using subsample
    if args.use_subsample:
        sample_idx = samplemesh(meshFiles, int(args.num_subsample))
        meshFiles = [meshFiles[i] for i in sample_idx]

    pointDir = outputDirectory + 'shape_models/'
    if not os.path.exists(pointDir):
        os.makedirs(pointDir)

    parameterDictionary = {
        "number_of_particles" : 128,
        "use_normals": 0,
        "normal_weight": 10.0,
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
        "verbosity" : 3
      }

    if args.tiny_test:
        parameterDictionary["number_of_particles"] = 32
        parameterDictionary["optimization_iterations"] = 25

    if not args.use_single_scale:
        parameterDictionary["use_shape_statistics_after"] = 32


    """
    Now we execute a single scale particle optimization function.
    """
    [localPointFiles, worldPointFiles] = runShapeWorksOptimize(pointDir, meshFiles, parameterDictionary)

    # if args.tiny_test:
    #     print("Done with tiny test")
    #     exit()

    print("\nStep 5. Analysis - ShapeWorks PCA Python API\n")
    
    # Read the world/local particle files

    # Provide the list of file names
    particleData = sw.ParticleSystem(localPointFiles)
    
    # Calculate the PCA for the read particle system
    shapeStatistics = sw.ParticleShapeStatistics()
    shapeStatistics.PCA(particleData)
    
    #Calculate the loadings
    shapeStatistics.principalComponentProjections()
    pcaLoadings = shapeStatistics.pcaLoadings()
    
    #Calculate the variance explained by each mode using the eigen values
    eigvals = shapeStatistics.eigenValues()
    print(eigvals)
    input('')
    # explainedVariance = sorted((eigvals/sum(eigvals)),reverse=True)
    
    # Cummulative variance
    cumulativeVariance = np.array(shapeStatistics.percentVarByMode())
    
    N = len(cumulativeVariance) 
    X = np.array(list(range(N))) + 1
    # plt.bar(X, explainedVariance)
    plt.plot(X,cumulativeVariance,linewidth=4.0)
    fig = plt.gcf()
    fig.set_size_inches(10, 10)
    plt.title('Variance Plot')
    plt.xlabel('Mode')
    plt.ylabel('Percentage Variance')
    plt.xticks(X)
    # plt.ylim(bottom=0, top=1.2)
    # plt.xlim(left=1, right=N)
    plt.grid()
    plt.savefig(pointDir+"variance_plot.png")
    plt.show(block=False)
    plt.pause(3)
    plt.close(fig)

    print("Figure saved in directory -" + pointDir)
    print()


    # print("\nStep 5. Analysis - Launch ShapeWorksStudio - sparse correspondence model.\n")
    # if args.interactive != 0:
    #     input("Press Enter to continue")

    # launchShapeWorksStudio(pointDir, meshFiles, localPointFiles, worldPointFiles)
    