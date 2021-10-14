import os
import sys
import builtins
import numpy as np
from shapeworks import *

success = True

def closestpointTest1():
  mesh = Mesh(os.environ["DATA"] + "/ellipsoid_0.ply")
  mesh.computeNormals()
  n = mesh.getField("Normals")[42]
  p = mesh.getPoint(42)
  pNew = p + n
  closeToP, outside, face_id = mesh.closestPoint(pNew)

  return np.linalg.norm(p-closeToP) == 0.0 and outside == True and face_id == 90

success &= utils.test(closestpointTest1)

def closestpointTest2():
  mesh = Mesh(os.environ["DATA"] + "/sphere_highres.ply")
  mesh.computeNormals()
  n = mesh.getField("Normals")[42]
  p = mesh.getPoint(42)
  pNew = (p - n) * 1.1
  closeToP, outside, face_id = mesh.closestPoint(pNew)

  return np.linalg.norm(p-closeToP) < 1e-4 and outside == False and face_id == 9

success &= utils.test(closestpointTest2)

sys.exit(not success)
