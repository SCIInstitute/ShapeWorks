import os
import sys
import shapeworks as sw

def decimateTest1():
  mesh = sw.Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.decimate()

  compareMesh = sw.Mesh(os.environ["DATA"] + "/decimate1.vtk")

  return mesh == compareMesh

val = decimateTest1()

if val is False:
  sys.exit(1)

def decimateTest2():
  mesh = sw.Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.decimate(0.0, 0.0, True)

  compareMesh = sw.Mesh(os.environ["DATA"] + "/decimate2.vtk")

  return mesh == compareMesh

val = decimateTest2()

if val is False:
  sys.exit(1)