import glob
import os
import shapeworks as sw
from pathlib import Path
import pandas as pd
import numpy as np
import json
PRE_ABLATION = 'pre'
POST_ABLATION = 'post'
JOINT = ''
MODEL_TYPE = JOINT


PROJECT_DIR = '/home/sci/nawazish.khan/SSM-4D/'
meshes_dir = f'{PROJECT_DIR}/old-meshes/'
meshes_pre_groom_dir = f'{PROJECT_DIR}/pre_groomed_meshes/'
shape_models_dir = f'{PROJECT_DIR}/shape_models/'
cur_model_dir = f'{shape_models_dir}/pre/'

DIRS = [meshes_pre_groom_dir]
def make_dirs(dirs):
    for dir in dirs:
        if not os.path.exists(dir):
            os.makedirs(dir)
make_dirs(DIRS)
def build_shapes_info_json():
    all_meshes, mesh_names = sort_files_from_dir(files_dir=meshes_dir)
    subj_set = set()
    for mesh_name in mesh_names:
        print(mesh_name)
        sub_name = mesh_name.split('_20')[0]
        subj_set.add(sub_name)
    
    shapes_info = {}
    shapes_info['ALL_SUBJECTS'] = list(subj_set)
    shapes_info_file = f'{meshes_dir}/description.json'
    for subject in subj_set:
        print(f'Sub -- {subject}')
        # get all pre shapes
        pre_shapes = glob.glob(f"{meshes_dir}/{subject}*_pre*")
        post_shapes = glob.glob(f"{meshes_dir}/{subject}*_post*")
        base_shapes = glob.glob(f"{meshes_dir}/{subject}*base*")
        shapes_info[subject] = {}
        shapes_info[subject]['PRE_ABLATION_TIME_POINTS'] = sorted(list(set(pre_shapes) - set(base_shapes)), key=lambda x: int(x.rsplit('.', 2)[-2]))
        shapes_info[subject]['POST_ABLATION_TIME_POINTS'] = sorted(list(set(post_shapes) - set(base_shapes)), key=lambda x: int(x.rsplit('.', 2)[-2]))
        shapes_info[subject]['BASE_SHAPES'] = sorted(base_shapes)

        print(f"len is {len(shapes_info[subject]['PRE_ABLATION_TIME_POINTS'])}")
    
    with open(shapes_info_file, "w") as outfile:
        json.dump(shapes_info, outfile)

def create_project_with_shapes(model_type='pre'):
    dom_ar = []
    shapes_ar = []
    with open(f"{meshes_dir}/description.json") as json_file:
        shapes_info = json.load(json_file)

    for i in range(0,25):
        shapes_ar.append(f"shape_Time{i}")

    for idx, time_inst in enumerate(shapes_ar):
        cur_ar = []
        for sub in shapes_info['ALL_SUBJECTS']:
            cur_ar.append(shapes_info[sub][f'{MODEL_TYPE.upper()}_ABLATION_TIME_POINTS'][idx])
        dom_ar.append(cur_ar)
    
    df = pd.DataFrame(list(zip(*dom_ar)), columns = shapes_ar)
    df.to_excel(f"{cur_model_dir}/new_project_all.xlsx", sheet_name="data", index=False)
    print('all done')


def pre_groom():
    input_meshes, shape_names = sort_files_from_dir(files_dir=meshes_dir, mid_sub_string=MODEL_TYPE)
    print(f'Loaded {len(input_meshes)} shapes from {meshes_dir}-----')
    meshes = []
    idx = 0
    for mesh_file in input_meshes:
        print(mesh_file)
        mesh = sw.Mesh(mesh_file)
        mesh.smooth(iterations=17, relaxation=1.0)
        translation = np.eye(4) # Identity
        translation[:3,-1] = -mesh.center() # Translate center to (0,0,0)
        mesh.applyTransform(translation)
        print('smoothing , translation done')
        meshes.append(mesh)
        idx += 1
    groomed_meshes = get_rigid_transforms(meshes=meshes, rigid_mesh_available=True, apply_transform=True)
    print("------Writing groomed meshes----")
    groomed_mesh_files = sorted(sw.utils.save_meshes(meshes_pre_groom_dir, groomed_meshes, shape_names, extension='vtk', compressed=False, verbose=True))


def get_rigid_transforms(meshes, rigid_mesh_available=False, apply_transform=False):
    # Find ref mesh and save it
    print('Rigid Transforming...')
    if not rigid_mesh_available:
        ref_index = sw.find_reference_mesh_index(meshes)
        ref_mesh = meshes[ref_index].copy().write(cur_model_dir + '/rigid_reference.vtk')  # Base Model Reference
        ref_mesh = meshes[ref_index]
    else:
        try:
            print(f'----Loading existing Ref Mesh for Rigid Transform-----')
            ref_mesh = sw.Mesh(cur_model_dir + '/rigid_reference.vtk')
            print('Rigid Mesh Loaded')
        except:
            print('Reference Mesh does not exist')
    rigid_transforms = []
  
    for mesh in meshes:
        # compute rigid transformation
        rigid_transform = mesh.createTransform(ref_mesh, sw.Mesh.AlignmentType.Rigid, 100)
        rigid_transforms.append(rigid_transform)
        if apply_transform:
            mesh.applyTransform(rigid_transform)
    if apply_transform:
            return meshes
    return rigid_transforms, ref_mesh

def sort_files_from_dir(files_dir, file_prefix='', mid_sub_string='', file_postfix='', ext='.vtk'):
    loaded_files_path = (glob.glob(f'{files_dir}/{file_prefix}*{mid_sub_string}*{file_postfix}{ext}'))
    sorted_files_path = sorted(loaded_files_path, key = lambda x : os.path.splitext(os.path.basename(x))[0])
    sorted_files_name = [Path(x).stem for x in sorted_files_path]
    return sorted_files_path, sorted_files_name

class Ssm_Project:
    def __init__(self) -> None:
        self.meshes_dir = meshes_dir



# create_project_with_shapes()
pre_groom()