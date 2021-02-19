import os
import sys
from shapeworks import *

def toImageTest():
  mesh = Mesh(os.environ["DATA"] + "/femur.ply")
  img = mesh.toImage([1.0, 1.0, 1.0])

  compareImg = Image(os.environ["DATA"] + "/femurImage.nrrd")

  return img == compareImg

val = toImageTest()

if val is False:
  sys.exit(1)
