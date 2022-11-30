import os
import sys
from shapeworks import *

success = True

def divTest():
  img = Image(os.environ["DATA"] + "/la-bin.nrrd")
  img /= 3.14

  compareImg = Image(os.environ["DATA"] + "/baseline_divide.nrrd")

  return img.compare(compareImg)

success &= utils.test(divTest)

sys.exit(not success)
