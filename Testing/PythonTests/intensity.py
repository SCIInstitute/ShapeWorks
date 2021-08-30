import os
import sys
from shapeworks import *

def intensityTest():
  img = Image(os.environ["DATA"] + "/nonBinary.nrrd")
  img.applyIntensityFilter(0, 100)

  compareImg = Image(os.environ["DATA"] + "/intensity.nrrd")

  return img.compare(compareImg)

utils.test(intensityTest)
