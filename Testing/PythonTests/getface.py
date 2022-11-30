import os
import sys
import numpy as np
from shapeworks import *

success = True

def getfaceTest():
  mesh = Mesh(os.environ["DATA"] + "/simple_ellipsoid.ply")
  f = mesh.getFace(12)
  face = [9,12,1]

  return (f == face).all()

success &= utils.test(getfaceTest)

sys.exit(not success)
