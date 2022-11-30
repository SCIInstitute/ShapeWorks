import os
import sys
import numpy as np
from shapeworks import *

success = True

def getpointTest():
  mesh = Mesh(os.environ["DATA"] + "/simple_ellipsoid.ply")
  p = mesh.getPoint(7)
  closeToP = [44.7543, 2.43769, 12.953]

  return np.linalg.norm(p-closeToP) < 1e-4

success &= utils.test(getpointTest)

sys.exit(not success)
