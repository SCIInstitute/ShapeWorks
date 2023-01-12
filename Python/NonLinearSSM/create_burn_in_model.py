INPUT_DIR = '/home/sci/nawazish.khan/non-linear-ssm-experiments/'
DATASET = 'Pancreas'
SEGMENTATION_DIR = f'{INPUT_DIR}/{DATASET}/segmentations'


import glob
import shapeworks as sw
import numpy as np

def center_segmentations():
    seg_files = glob.glob(f'{SEGMENTATION_DIR}/*.nrrd')
    out_dir =  f'{INPUT_DIR}/{DATASET}/segmentations-centered'

    for seg_file in seg_files:
        seg = sw.Image(seg_file)
        seg_name = seg_file.split('/')[-1].split('.nrrd')[0]
        print(seg_name)
        translation = np.eye(4) # Identity
        translation[:3,-1] = -seg.center() # Translate center to (0,0,0)
        seg.applyTransform(translation)
        seg.write(f'{out_dir}/{seg_name}.nrrd')

def convert_to_meshes():
    seg_dir = f'{INPUT_DIR}/{DATASET}/segmentations-centered'
    out_dir = f'{INPUT_DIR}/{DATASET}/meshes/'
    seg_files = glob.glob(f'{seg_dir}/*.nrrd')

    shape_names = []
    meshes = []
    for seg_file in seg_files:
        seg = sw.Image(seg_file)
        seg_name = seg_file.split('/')[-1].split('.nrrd')[0]
        print(seg_name)
        shape_names.append(seg_name)
        seg.antialias(10)
        seg.resample(1, sw.InterpolationType.Linear)
        seg.computeDT()
        seg.gaussianBlur(2.0)
        mesh = seg.toMesh(0.0)  # Get iso surface
        mesh.fillHoles()
        mesh.remeshPercent(percentage=0.50, adaptivity=1.0)  # Perform ACVD Remeshing
        meshes.append(mesh)
    
    mesh_files = sw.utils.save_meshes(out_dir, meshes, shape_names, extension='vtk', compressed=False, verbose=True)

convert_to_meshes()