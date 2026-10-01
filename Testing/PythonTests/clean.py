import sys
import numpy as np
from shapeworks import *

success = True

def cleanTest():
  # closed tetrahedron plus a doubled fin triangle hanging off edge (0,1)
  points = np.array([[0, 0, 0], [1, 0, 0], [0, 1, 0], [0, 0, 1], [0.5, -0.5, 0.2]], dtype=np.float64)
  faces = np.array([[0, 2, 1], [0, 1, 3], [1, 2, 3], [0, 3, 2], [0, 1, 4], [1, 4, 0]], dtype=np.int32)
  mesh = Mesh(points, faces)
  mesh.clean()

  return mesh.numFaces() == 4 and mesh.numPoints() == 4

success &= utils.test(cleanTest)

sys.exit(not success)
