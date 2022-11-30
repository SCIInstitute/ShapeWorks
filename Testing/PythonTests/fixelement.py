import os
import sys
from shapeworks import *

success = True

def fixelementTest():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.fixElement()

  compareMesh = Mesh(os.environ["DATA"] + "/fixElement.vtk")

  return mesh == compareMesh

success &= utils.test(fixelementTest)

sys.exit(not success)
