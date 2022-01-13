import os
import sys
from shapeworks import *

success = True

def transformTest1():
    img1 = Image(os.environ["DATA"] + "/transforms/ellipsoid_00.nrrd");
    img2 = img1.copy()

    mesh1 = Mesh(os.environ["DATA"] + "/transforms/ellipsoid_00.vtk");
    mesh2 = Mesh(os.environ["DATA"] + "/transforms/ellipsoid_01.vtk");
    vtkTransform = mesh1.createTransform(mesh2, Mesh.AlignmentType.Rigid, 100);

    img1.applyTransform(vtkTransform)
    img2.applyTransform(vtkTransform, meshTransform=True)

    withoutMeshTransform = Image(os.environ["DATA"] + "/transforms/imageTransformWithoutMeshTransform.nrrd")
    withMeshTransform = Image(os.environ["DATA"] + "/transforms/imageTransformWithMeshTransform.nrrd")
    
    return img1 == withoutMeshTransform and img2 == withMeshTransform

transformTest1()

success &= utils.test(transformTest1)

def transformTest2():
    img1 = Image(os.environ["DATA"] + "/transforms/ellipsoid_00.nrrd");
    img2 = Image (os.environ["DATA"] + "/transforms/ellipsoid_01.nrrd");
    itkTransform = img1.createRigidRegistrationTransform(img2, 0.5, 100);

    mesh1 = Mesh(os.environ["DATA"] + "/transforms/ellipsoid_00.vtk");
    mesh2 = mesh1.copy()

    mesh1.applyTransform(itkTransform)
    mesh2.applyTransform(itkTransform, imageTransform=True)

    withoutImageTransform = Mesh(os.environ["DATA"] + "/transforms/meshTransformWithoutImageTransform.vtk")
    withImageTransform = Mesh(os.environ["DATA"] + "/transforms/meshTransformWithImageTransform.vtk")

    return mesh1 == withoutImageTransform and mesh2 == withImageTransform

transformTest2()

success &= utils.test(transformTest2)

sys.exit(not success)
