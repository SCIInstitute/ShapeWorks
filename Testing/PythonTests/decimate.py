import os
import sys
from shapeworks import *

def decimateTest1():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.decimate(0.0, 0.0, False)

  compareMesh = Mesh(os.environ["DATA"] + "/decimate1.vtk")

  return mesh == compareMesh

utils.test(decimateTest1)

def decimateTest2():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.decimate(0.0, 0.0)

  compareMesh = Mesh(os.environ["DATA"] + "/decimate2.vtk")

  return mesh == compareMesh

utils.test(decimateTest2)

def decimateTest3():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.decimate()

  compareMesh = Mesh(os.environ["DATA"] + "/decimate3.vtk")

  return mesh == compareMesh

utils.test(decimateTest3)

def decimateTest4():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.decimate(0.9, 25.5, False)

  compareMesh = Mesh(os.environ["DATA"] + "/decimate4.vtk")

  return mesh == compareMesh

utils.test(decimateTest4)

def decimateTest5():
  mesh = Mesh(os.environ["DATA"] + "/femur.vtk")
  mesh.decimate(0.9, 25.5)

  compareMesh = Mesh(os.environ["DATA"] + "/decimate5.vtk")

  return mesh == compareMesh

utils.test(decimateTest5)

def decimateTest6():
  mesh = Mesh(os.environ["DATA"] + "/ellipsoid_0.ply")
  mesh.cvdDecimate(0.5)

  compareMesh = Mesh(os.environ["DATA"] + "/cvdDecimate1.ply")

  return mesh == compareMesh

utils.test(decimateTest6)

def decimateTest7():
  mesh = Mesh(os.environ["DATA"] + "/ellipsoid_01.vtk")
  mesh.cvdDecimate(1.0)

  compareMesh = Mesh(os.environ["DATA"] + "/cvdDecimate2.vtk")

  return mesh == compareMesh

utils.test(decimateTest7)
