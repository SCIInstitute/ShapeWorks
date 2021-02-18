import os
import sys
from shapeworks import *

def reflectTest1():
  img = Image(os.environ["DATA"] + "/la-bin.nrrd")
  img.reflect(Axis.Z)

  compareImg = Image(os.environ["DATA"] + "/reflect1.nrrd")

  return img.compare(compareImg)

val = reflectTest1()

if val is False:
  sys.exit(1)

def reflectTest2():
  img = Image(os.environ["DATA"] + "/la-bin.nrrd")
  img.reflect(Axis.X)

  compareImg = Image(os.environ["DATA"] + "/reflect2.nrrd")

  return img.compare(compareImg)

val = reflectTest2()

if val is False:
  sys.exit(1)

def reflectTest3():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.reflect(Axis.X)

  compareMesh = Mesh(os.environ["DATA"] + "/reflect1.vtk")

  return mesh == compareMesh

val = reflectTest3()

if val is False:
  sys.exit(1)

def reflectTest4():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.reflect(Axis.Y)

  compareMesh = Mesh(os.environ["DATA"] + "/reflect2.vtk")

  return mesh == compareMesh

val = reflectTest4()

if val is False:
  sys.exit(1)
