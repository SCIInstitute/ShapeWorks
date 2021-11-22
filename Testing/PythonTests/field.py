import os
import sys
from shapeworks import *
import numpy as np

success = True

def fieldvalueTest1():
  dist = Mesh(os.environ["DATA"] + "/meshdistance2.vtk")
  a = dist.getFieldValue("distance", 0)
  b = dist.getFieldValue("distance", 1000)
  c = dist.getFieldValue("distance", dist.numPoints()-1)

  return abs(a - 0.375761) < 1e-4 and abs(b - 2.18114) < 1e-4 and abs(c - 6.915) < 1e-4

success &= utils.test(fieldvalueTest1)

def fieldvalueTest2():
  mesh = Mesh(os.environ["DATA"] + "/mesh1.vtk")
  a = mesh.getFieldValue("scalars", 0)
  b = mesh.getFieldValue("scalars", 1000)
  c = mesh.getFieldValue("Normals", 4231)   # returns first value of vector fields
  d = mesh.getFieldValue("Normals", 5634)

  return (abs(a - 0.35219) < 1e-4 and
          abs(b - 0.46825) < 1e-4 and
          abs(c - 0.85125) < 1e-4 and
          abs(d - -0.47862) < 1e-4)

success &= utils.test(fieldvalueTest2)

def multifieldvalueTest():
  mesh = Mesh(os.environ["DATA"] + "/mesh1.vtk")
  a = mesh.getMultiFieldValue("Normals", 1024)
  c = mesh.getMultiFieldValue("Normals", 768)

  return (np.allclose(a, np.array([ 0.21653531,  0.34913558, -0.91171086])) and
          np.allclose(c, np.array([ 0.,  0., -1. ])))

success &= utils.test(multifieldvalueTest)

def fieldrangeTest():
  mesh = Mesh(os.environ["DATA"] + "/mesh1.vtk")
  scalarRange = mesh.getFieldRange("scalars")

  return abs(scalarRange[0] - -4.21119) < 1e-4 and abs(scalarRange[1] - 4.52366) < 1e-4

success &= utils.test(fieldrangeTest)

def missingfieldTest():
  mesh = Mesh(os.environ["DATA"] + "/ellipsoid_01.vtk")
  field = mesh.getFieldRange("nonexistent_fieldname")

success &= utils.expectException(missingfieldTest, ValueError)

def getfieldTest1():
  mesh = Mesh(os.environ["DATA"] + "/mesh1.vtk")
  field = mesh.getField("scalars")

  return abs(field[640] - -0.91761) < 1e-4 and abs(field[4800] - 0.56277) < 1e-4

success &= utils.test(getfieldTest1)

def getfieldTest2():
  mesh = Mesh(os.environ["DATA"] + "/mesh1.vtk")
  field = mesh.getField("scalars")

  return field.shape == (36599,)

success &= utils.test(getfieldTest2)

def getfieldTest3():
  mesh = Mesh(os.environ["DATA"] + "/ellipsoid_01.vtk")
  field = mesh.getField("Normals")

  return field.shape == (530, 3)

success &= utils.test(getfieldTest3)

def getfieldTest4():
  mesh = Mesh(os.environ["DATA"] + "/ellipsoid_01.vtk")
  field = mesh.getField("Normals")

  return field.strides == (12, 4)

success &= utils.test(getfieldTest4)

def getfieldTest5():
  mesh = Mesh(os.environ["DATA"] + "/ellipsoid_01.vtk")
  field = mesh.getField("Normals")

  return field.flags["OWNDATA"] == False

success &= utils.test(getfieldTest5)

def getfieldTest6():
  mesh = Mesh(os.environ["DATA"] + "/ellipsoid_01.vtk")
  field = mesh.getField("Normals")

  return field.flags["C_CONTIGUOUS"] == True and field.flags["F_CONTIGUOUS"] == False

success &= utils.test(getfieldTest6)

def getfieldTest7():
  mesh = Mesh(os.environ["DATA"] + "/mesh1.vtk")
  field = mesh.getField("scalars")

  return field[3456] == mesh.getFieldValue("scalars", 3456)  # (it's 0.1789221419275439)

success &= utils.test(getfieldTest7)

def getfieldTest8():
  mesh = Mesh(os.environ["DATA"] + "/mesh1.vtk")
  field = mesh.getField("scalars")
  origval = field[6543]
  field[6543] = 42

  return origval != mesh.getFieldValue("scalars", 6543) and mesh.getFieldValue("scalars", 6543) == 42

success &= utils.test(getfieldTest8)

def setfieldTest1():
  mesh = Mesh(os.environ["DATA"] + "/ellipsoid_01.vtk")
  field = mesh.getField("Normals")
  mesh.setField("newfieldname", field)  # python doesn't own field, so it can't be transferred

success &= utils.expectException(setfieldTest1, ValueError)

def setfieldTest2():
  mesh = Mesh(os.environ["DATA"] + "/ellipsoid_01.vtk")
  field = mesh.getField("Normals")
  mesh.setField("newfieldname", field.copy())

  return "newfieldname" in mesh.getFieldNames()

success &= utils.test(setfieldTest2)

def setfieldTest3():
  mesh = Mesh(os.environ["DATA"] + "/ellipsoid_01.vtk")
  field = np.zeros(1000)
  assert(field.flags["OWNDATA"] == True)
  mesh.setField("newfieldname", field)

  return field.flags["OWNDATA"] == False and "newfieldname" in mesh.getFieldNames()

success &= utils.test(setfieldTest3)

sys.exit(not success)
