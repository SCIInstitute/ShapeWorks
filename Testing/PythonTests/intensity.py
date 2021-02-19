import os
import sys
from shapeworks import *

def intensityTest():
  img = Image(os.environ["DATA"] + "/nonBinary.nrrd")
  img.applyIntensityFilter(0, 100)

  compareImg = Image(os.environ["DATA"] + "/intensity.nrrd")

  return img.compare(compareImg)

val = intensityTest()

if val is False:
  sys.exit(1)
