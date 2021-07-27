import os
import sys
from shapeworks import *

def decimateTest1():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.decimate(0.0, 0.0, False)

  compareMesh = Mesh(os.environ["DATA"] + "/decimate1.vtk")

  return mesh == compareMesh

try:
  decimateTest1()
except ValueError:
  print("decimateTest1 failed")
  sys.exit(1)

def decimateTest2():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.decimate(0.0, 0.0)

  compareMesh = Mesh(os.environ["DATA"] + "/decimate2.vtk")

  return mesh == compareMesh

try:
  decimateTest2()
except ValueError:
  print("decimateTest2 failed")
  sys.exit(1)

def decimateTest3():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.decimate()

  compareMesh = Mesh(os.environ["DATA"] + "/decimate3.vtk")

  return mesh == compareMesh

try:
  decimateTest3()
except ValueError:
  print("decimateTest3 failed")
  sys.exit(1)

def decimateTest4():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.decimate(0.9, 25.5, False)

  compareMesh = Mesh(os.environ["DATA"] + "/decimate4.vtk")

  return mesh == compareMesh

try:
  decimateTest4()
except ValueError:
  print("decimateTest4 failed")
  sys.exit(1)

def decimateTest5():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.decimate(0.9, 25.5)

  compareMesh = Mesh(os.environ["DATA"] + "/decimate5.vtk")

  return mesh == compareMesh

try:
  decimateTest5()
except ValueError:
  print("decimateTest5 failed")
  sys.exit(1)
