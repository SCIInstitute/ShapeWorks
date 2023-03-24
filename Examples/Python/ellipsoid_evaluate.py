import argparse
import os
import sys
import platform
from pathlib import Path
import glob
import shapeworks as sw
import numpy as np





def Run_Pipeline(args):
    ellipsoids_dir = 'Output/ellipsoid'
    shape_models_dir = f'{ellipsoids_dir}/ellipsoid_multiscale_particles'
    if args.tiny_test:
        shape_models_dir = f'{ellipsoids_dir}/ellipsoid_tiny_test_multiscale_particles'
    if not os.path.exists(shape_models_dir):
        print(f'Ellipsoids output not found in {shape_models_dir}. Please run the ellipsoid use case first.', file=sys.stderr)
        sys.exit(1)
    
    eval_dir = f'{ellipsoids_dir}/evaluation'
    for subdir in ('compactness', 'generalization', 'specificity'):
        Path(eval_dir).joinpath(Path(subdir)).mkdir(
            parents=True, exist_ok=True)

    # get the list of all the world particles
    particleFilesList = glob.glob(shape_models_dir+"/*world.particles")
    if(len(particleFilesList)==0):
        print(f'Shape model file at {shape_models_dir} empty. No particle files found',file=sys.stderr)
        print(f'Please run the ellipsoid use case first.', file=sys.stderr)
        sys.exit(1)
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
    save_dir = eval_dir + '/compactness/'

    # Calculate compactness and saved the values in scree.txt
    # Get the compactness of a specific mode 
    nCompactness = sw.ShapeEvaluation.ComputeCompactness(particleSystem=particleSystem,nModes=1)
    print("Compactness value of the 1st mode - ", nCompactness)

    # Get compactness of all the modes
    allCompactness = sw.ShapeEvaluation.ComputeFullCompactness(particleSystem=particleSystem)
    filename = "scree.txt"
    np.savetxt(save_dir+filename,allCompactness)

    if not args.tiny_test:
        sw.plot.plot_mode_line(save_dir,filename,"Compactness","Explained Variance")

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
    save_dir = eval_dir + '/generalization/'

    # Calculate generalization
    # Get the generalization of a specific mode and saves the reconstructions
    nGeneralization = sw.ShapeEvaluation.ComputeGeneralization(particleSystem=particleSystem, nModes=1,saveTo=save_dir)
    print("Generalization value of the 1st mode - ", nGeneralization)

    #Get generalization values for all modes
    allGeneralization = sw.ShapeEvaluation.ComputeFullGeneralization(particleSystem=particleSystem)
    filename = "generalization.txt"
    np.savetxt(save_dir+filename,allGeneralization)

    if not args.tiny_test:
        sw.plot.plot_mode_line(save_dir,filename,"Generalization","Generalization")
        sw.plot.visualize_reconstruction(save_dir)


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
    save_dir = eval_dir + '/specificity/'

    # Calculate specificity of a given mode and saves the reconstructions
    nSpecificity = sw.ShapeEvaluation.ComputeSpecificity(particleSystem=particleSystem, nModes=1,saveTo=save_dir)
    print("Specificity value of the 1st mode - ", nSpecificity)

    #Get specificity values for all modes
    allSpecificity = sw.ShapeEvaluation.ComputeFullSpecificity(particleSystem=particleSystem)
    filename = "specificity.txt"
    np.savetxt(save_dir+filename,allSpecificity)
    if not args.tiny_test:
        sw.plot.plot_mode_line(save_dir,filename,"Specificity","Specificity")
        sw.plot.visualize_reconstruction(save_dir)
