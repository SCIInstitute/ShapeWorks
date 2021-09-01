import os
import sys
from typing import Type
from shapeworks import *

def scaleTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.scale([1.0, 2.0, 2.0])

  compareImg = Image(os.environ["DATA"] + "/scale1.nrrd")

  return img.compare(compareImg)

utils.test(scaleTest1)

def scaleTest2():
  img = Image(os.environ["DATA"] + "/la-bin-centered.nrrd")
  img.scale([1.0, -2.0, -1.0])

  compareImg = Image(os.environ["DATA"] + "/scale2.nrrd")

  return img.compare(compareImg)

utils.test(scaleTest2)

def scaleTest3():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.scale([1.0, 1.0, 1.0])

  compareMesh = Mesh(os.environ["DATA"] + "/scale1.vtk")

  return mesh == compareMesh

utils.test(scaleTest3)

def scaleTest4():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.scale([-1.0, 1.5, 1.0])

  compareMesh = Mesh(os.environ["DATA"] + "/scale2.vtk")

  return mesh == compareMesh

utils.test(scaleTest4)

def scalefailTest():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.scale()

  compareImg = Image(os.environ["DATA"] + "/scalefail.nrrd")

  return img.compare(compareImg)

utils.expectException(scalefailTest, TypeError)
