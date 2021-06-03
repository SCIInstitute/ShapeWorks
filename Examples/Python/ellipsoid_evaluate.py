import argparse
import os
import sys
import platform
from pathlib import Path
import glob
import shapeworks as sw
import numpy as np
import matplotlib.pyplot as plt
import subprocess

"""
Function to generate the scree plot based on the explained variance calculated by the ComputeCompactness command
"""


def plot_scree(save_dir):
    # Load scree plot data
    Y = np.loadtxt(save_dir+'/scree.txt')
    N = len(Y)
    X = np.arange(1, N+1)

    # Plot data
    plt.plot(X, Y, linewidth=4.0)
    fig = plt.gcf()
    fig.set_size_inches(10, 10)
    plt.title('Scree Plot')
    plt.xlabel('Mode')
    plt.ylabel('Scree')
    plt.xticks(X)
    plt.ylim(bottom=0, top=1.2)
    plt.xlim(left=1, right=N)
    plt.grid()
    plt.savefig(save_dir+"scree_plot.png")
    plt.show(block=False)
    plt.pause(3)
    plt.close(fig)

    print("Figure saved in directory -" + save_dir)
    print()


"""
Call to ShapeWorksStudio to visualize the best and the worst reconstruction
"""


def visualize_reconstruction(save_dir):
    print("*************************")
    print("* Best reconstruction")
    print("*************************")
    subprocess.check_call(['ShapeWorksStudio', save_dir+'/0perc.xml'])

    print("*************************")
    print("* Worst reconstruction")
    print("*************************")
    subprocess.check_call(['ShapeWorksStudio', save_dir+'/100perc.xml'])

    print()


def Run_Pipeline(args):
    ellipsoids_dir = 'Output/ellipsoid'
    shape_models_dir = f'{ellipsoids_dir}/shape_models/128'
    if args.tiny_test:
        shape_models_dir = f'{ellipsoids_dir}/shape_models/32'
    if not os.path.exists(shape_models_dir):
        print(f'Ellipsoids output not found in {shape_models_dir}. Please run the ellipsoid use case first.', file=sys.stderr)
        sys.exit(1)
    
    eval_dir = f'{ellipsoids_dir}/evaluation'
    for subdir in ('compactness', 'generalization', 'specificity'):
        Path(eval_dir).joinpath(Path(subdir)).mkdir(
            parents=True, exist_ok=True)

    # get the list of all the world particles
    particleFilesList = glob.glob(shape_models_dir+"/*world.particles")
    # read all he particles files into a particleSystem object
    particleSystem = sw.ParticleSystem(particleFilesList)

    print('\nCompactness\n'
          '-----------')
    """
    ComputeCompactness takes a particleSystem which has the particle data to calculate PCA explained variance
    and generate a scree.txt showing the explainability of each mode. This is used to computethe compactness 
    of the SSM.
    """

    # directory where the scree values will be saved
    save_dir = eval_dir + 'compactness/'

    # Calculate compactness and saved the values in scree.txt
    sw.ShapeEvaluation.ComputeCompactness(
        particleSystem=particleSystem, nModes=1, saveTo=save_dir+"scree.txt")
    if not args.tiny_test:
        plot_scree(save_dir)

    """
    ########################################################################################################
    """

    print('\nGeneralization\n'
          '--------------')
    """
    ComputeGeneralization takes a particleSystem which has the particle data and computes the generalization
    of the SSM. The reconstructions are saved, and the 0th and 100th percentile are opened in
    ShapeWorksStudio for visualization
    """

    # directory where the reconstructions related to generalization will be saved
    save_dir = eval_dir + 'generalization/'

    # Calculate generalization
    sw.ShapeEvaluation.ComputeGeneralization(
        particleSystem=particleSystem, nModes=1, saveTo=save_dir)
    if not args.tiny_test:
        visualize_reconstruction(save_dir)

    """
    ########################################################################################################
    """

    print('\nSpecificity\n'
          '--------------')

    """
    ComputeSpecificity takes a particleSystem which has the particle data  and computes the specficity
    of the SSM. The sampled reconstructions are saved, and the 0th and 100th percentile are opened in
    ShapeWorksStudio for visualization
    """

    # directory where the reconstructions related to specificity will be saved
    save_dir = eval_dir + 'specificity/'

    # Calculate specificity
    sw.ShapeEvaluation.ComputeSpecificity(
        particleSystem=particleSystem, nModes=1, saveTo=save_dir)
    if not args.tiny_test:
        visualize_reconstruction(save_dir)
