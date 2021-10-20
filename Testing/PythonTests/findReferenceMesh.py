import os
import sys
from shapeworks import *

success = True

def findReferenceMeshTest():
  mesh1 = Mesh(os.environ["DATA"] + "/m03_L_femur.ply")
  mesh2 = Mesh(os.environ["DATA"] + "/m04_L_femur.ply")
  mesh3 = Mesh(os.environ["DATA"] + "/m03.vtk")

  meshList = []
  meshList.append(mesh1)
  meshList.append(mesh2)
  meshList.append(mesh3)

  return MeshUtils.findReferenceMesh(meshList) == 2

success &= utils.test(findReferenceMeshTest)

sys.exit(not success)
