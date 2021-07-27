import os
import sys
from shapeworks import *

def rotateTest():
  img = Image(os.environ["DATA"] + "/la-bin-centered.nrrd")
  img.rotate(0.7854, [1.0, 1.0, 1.0])

  compareImg = Image(os.environ["DATA"] + "/rotate2.nrrd")

  return img.compare(compareImg)

try:
  rotateTest()
except ValueError:
  print("rotateTest failed")
  sys.exit(1)
