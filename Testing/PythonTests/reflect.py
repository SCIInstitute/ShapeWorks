import os
import sys
from shapeworks import *

def reflectTest1():
  img = Image(os.environ["DATA"] + "/la-bin.nrrd")
  img.reflect(Axis.Z)

  compareImg = Image(os.environ["DATA"] + "/reflect1.nrrd")

  return img.compare(compareImg)

utils.test(reflectTest1)

def reflectTest2():
  img = Image(os.environ["DATA"] + "/la-bin.nrrd")
  img.reflect(Axis.X)

  compareImg = Image(os.environ["DATA"] + "/reflect2.nrrd")

  return img.compare(compareImg)

utils.test(reflectTest2)

def reflectTest3():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.reflect(Axis.X)

  compareMesh = Mesh(os.environ["DATA"] + "/reflect1.vtk")

  return mesh == compareMesh

utils.test(reflectTest3)

def reflectTest4():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.reflect(Axis.Y)

  compareMesh = Mesh(os.environ["DATA"] + "/reflect2.vtk")

  return mesh == compareMesh

utils.test(reflectTest4)

def reflectfailTest():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.reflect()

  compareImg = Image(os.environ["DATA"] + "/reflectfail.nrrd")

  return img.compare(compareImg)

utils.expectException(reflectfailTest, TypeError)
