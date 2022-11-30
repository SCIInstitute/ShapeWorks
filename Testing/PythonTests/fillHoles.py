import os
import sys
from shapeworks import *

success = True

def fillholesTest():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.fillHoles()

  compareMesh = Mesh(os.environ["DATA"] + "/fillholes.vtk")

  return mesh == compareMesh

success &= utils.test(fillholesTest)

sys.exit(not success)
