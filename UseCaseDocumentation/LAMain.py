# -*- coding: utf-8 -*-

"""
Most of the following steps even though wrapped in python functions are using
the underlying c++ code, for which we need to call the source paths to the
binaries. This step should be common for any use of a function in ShapeWorks.
__This requires the full ShapeWorks to be correctly built/downloaded!__'

These following commands set the temporary environment variables to point to
shapeworks binaries and set the necessary library paths
"""
from RunLeftAtrium import *

binpath = "../build/shapeworks/src/ShapeWorks-build/bin"
parser = argparse.ArgumentParser(description='Example ShapeWorks LA Pipeline')
parser.add_argument("--interactive", help="Run in interactive mode", action="store_true")
parser.add_argument("--start_with_prepped_data", help="Start with already prepped data", action="store_true")
parser.add_argument("--start_with_image_and_segmentation_data", help = "use images and segmentations data for preprocessing", action="store_true")
parser.add_argument("--use_single_scale", help="Single scale or multi scale optimization", action="store_true")
parser.add_argument("shapeworks_path", help="Path to ShapeWorks executables (default: "+binpath+")", nargs='?', type=str, default=binpath)
args = parser.parse_args()
os.environ["PATH"] = args.shapeworks_path + ":" + os.environ["PATH"]


try:

    Run_LeftAtrium_Pipline(args)

    print("\nShapeworks Pipeline Complete!")


except KeyboardInterrupt:
    print("KeyboardInterrupt exception caught")
    sys.exit(1)
except subprocess.CalledProcessError as e:
    print("general exception caught: "+e.returncode+", "+e.output)
    sys.exit(1)
