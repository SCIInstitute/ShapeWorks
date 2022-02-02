import os
import sys
from shapeworks import *

success = True

def intensityTest():
  img = Image(os.environ["DATA"] + "/nonBinary.nrrd")
  img.applyIntensityFilter(0, 100)

  compareImg = Image(os.environ["DATA"] + "/intensity.nrrd")

  return img.compare(compareImg)

success &= utils.test(intensityTest)

sys.exit(not success)
