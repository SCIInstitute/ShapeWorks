import os
import sys
from shapeworks import *

success = True

def remeshTest1():
  mesh = Mesh(os.environ["DATA"] + "/ellipsoid_0.ply")
  mesh.remesh(3000, 1.0)

  compareMesh = Mesh(os.environ["DATA"] + "/remesh1.vtk")

  return mesh == compareMesh

success &= utils.test(remeshTest1)

def remeshTest2():
  mesh = Mesh(os.environ["DATA"] + "/ellipsoid_01.vtk")
  mesh.remesh(1000, 2.0)

  compareMesh = Mesh(os.environ["DATA"] + "/remesh2.vtk")

  return mesh == compareMesh

success &= utils.test(remeshTest2)

def remeshPercentTest1():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.remeshPercent(0.25, 1.0)

  compareMesh = Mesh(os.environ["DATA"] + "/remeshPercent1.vtk")

  return mesh == compareMesh

success &= utils.test(remeshPercentTest1)

def remeshPercentTest2():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.remeshPercent(0.1, 0.5)

  compareMesh = Mesh(os.environ["DATA"] + "/remeshPercent2.vtk")

  return mesh == compareMesh

success &= utils.test(remeshPercentTest2)


sys.exit(not success)
