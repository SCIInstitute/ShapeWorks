import os
import sys
from shapeworks import *

success = True

def smoothTest1():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.smooth(10, 0.01)

  compareMesh = Mesh(os.environ["DATA"] + "/smooth1.vtk")

  return mesh == compareMesh

success &= utils.test(smoothTest1)

def smoothTest2():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.smooth()

  compareMesh = Mesh(os.environ["DATA"] + "/smooth2.vtk")

  return mesh == compareMesh

success &= utils.test(smoothTest2)

sys.exit(not success)
