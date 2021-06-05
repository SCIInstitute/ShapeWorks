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

#violin plot for loadings
def violinplot(loadings,cumulative_variance,shape_models_dir):

    
    
    min_dims = np.where(cumulative_variance <=99)[0]
    #if the first mode is the most dominant, min_dims will be empty
    if(min_dims.size==0):
        min_dims = 1
    else:
        min_dims = min_dims[-1]+1
    print("\nNumber of modes covering 99% varaince - ", min_dims)
    
    loadings = loadings[:,:min_dims]
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
    plt.savefig(shape_models_dir+"pca_loadings_violin_plot.png")
    plt.show(block=False)
    plt.close(fig)

    print("\nLoadings plot saved in directory -" + shape_models_dir)
    print()

# plot the PCA metrics similar to Studio visualization in Analyze pane
def plot_pca_metrics(cumulative_variance,explained_variance,shape_models_dir):
    N = len(cumulative_variance) 
    X = np.array(list(range(N))) + 1
    plt.bar(X, explained_variance)
    plt.plot(X,cumulative_variance,linewidth=4.0,c='black')
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

    print("\nPCA metrics plot saved in directory -" + shape_models_dir)
    print()

def Run_Pipeline(args):

    ellipsoids_dir = 'Output/ellipsoid'
    shape_models_dir = ellipsoids_dir+'/shape_models/128'
    if args.tiny_test:
        shape_models_dir = ellipsoids_dir+'/shape_models/32'
    if not os.path.exists(shape_models_dir):
        print(f'Ellipsoids output not found in {shape_models_dir}. Please run the ellipsoid use case first.', file=sys.stderr)
        sys.exit(1)

    local_point_files = glob.glob(shape_models_dir + "/*local.particles")
    world_point_files = glob.glob(shape_models_dir + "/*world.particles")



    print("\nAnalysis - ShapeWorks PCA Python API\n")
    
    # Read the world/local particle files

    # Provide the list of file names
    particle_data = sw.ParticleSystem(local_point_files)
    
    # Calculate the PCA for the read particle system
    shape_statistics = sw.ParticleShapeStatistics()
    shape_statistics.PCA(particle_data)
    
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
    print("\nSaving the PCA loadings and eigen vectors in the directory : " + shape_models_dir)
    np.savetxt(shape_models_dir+"pca_loadings.txt",pca_loadings)
    np.savetxt(shape_models_dir+"pca_eigen_vectors.txt",eigen_vectors)
    
    if len(local_point_files)>3 and not args.tiny_test:
        violinplot(pca_loadings,cumulative_variance,shape_models_dir)
        plot_pca_metrics(cumulative_variance,explained_variance,shape_models_dir)


    


    