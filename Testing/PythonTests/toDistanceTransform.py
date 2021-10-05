import os
import sys
from shapeworks import *

success = True

def toDistanceTransformTest():
  mesh = Mesh(os.environ["DATA"] + "/femur.ply")
  img = mesh.toDistanceTransform()

  compareImg = Image(os.environ["DATA"] + "/femurDT.nrrd")

  return img.compare(compareImg)

success &= utils.test(toDistanceTransformTest)

sys.exit(not success)
