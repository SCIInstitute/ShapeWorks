import os
import sys
from shapeworks import *

def reflectTest1():
  img = Image(os.environ["DATA"] + "/la-bin.nrrd")
  img.reflect(Axis.Z)

  compareImg = Image(os.environ["DATA"] + "/reflect1.nrrd")

  return img.compare(compareImg)

try:
  reflectTest1()
except ValueError:
  print("reflectTest1 failed")
  sys.exit(1)

def reflectTest2():
  img = Image(os.environ["DATA"] + "/la-bin.nrrd")
  img.reflect(Axis.X)

  compareImg = Image(os.environ["DATA"] + "/reflect2.nrrd")

  return img.compare(compareImg)

try:
  reflectTest2()
except ValueError:
  print("reflectTest2 failed")
  sys.exit(1)

def reflectTest3():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.reflect(Axis.X)

  compareMesh = Mesh(os.environ["DATA"] + "/reflect1.vtk")

  return mesh == compareMesh

try:
  reflectTest3()
except ValueError:
  print("reflectTest3 failed")
  sys.exit(1)

def reflectTest4():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.reflect(Axis.Y)

  compareMesh = Mesh(os.environ["DATA"] + "/reflect2.vtk")

  return mesh == compareMesh

try:
  reflectTest4()
except ValueError:
  print("reflectTest4 failed")
  sys.exit(1)

def reflectfailTest():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.reflect()

  compareImg = Image(os.environ["DATA"] + "/reflectfail.nrrd")

  return img.compare(compareImg)

try:
  reflectfailTest()
  print("reflectfailTest failed. There should be no default version of this function.")
  sys.exit(1)
except TypeError:
  sys.exit(0)
