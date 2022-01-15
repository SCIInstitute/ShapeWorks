import vtk
import numpy as np
from ShapeCohortGen.CohortGenUtils import *

def addTorus(center, radii, rotation, rotation_axis='Y',resolution=50):
    torus = vtk.vtkParametricTorus()
    torus.SetRingRadius(radii[0])
    torus.SetCrossSectionRadius(radii[1])

    torusSource = vtk.vtkParametricFunctionSource()
    torusSource.SetParametricFunction(torus)
    torusSource.SetUResolution(resolution)
    torusSource.SetVResolution(resolution)
    torusSource.Update()

    transform = vtk.vtkTransform()
    if(rotation_axis=='X'):
        transform.RotateX(rotation)
    elif(rotation_axis=='Y'):
        transform.RotateY(rotation)
    else:
        transform.RotateZ(rotation)

    transform.Translate(center)

    transformFilter = vtk.vtkTransformFilter()
    transformFilter.SetInputConnection(torusSource.GetOutputPort())
    transformFilter.SetTransform(transform)
    transformFilter.Update()

    return transformFilter

def generate_tori(filename, meshDir, randomize_center, randomize_rotation, randomize_ring_radius, randomize_cross_section_radius):
    vtkFileName = meshDir+"torus_"+filename+".vtk"
    if randomize_center:
        center_loc = list(np.random.randint(low = 0,high=50,size=3))
    else:
        center_loc = [0,0,0]
    if randomize_ring_radius:
        ring_radius = np.random.randint(low =15,high=25,size =1)[0]
    else:
        ring_radius = 20
    if randomize_cross_section_radius:
        cross_section_radius = np.random.randint(low =5,high=15,size =1)[0]
    else:
        cross_section_radius = 10
    radii = [ring_radius,cross_section_radius]
    if randomize_rotation:
        rotation = np.random.randint(low=0,high=180,size=1)[0]
    else:
        rotation = 0
    torus = addTorus(center_loc,radii,rotation)

    vtk_writer = vtk.vtkPolyDataWriter()
    vtk_writer.SetInputData(torus.GetOutput())
    vtk_writer.SetFileName(vtkFileName)
    vtk_writer.Update()
    
    
def generate(num_samples,out_dir,randomize_center, randomize_rotation, randomize_ring_radius, randomize_cross_section_radius):
    meshDir = out_dir + "meshes/"
    make_dir(meshDir)
    for i in range(num_samples):
        filename = str(i).zfill(2)
        generate_tori(filename, meshDir, randomize_center, randomize_rotation, randomize_ring_radius, randomize_cross_section_radius)
    return get_files(meshDir)
