import os
import sys
import numpy as np
from shapeworks import *

def closestpointTest1():
  mesh = Mesh(os.environ["DATA"] + "/ellipsoid_0.ply")
  mesh.generateNormals()
  normals = mesh.getField("Normals")
  p = mesh.getPoint(42)
  pNew = []
  for i in range(3):
      pNew.append(p[i] + normals[42][i])
  closeToP = mesh.closestPoint(pNew)

  return np.linalg.norm(p-closeToP) == 0.0

try:
  if not closestpointTest1():
    print("closestpointTest1 failed")
    sys.exit(1)
except RuntimeError:
  print("closestpointTest1 failed (exception)")
  sys.exit(1)

def closestpointTest2():
  mesh = Mesh(os.environ["DATA"] + "/sphere_highres.ply")
  mesh.generateNormals()
  normals = mesh.getField("Normals")
  p = mesh.getPoint(42)
  pNew = []
  for i in range(3):
      pNew.append(p[i] - normals[42][i] * 1.1)
  closeToP = mesh.closestPoint(pNew)

  return np.linalg.norm(p-closeToP) < 1e-4

try:
  if not closestpointTest2():
    print("closestpointTest2 failed")
    sys.exit(1)
except RuntimeError:
  print("closestpointTest2 failed (exception)")
  sys.exit(1)
