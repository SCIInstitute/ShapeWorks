import os
import sys
import shapeworks as sw

def smoothTest1():
  mesh = sw.Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.smooth(10, 0.01)

  compareMesh = sw.Mesh(os.environ["DATA"] + "/smooth1.vtk")

  return mesh == compareMesh

val = smoothTest1()

if val is False:
  sys.exit(1)

def smoothTest2():
  mesh = sw.Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.smooth()

  compareMesh = sw.Mesh(os.environ["DATA"] + "/smooth2.vtk")

  return mesh == compareMesh

val = smoothTest2()

if val is False:
  sys.exit(1)