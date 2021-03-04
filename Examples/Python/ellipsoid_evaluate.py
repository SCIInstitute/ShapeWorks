import argparse
import os
import sys
import platform
from pathlib import Path
import glob 

from EvaluationUtils import scree_plot, generalization, specificity

def Run_Pipeline(args):
    ellipsoids_dir = 'Output/ellipsoid'
    shape_models_dir = f'{ellipsoids_dir}/shape_models/128'
    if not os.path.exists(shape_models_dir):
        print(f'Ellipsoids output not found in {shape_models_dir}. Please run the ellipsoid use case first.', file=sys.stderr)
        sys.exit(1)
    
    eval_dir = f'{ellipsoids_dir}/evaluation'
    for subdir in ('compactness', 'generalization', 'specificity'):
        Path(eval_dir).joinpath(Path(subdir)).mkdir(parents=True, exist_ok=True)
    

    particleFilesString = ' '.join(glob.glob(shape_models_dir+"/*world.particles"))
    # Compute compactness
    scree_plot(particleFilesString, f'{eval_dir}/compactness')
    
    # Compute generalization
    generalization(particleFilesString, f'{eval_dir}/generalization')
    
    # Compute specificity
    specificity(particleFilesString, f'{eval_dir}/specificity')