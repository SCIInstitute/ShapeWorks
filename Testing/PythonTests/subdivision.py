import os
import sys
from shapeworks import *

success = True

def subdivisionTest1():
  femur = Mesh(os.environ["DATA"] + "/m03.vtk")
  femur.applySubdivisionFilter()

  compareMesh = Mesh(os.environ["DATA"] + "/butterfly.vtk")

  return femur == compareMesh

success &= utils.test(subdivisionTest1)

def subdivisionTest2():
  femur = Mesh(os.environ["DATA"] + "/m03.vtk")
  femur.applySubdivisionFilter(Mesh.SubdivisionType.Loop)

  compareMesh = Mesh(os.environ["DATA"] + "/loop.vtk")

  return femur == compareMesh

success &= utils.test(subdivisionTest2)

sys.exit(not success)
