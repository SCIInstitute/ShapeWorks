import os
import sys
import shapeworks as sw

def toDistanceTransformTest():
  mesh = sw.Mesh(os.environ["DATA"] + "/femur.ply")
  img = mesh.toDistanceTransform()

  compareImg = sw.Image(os.environ["DATA"] + "/femurDT.nrrd")

  return img.compare(compareImg)

val = toDistanceTransformTest()

if val is False:
  sys.exit(1)