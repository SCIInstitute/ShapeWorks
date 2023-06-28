import os
import sys
import builtins
import numpy as np
from shapeworks import *

success = True

def closestpointTest1():
  mesh = Mesh(os.environ["DATA"] + "/ellipsoid_0.ply")
  mesh.computeNormals()
  n = mesh.getField("Normals", Mesh.Point)[42]
  p = mesh.getPoint(42)
  pNew = p + n
  closeToP, face_id = mesh.closestPoint(pNew)

  return np.linalg.norm(p-closeToP) == 0.0

success &= utils.test(closestpointTest1)

def closestpointTest2():
  mesh = Mesh(os.environ["DATA"] + "/sphere_highres.ply")
  mesh.computeNormals()
  n = mesh.getField("Normals", Mesh.Point)[42]
  p = mesh.getPoint(42)
  pNew = (p - n) * 1.1
  closeToP, face_id = mesh.closestPoint(pNew)

  return np.linalg.norm(p-closeToP) < 1e-4

success &= utils.test(closestpointTest2)

sys.exit(not success)
