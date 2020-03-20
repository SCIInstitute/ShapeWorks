import argparse
import os
import sys
import platform
from pathlib import Path

from EvaluationUtils import scree_plot, generalization

binpath = "../build/shapeworks/src/ShapeWorks-build/bin:../../bin"
if platform.system() == "Windows":
    binpath = "C:\\Program Files\ShapeWorks\\bin"
if platform.system() == "Darwin":
    binpath = binpath + ":/Applications/ShapeWorks/bin"

parser = argparse.ArgumentParser(description='Evaluate the ellipsoid example')
parser.add_argument("shapeworks_path", help="Path to ShapeWorks executables (default: "+binpath+")", nargs='?', type=str, default=binpath)
args = parser.parse_args()
binpath = args.shapeworks_path

# Path final
if platform.system() == "Darwin":
    items = binpath.split(os.pathsep)
    binpath = ""
    for item in items:
        binpath = binpath + os.pathsep + item \
            + os.pathsep + item + "/ShapeWorksStudio.app/Contents/MacOS"

os.environ["PATH"] = binpath + os.pathsep + os.environ["PATH"]

def main():
    if not os.path.exists('BadTestEllipsoids'):
        print('No BadTestEllipsoids folder found. Please run `ellipsoidMain.py` first.', fil=sys.stderr)
        sys.exit(1)
    
    eval_dir = 'BadTestEllipsoids/Evaluation'
    for subdir in ('compactness', 'generalization', 'specifcity'):
        Path(eval_dir).joinpath(Path(subdir)).mkdir(parents=True, exist_ok=True)
    
    # Compute compactness
    scree_plot('BadTestEllipsoids/PointFiles/128/*world.particles', f'{eval_dir}/compactness')
    
    # Compute generalization
    generalization('BadTestEllipsoids/PointFiles/128/*world.particles', f'{eval_dir}/generalization')

if __name__ == '__main__':
    main()
