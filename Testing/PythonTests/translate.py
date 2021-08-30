import os
import sys
from shapeworks import *

def translateTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.translate([10.0, 10.0, 10.0])

  compareImg = Image(os.environ["DATA"] + "/translate1.nrrd")

  return img.compare(compareImg)

utils.test(translateTest1)

def translateTest2():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.translate([-10.0, -10.0, -10.0])

  compareImg = Image(os.environ["DATA"] + "/translate2.nrrd")

  return img.compare(compareImg)

utils.test(translateTest2)

def translateTest3():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.translate([1.0, 1.0, 1.0])

  compareMesh = Mesh(os.environ["DATA"] + "/translate1.vtk")

  return mesh == compareMesh

utils.test(translateTest3)

def translateTest4():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.translate([-10.0, -10.0, -10.0])

  compareMesh = Mesh(os.environ["DATA"] + "/translate2.vtk")

  return mesh == compareMesh

utils.test(translateTest4)

def translateTest5():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.translate([0.0, 0.0, 1.0])

  compareMesh = Mesh(os.environ["DATA"] + "/translate3.vtk")

  return mesh == compareMesh

utils.test(translateTest5)

def translatefailTest():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.translate()

  compareImg = Image(os.environ["DATA"] + "/translatefail.nrrd")

  return img.compare(compareImg)

utils.expectException(translatefailTest, TypeError)
