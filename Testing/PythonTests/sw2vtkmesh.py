import os
import sys
import numpy as np
from shapeworks import *

success = True

def sw2vtkTest():
  swMesh = Mesh(os.environ["DATA"] + "/meanCurvatureEllipsoid.vtk")
  vtkMesh = sw2vtkMesh(swMesh)

  fieldNames = swMesh.getFieldNames()

  for name in fieldNames:
    swField = swMesh.getField(name, Mesh.Point)
    swField.resize(swField.shape[0], 1)
    vtkField = vtkMesh.field_arrays.get_array(name)
    vtkField.resize(vtkField.shape[0], 1)
    if (not np.array_equal(swField, vtkField)):
      return False

  return (swMesh.numPoints() == vtkMesh.n_points and swMesh.numFaces() == vtkMesh.n_cells)

success &= utils.test(sw2vtkTest)

sys.exit(not success)
