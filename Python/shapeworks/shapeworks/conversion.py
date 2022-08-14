import pyvista as pv
import numpy as np
import os
import shapeworks as sw

# converts shapeworks Image object to vtk image
def sw2vtkImage(swImg, verbose = False):

    # get the numpy array of the shapeworks image for viewing (as if it were in fortran order)
    array  = swImg.toArray(for_viewing=True)

    # converting a numpy array to a vtk image using pyvista's wrap function
    vtkImg = pv.wrap(array)

    # set spacing
    spacing = swImg.spacing()
    vtkImg.spacing = [spacing[0], spacing[1], spacing[2]]

    # set origin
    origin = swImg.origin()
    vtkImg.origin = [origin[0], origin[1], origin[2]]

    if verbose:
        print('shapeworks image header information: ')
        print(swImg)

        print('\nvtk image header information: ')
        print(vtkImg)

    return vtkImg

# converts shapeworks Mesh object to vtk mesh
def sw2vtkMesh(swMesh, verbose = False):

    # get points and faces of shapeworks mesh
    points = swMesh.points()
    swfaces = swMesh.faces()

    # create array of type (num of points defining each face, indices of vertices which comprise that face)
    npFaces = np.full((swMesh.numFaces(), 1), 3)
    vtkFaces = np.column_stack((npFaces, swfaces))

    # create polydata
    vtkMesh = pv.PolyData(points, vtkFaces)

    # add fields to polydata
    fieldNames = swMesh.getFieldNames()

    for name in fieldNames:
        field = swMesh.getField(name, sw.Mesh.FieldType.Point)
        vtkMesh.field_data.set_array(field, name)

    if verbose:
        print('shapeworks mesh header information: ')
        print(swMesh)

        print('vtk mesh header information: ')
        print(vtkMesh)

    return vtkMesh
