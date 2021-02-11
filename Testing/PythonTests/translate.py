import os
import sys
import shapeworks as sw

def translateTest1():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.translate([10.0, 10.0, 10.0])

  compareImg = sw.Image(os.environ["DATA"] + "/translate1.nrrd")

  return img.compare(compareImg)

val = translateTest1()

if val is False:
  sys.exit(1)

def translateTest2():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.translate([-10.0, -10.0, -10.0])

  compareImg = sw.Image(os.environ["DATA"] + "/translate2.nrrd")

  return img.compare(compareImg)

val = translateTest2()

if val is False:
  sys.exit(1)

def translateTest3():
  mesh = sw.Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.translate([1.0, 1.0, 1.0])

  compareMesh = sw.Mesh(os.environ["DATA"] + "/translate1.vtk")

  return mesh == compareMesh

val = translateTest3()

if val is False:
  sys.exit(1)

def translateTest4():
  mesh = sw.Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.translate([-10.0, -10.0, -10.0])

  compareMesh = sw.Mesh(os.environ["DATA"] + "/translate2.vtk")

  return mesh == compareMesh

val = translateTest4()

if val is False:
  sys.exit(1)

def translateTest5():
  mesh = sw.Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.translate([0.0, 0.0, 1.0])

  compareMesh = sw.Mesh(os.environ["DATA"] + "/translate3.vtk")

  return mesh == compareMesh

val = translateTest5()

if val is False:
  sys.exit(1)