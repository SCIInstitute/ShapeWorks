import os
import sys
from shapeworks import *

def gradientTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.applyGradientFilter()

  compareImg = Image(os.environ["DATA"] + "/gradient.nrrd")

  return img.compare(compareImg)

val = gradientTest1()

if val is False:
  sys.exit(1)