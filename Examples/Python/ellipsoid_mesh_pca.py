import os
from GroomUtils import *
from OptimizeUtils import *
from AnalyzeUtils import *
import CommonUtils
import shapeworks as sw
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns


def violinplot(loadings,cumulativeVariance,shape_models_dir):

    
    print(cumulativeVariance)
    minDims = np.where(cumulativeVariance <=99)[0]
    #if the first mode is the most dominant, minDims will be empty
    if(minDims.size==0):
        minDims = 1
    else:
        minDims = minDims[-1]+1
    print("Number of modes covering 99% varaince - ", minDims)
    
    loadings = loadings[:,:minDims]
    dims = []
    for i in range(len(loadings)):
        for j in range(np.shape(loadings)[1]):
            dims.append(j+1)
            

    loadings = loadings.flatten()
    data = {'PCA Mode':dims, "PCA Score":loadings}
    df = pd.DataFrame(data) 
    plt.figure(figsize=(6,4),dpi=300)  
    ax = sns.violinplot(x='PCA Mode', y='PCA Score',\
                        data=df, palette="cool_r", split=True, scale="area")
    fig = plt.gcf()
    plt.savefig(shape_models_dir+"pcaLoadings_violin_plot.png")
    plt.show(block=False)
    plt.close(fig)

    print("Figure saved in directory -" + shape_models_dir)
    print()

def plot_pca_metrics(cumulativeVariance,explainedVariance,shape_models_dir):
    N = len(cumulativeVariance) 
    X = np.array(list(range(N))) + 1
    plt.bar(X, explainedVariance)
    plt.plot(X,cumulativeVariance,linewidth=4.0,c='black')
    fig = plt.gcf()
    fig.set_size_inches(10, 10)
    plt.title('Variance Plot')
    plt.xlabel('Mode')
    plt.ylabel('Explained Variance')
    plt.xticks(X)
    plt.grid()
    plt.savefig(shape_models_dir+"variance_plot.png")
    plt.show(block=False)
    plt.close(fig)

    print("Figure saved in directory -" + shape_models_dir)
    print()

def Run_Pipeline(args):

    ellipsoids_dir = 'Output/ellipsoid'
    shape_models_dir = ellipsoids_dir+'/shape_models/128'
    if args.tiny_test:
        shape_models_dir = ellipsoids_dir+'/shape_models/32'
    if not os.path.exists(shape_models_dir):
        print(f'Ellipsoids output not found in {shape_models_dir}. Please run the ellipsoid use case first.', file=sys.stderr)
        sys.exit(1)

    localPointFiles = glob.glob(shape_models_dir + "/*local.particles")
    worldPointFiles = glob.glob(shape_models_dir + "/*world.particles")



    print("\nAnalysis - ShapeWorks PCA Python API\n")
    
    # Read the world/local particle files

    # Provide the list of file names
    particleData = sw.ParticleSystem(localPointFiles)
    
    # Calculate the PCA for the read particle system
    shapeStatistics = sw.ParticleShapeStatistics()
    shapeStatistics.PCA(particleData)
    
    #Calculate the loadings
    shapeStatistics.principalComponentProjections()
    pcaLoadings = shapeStatistics.pcaLoadings()

    np.savetxt(shape_models_dir+"pca_loadings.txt",pcaLoadings)
    
    
    #Calculate the variance explained by each mode using the eigen values
    eigvals = np.array(shapeStatistics.eigenValues())
    explainedVariance = sorted((eigvals/sum(eigvals)),reverse=True)
    explainedVariance = np.array(explainedVariance)*100
    
    # Cummulative variance
    cumulativeVariance = np.array(shapeStatistics.percentVarByMode())*100
    
    if len(localPointFiles)>3 and not args.tiny_test:
        violinplot(pcaLoadings,cumulativeVariance,shape_models_dir)

        plot_pca_metrics(cumulativeVariance,explainedVariance,shape_models_dir)


    


    