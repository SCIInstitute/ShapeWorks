import os
import sys
from shapeworks import *

def recenterTest():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.recenter()

  compareImg = Image(os.environ["DATA"] + "/recenter.nrrd")

  return img.compare(compareImg)

try:
  recenterTest()
except ValueError:
  print("recenterTest failed")
  sys.exit(1)
