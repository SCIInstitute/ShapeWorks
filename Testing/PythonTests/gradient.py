import os
import sys
from shapeworks import *

def gradientTest():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.applyGradientFilter()

  compareImg = Image(os.environ["DATA"] + "/gradient.nrrd")

  return img.compare(compareImg)

try:
  gradientTest()
except ValueError:
  print("gradientTest failed")
  sys.exit(1)
