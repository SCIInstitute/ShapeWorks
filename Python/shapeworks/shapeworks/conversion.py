import pyvista as pv
import numpy as np
import os

# converts shapeworks Image object to vtk image
def sw2vtkImage(swImg, verbose = False):

    # get the numpy array of the shapeworks image
    array  = swImg.toArray()

    # the numpy array needs to be permuted to match the shapeworks image dimensions
    array = np.transpose(array,(2,1,0))

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

# converts shapeworks Mesh object to vtk mesh (currently saves then loads the
# mesh since getting the vtkPolyData from the C binding isn't working, see
# GitHub issue #825)
def sw2vtkMesh(swMesh, verbose = False):

    if verbose:
        print('Header information: ')
        print(swMesh)

    # save mesh
    swMesh.write('temp.vtk')

    # read mesh into an itk mesh data
    vtkMesh = pv.read('temp.vtk')

    # remove the temp mesh file
    os.remove('temp.vtk')
    
    return vtkMesh
