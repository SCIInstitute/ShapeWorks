import os
import sys
from shapeworks import *

def decimateTest1():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.decimate()

  compareMesh = Mesh(os.environ["DATA"] + "/decimate1.vtk")

  return mesh == compareMesh

val = decimateTest1()

if val is False:
  print("decimateTest1 failed")
  sys.exit(1)

def decimateTest2():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.decimate(0.0, 0.0, True)

  compareMesh = Mesh(os.environ["DATA"] + "/decimate2.vtk")

  return mesh == compareMesh

val = decimateTest2()

if val is False:
  print("decimateTest2 failed")
  sys.exit(1)
