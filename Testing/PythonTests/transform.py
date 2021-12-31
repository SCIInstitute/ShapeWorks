import os
import sys
from shapeworks import *

def transformTest1():
    img1 = Image(os.environ["DATA"] + "/../../Examples/Python/Data/ellipsoid_1mode/segmentations/ellipsoid_00.nrrd");
    copyImg = img1.copy()
    img2 = Image (os.environ["DATA"] + "/../../Examples/Python/Data/ellipsoid_1mode/segmentations/ellipsoid_01.nrrd");
    itkTransform = img1.createRigidRegistrationTransform(img2, 0.5, 100);

    mesh1 = Mesh(os.environ["DATA"] + "/../../Examples/Python/Data/ellipsoid_1mode/meshes/ellipsoid_00.vtk");
    copyMesh = mesh1.copy()
    mesh2 = Mesh(os.environ["DATA"] + "/../../Examples/Python/Data/ellipsoid_1mode/meshes/ellipsoid_01.vtk");
    vtkTransform = mesh1.createTransform(mesh2, Mesh.AlignmentType.Rigid, 100);

    img3 = img1.applyTransform(vtkTransform)
    img4 = copyImg.applyTransform(vtkTransform, meshTransform=True)

    img3.write("imageTransformWithoutMeshTransform.nrrd")
    img4.write("imageTransformWithMeshTransform.nrrd")

    mesh3 = mesh1.applyTransform(itkTransform)
    mesh4 = copyMesh.applyTransform(itkTransform, imageTransform=True)

    mesh3.write("meshTransformWithoutImageTransform.vtk")
    mesh4.write("meshTransformWithImageTransform.vtk")

transformTest1()
