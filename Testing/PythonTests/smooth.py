import os
import sys
from shapeworks import *

def smoothTest1():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.smooth(10, 0.01)

  compareMesh = Mesh(os.environ["DATA"] + "/smooth1.vtk")

  return mesh == compareMesh

try:
  smoothTest1()
except ValueError:
  print("smoothTest1 failed")
  sys.exit(1)

def smoothTest2():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.smooth()

  compareMesh = Mesh(os.environ["DATA"] + "/smooth2.vtk")

  return mesh == compareMesh

try:
  smoothTest2()
except ValueError:
  print("smoothTest2 failed")
  sys.exit(1)
