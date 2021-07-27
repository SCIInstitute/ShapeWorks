import os
import sys
from typing import Type
from shapeworks import *

def scaleTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.scale([1.0, 2.0, 2.0])

  compareImg = Image(os.environ["DATA"] + "/scale1.nrrd")

  return img.compare(compareImg)

try:
  scaleTest1()
except ValueError:
  print("scaleTest1 failed")
  sys.exit(1)

def scaleTest2():
  img = Image(os.environ["DATA"] + "/la-bin-centered.nrrd")
  img.scale([1.0, -2.0, -1.0])

  compareImg = Image(os.environ["DATA"] + "/scale2.nrrd")

  return img.compare(compareImg)

try:
  scaleTest2()
except ValueError:
  print("scaleTest2 failed")
  sys.exit(1)

def scaleTest3():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.scale([1.0, 1.0, 1.0])

  compareMesh = Mesh(os.environ["DATA"] + "/scale1.vtk")

  return mesh == compareMesh

try:
  scaleTest3()
except ValueError:
  print("scaleTest3 failed")
  sys.exit(1)

def scaleTest4():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.scale([-1.0, 1.5, 1.0])

  compareMesh = Mesh(os.environ["DATA"] + "/scale2.vtk")

  return mesh == compareMesh

try:
  scaleTest4()
except ValueError:
  print("scaleTest4 failed")
  sys.exit(1)

def scalefailTest():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.scale()

  compareImg = Image(os.environ["DATA"] + "/scalefail.nrrd")

  return img.compare(compareImg)

try:
  scalefailTest()
  print("scalefailTest failed. There should be no default version of this function.")
  sys.exit(1)
except TypeError:
  sys.exit(0)
