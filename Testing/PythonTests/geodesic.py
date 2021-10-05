import os
import sys
from shapeworks import *

success = True

def geodesicTest():
  mesh = Mesh(os.environ["DATA"] + "/ellipsoid_0.ply")
  dist = mesh.geodesicDistance(10, 20)

  return abs(dist - 1.0083) < 1e-4

success &= utils.test(geodesicTest)

sys.exit(not success)
