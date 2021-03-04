import os
import sys
from shapeworks import *

def clipTest1():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.clip([0, 20, 0], [1, 55, 0], [1, 75, 0], 0.0)

  compareImg = Image(os.environ["DATA"] + "/clip1.nrrd")

  return img.compare(compareImg)

val = clipTest1()

if val is False:
  sys.exit(1)

def clipTest2():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.clip([0, 20, 0], [1, 55, 0], [1, 75, 0], 3.14)

  compareImg = Image(os.environ["DATA"] + "/clip2.nrrd")

  return img.compare(compareImg)

val = clipTest2()

if val is False:
  sys.exit(1)

def clipTest3():
  img = Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.clip([0, 20, 0], [1, 55, 1], [1, 75, 1], 0.0)

  compareImg = Image(os.environ["DATA"] + "/clip3.nrrd")

  return img.compare(compareImg)

val = clipTest3()

if val is False:
  sys.exit(1)

def clipTest4():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  plane = Plane([0.0, 0.0, 1.0], [-91.0, 0.0, 1230.0])
  mesh.clip(plane)

  compareMesh = Mesh(os.environ["DATA"] + "/clip1.vtk")

  return mesh == compareMesh

val = clipTest4()

if val is False:
  sys.exit(1)

def clipTest5():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  plane = Plane([0.0, 0.0, -1.0], [-91.0, 0.0, 1230.0])
  mesh.clip(plane)

  compareMesh = Mesh(os.environ["DATA"] + "/clip2.vtk")

  return mesh == compareMesh

val = clipTest5()

if val is False:
  sys.exit(1)

def clipTest6():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  plane = Plane([-5.0, 3.14159, 1.0], [-60.0, 10.0, 1235.0])
  mesh.clip(plane)

  compareMesh = Mesh(os.environ["DATA"] + "/clip3.vtk")

  return mesh == compareMesh

val = clipTest6()

if val is False:
  sys.exit(1)

def clipTest7():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  plane = Plane([0.0, 850.0, 0.0], [10.0, 0.0, 10.0])
  mesh.clipClosedSurface(plane)

  compareMesh = Mesh(os.environ["DATA"] + "/clipClosed1.vtk")

  return mesh == compareMesh

val = clipTest7()

if val is False:
  sys.exit(1)
