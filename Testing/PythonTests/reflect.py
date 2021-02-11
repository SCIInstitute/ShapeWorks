import os
import sys
import shapeworks as sw

def reflectTest1():
  img = sw.Image(os.environ["DATA"] + "/la-bin.nrrd")
  img.reflect(sw.Axis.Z)

  compareImg = sw.Image(os.environ["DATA"] + "/reflect1.nrrd")

  return img.compare(compareImg)

val = reflectTest1()

if val is False:
  sys.exit(1)

def reflectTest2():
  img = sw.Image(os.environ["DATA"] + "/la-bin.nrrd")
  img.reflect(sw.Axis.X)

  compareImg = sw.Image(os.environ["DATA"] + "/reflect2.nrrd")

  return img.compare(compareImg)

val = reflectTest2()

if val is False:
  sys.exit(1)

def reflectTest3():
  mesh = sw.Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.reflect(sw.Axis.X)

  compareMesh = sw.Mesh(os.environ["DATA"] + "/reflect1.vtk")

  return mesh == compareMesh

val = reflectTest3()

if val is False:
  sys.exit(1)

def reflectTest4():
  mesh = sw.Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.reflect(sw.Axis.Y)

  compareMesh = sw.Mesh(os.environ["DATA"] + "/reflect2.vtk")

  return mesh == compareMesh

val = reflectTest4()

if val is False:
  sys.exit(1)