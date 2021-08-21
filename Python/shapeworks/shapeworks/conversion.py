import pyvista as pv
import numpy as np
import os

# Image.toArray returns a purportedly fortran-ordered array in order to avoid
# being unnecessarily copied when ravel'd or reshaped. These two functions make
# requesting an image's data more specific. Both return numpy arrays wrapped
# around the image's raw data so nothing is copied.
def toArrayC(img):
    arr = img.toArray()
    return arr.reshape((arr.shape[2], arr.shape[1], arr.shape[0]))

# TODO: let's try to make toArray return the C version by default (may not be possible)
#       ** see notes in Image.toArray Python binding function
def toArrayF(img):
    return img.toArray()

# converts shapeworks Image object to vtk image
def sw2vtkImage(swImg, verbose = False):

    # get the numpy array of the shapeworks image as if it were in fortran order
    array  = toArrayF(swImg)

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
