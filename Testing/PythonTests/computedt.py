import os
import sys
from shapeworks import *

success = True

def computedtTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  # Use legacy method to match existing ground truth
  img.computeDT(0.0, DistanceTransformType.ReinitializeLevelSet)

  compareImg = Image(os.environ["DATA"] + "/computedt1.nrrd")

  return img.compare(compareImg)

success &= utils.test(computedtTest1)

def computedtTest2():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  # Use legacy method to match existing ground truth
  img.computeDT(1.0, DistanceTransformType.ReinitializeLevelSet)

  compareImg = Image(os.environ["DATA"] + "/computedt2.nrrd")

  return img.compare(compareImg)

success &= utils.test(computedtTest2)

def computedtTest3():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  # Use legacy method to match existing ground truth
  img.computeDT(-1.0, DistanceTransformType.ReinitializeLevelSet)

  compareImg = Image(os.environ["DATA"] + "/computedt3.nrrd")

  return img.compare(compareImg)

success &= utils.test(computedtTest3)

sys.exit(not success)
