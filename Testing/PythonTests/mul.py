import os
import sys
from shapeworks import *

success = True

def mulTest():
  img = Image(os.environ["DATA"] + "/la-bin.nrrd")
  img *= 3.14

  compareImg = Image(os.environ["DATA"] + "/baseline_multiply.nrrd")

  return img.compare(compareImg)

success &= utils.test(mulTest)

sys.exit(not success)
