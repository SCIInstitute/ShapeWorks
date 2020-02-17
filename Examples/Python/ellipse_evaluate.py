import argparse
import os
import sys
import platform
from pathlib import Path

from EvaluationUtils import scree_plot, generalization, specificity

def Run_Pipeline(args):
    if not os.path.exists('TestEllipsoids'):
        print('No TestEllipsoids folder found. Please run the ellipse use case first.', file=sys.stderr)
        sys.exit(1)
    
    eval_dir = 'TestEllipsoids/Evaluation'
    for subdir in ('compactness', 'generalization', 'specificity'):
        Path(eval_dir).joinpath(Path(subdir)).mkdir(parents=True, exist_ok=True)
    
    # Compute compactness
    scree_plot('TestEllipsoids/PointFiles/128/*world.particles', f'{eval_dir}/compactness')
    
    # Compute generalization
    generalization('TestEllipsoids/PointFiles/128/*world.particles', f'{eval_dir}/generalization')
    
    # Compute specificity
    specificity('TestEllipsoids/PointFiles/128/*world.particles', f'{eval_dir}/specificity')