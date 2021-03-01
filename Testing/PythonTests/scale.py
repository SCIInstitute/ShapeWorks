import os
import sys
from shapeworks import *

def scaleTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.scale([1.0, 2.0, 2.0])

  compareImg = Image(os.environ["DATA"] + "/scale1.nrrd")

  return img.compare(compareImg)

val = scaleTest1()

if val is False:
  sys.exit(1)

def scaleTest2():
  img = Image(os.environ["DATA"] + "/la-bin-centered.nrrd")
  img.scale([1.0, -2.0, -1.0])

  compareImg = Image(os.environ["DATA"] + "/scale2.nrrd")

  return img.compare(compareImg)

val = scaleTest2()

if val is False:
  sys.exit(1)

def scaleTest3():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.scale([1.0, 1.0, 1.0])

  compareMesh = Mesh(os.environ["DATA"] + "/scale1.vtk")

  return mesh == compareMesh

val = scaleTest3()

if val is False:
  sys.exit(1)

def scaleTest4():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.scale([-1.0, 1.5, 1.0])

  compareMesh = Mesh(os.environ["DATA"] + "/scale2.vtk")

  return mesh == compareMesh

val = scaleTest4()

if val is False:
  sys.exit(1)
