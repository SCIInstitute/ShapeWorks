import os
import sys
from shapeworks import *

def translateTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.translate([10.0, 10.0, 10.0])

  compareImg = Image(os.environ["DATA"] + "/translate1.nrrd")

  return img.compare(compareImg)

val = translateTest1()

if val is False:
  print("translateTest1 failed")
  sys.exit(1)

def translateTest2():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.translate([-10.0, -10.0, -10.0])

  compareImg = Image(os.environ["DATA"] + "/translate2.nrrd")

  return img.compare(compareImg)

val = translateTest2()

if val is False:
  print("translateTest2 failed")
  sys.exit(1)

def translateTest3():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.translate([1.0, 1.0, 1.0])

  compareMesh = Mesh(os.environ["DATA"] + "/translate1.vtk")

  return mesh == compareMesh

val = translateTest3()

if val is False:
  print("translateTest3 failed")
  sys.exit(1)

def translateTest4():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.translate([-10.0, -10.0, -10.0])

  compareMesh = Mesh(os.environ["DATA"] + "/translate2.vtk")

  return mesh == compareMesh

val = translateTest4()

if val is False:
  print("translateTest4 failed")
  sys.exit(1)

def translateTest5():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.translate([0.0, 0.0, 1.0])

  compareMesh = Mesh(os.environ["DATA"] + "/translate3.vtk")

  return mesh == compareMesh

val = translateTest5()

if val is False:
  print("translateTest5 failed")
  sys.exit(1)
