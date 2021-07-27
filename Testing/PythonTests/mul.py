import os
import sys
from shapeworks import *

def mulTest():
  img = Image(os.environ["DATA"] + "/la-bin.nrrd")
  img *= 3.14

  compareImg = Image(os.environ["DATA"] + "/baseline_multiply.nrrd")

  return img.compare(compareImg)

try:
  mulTest()
except ValueError:
  print("mulTest failed")
  sys.exit(1)
