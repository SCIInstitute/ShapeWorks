import os
import sys
import shapeworks as sw

def toImageTest():
  mesh = sw.Mesh(os.environ["DATA"] + "/femur.ply")
  img = mesh.toImage([1.0, 1.0, 1.0])

  compareImg = sw.Image(os.environ["DATA"] + "/femurImage.nrrd")

  return img == compareImg

val = toImageTest()

if val is False:
  sys.exit(1)
