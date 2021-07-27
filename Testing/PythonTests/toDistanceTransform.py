import os
import sys
from shapeworks import *

def toDistanceTransformTest():
  mesh = Mesh(os.environ["DATA"] + "/femur.ply")
  img = mesh.toDistanceTransform()

  compareImg = Image(os.environ["DATA"] + "/femurDT.nrrd")

  return img.compare(compareImg)

try:
  toDistanceTransformTest()
except ValueError:
  print("toDistanceTransformTest failed")
  sys.exit(1)
