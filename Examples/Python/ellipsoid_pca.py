import os
import glob
from pathlib import Path
import shapeworks as sw
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns
import sys

def Run_Pipeline(args):

    ellipsoids_dir = 'Output/ellipsoid'
    shape_models_dir = ellipsoids_dir+'/shape_models/multiscale'
    if args.tiny_test:
        shape_models_dir = ellipsoids_dir+'/shape_models/tiny_test_multiscale'
    if not os.path.exists(shape_models_dir):
        print(f'Ellipsoids output not found in {shape_models_dir}. Please run the ellipsoid use case first.', file=sys.stderr)
        sys.exit(1)

    local_point_files = glob.glob(shape_models_dir + "/*local.particles")
    world_point_files = glob.glob(shape_models_dir + "/*world.particles")


    for subdir in (['pca']):
        Path(ellipsoids_dir).joinpath(Path(subdir)).mkdir(parents=True, exist_ok=True)
    pca_dir = ellipsoids_dir + "/pca/"

    print("\nAnalysis - ShapeWorks PCA Python API\n")
    
    # Read the world/local particle files

    # Provide the list of file names
    particle_data = sw.ParticleSystem(local_point_files)
    
    # Calculate the PCA for the read particle system
    shape_statistics = sw.ParticleShapeStatistics()
    shape_statistics.PCA(particleSystem = particle_data,domainsPerShape=1)
    
    #Calculate the loadings
    shape_statistics.principalComponentProjections()
    pca_loadings = shape_statistics.pcaLoadings()

    print("\nThe sample size of the dataset is : " , shape_statistics.sampleSize())
    print("\nThe dimensions of the dataset are : ", shape_statistics.numDims()) 
    
    #Calculate the variance explained by each mode using the eigen values
    eigen_values = np.array(shape_statistics.eigenValues())
    explained_variance = sorted((eigen_values/sum(eigen_values)),reverse=True)
    explained_variance = np.array(explained_variance)*100
    
    # Cummulative variance
    cumulative_variance = np.array(shape_statistics.percentVarByMode())*100

    #Extract the eigen vectors
    eigen_vectors = np.array(shape_statistics.eigenVectors())
    #Save the loadings
    print("\nSaving the PCA loadings and eigen vectors in the directory : " + pca_dir)
    np.savetxt(pca_dir+"pca_loadings.txt",pca_loadings)
    np.savetxt(pca_dir+"pca_eigen_vectors.txt",eigen_vectors)
    
    if len(local_point_files)>3 and not args.tiny_test:
        sw.plot.pca_loadings_violinplot(pca_loadings,cumulative_variance,pca_dir)
        sw.plot.plot_pca_metrics(cumulative_variance,explained_variance,pca_dir)


    


    