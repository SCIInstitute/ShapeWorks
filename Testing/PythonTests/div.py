import os
import sys
from shapeworks import *

def divTest():
  img = Image(os.environ["DATA"] + "/la-bin.nrrd")
  img /= 3.14

  compareImg = Image(os.environ["DATA"] + "/baseline_divide.nrrd")

  return img.compare(compareImg)

try:
  divTest()
except ValueError:
  print("divTest failed")
  sys.exit(1)
