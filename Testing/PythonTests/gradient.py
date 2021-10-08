import os
import sys
from shapeworks import *

success = True

def gradientTest():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.applyGradientFilter()

  compareImg = Image(os.environ["DATA"] + "/gradient.nrrd")

  return img.compare(compareImg)

success &= utils.test(gradientTest)

sys.exit(not success)
