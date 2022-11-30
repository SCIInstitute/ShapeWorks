import os
import sys
from shapeworks import *

success = True

def toDistanceTransformTest():
  mesh = Mesh(os.environ["DATA"] + "/femur_remesh.ply")
  img = mesh.toDistanceTransform(PhysicalRegion(), (5,5,5))

  compareImg = Image(os.environ["DATA"] + "/femur_remesh_dt.nrrd")

  return img.compare(compareImg)

success &= utils.test(toDistanceTransformTest)

sys.exit(not success)
