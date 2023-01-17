INPUT_DIR = '/home/sci/nawazish.khan/non-linear-ssm-experiments/'
DATASET = 'Pancreas'
SEGMENTATION_DIR = f'{INPUT_DIR}/{DATASET}/segmentations'


import glob
import shapeworks as sw
import numpy as np
import os

parameter_dictionary = {
        "number_of_particles": 1024,
        "use_normals": 0,
        "normal_weight": 10.0,
        "checkpointing_interval": 300,
        "keep_checkpoints": 0,
        "iterations_per_split": 0,
        "optimization_iterations": 1000,
        "starting_regularization": 10,
        "ending_regularization": 1,
        "recompute_regularization_interval": 1,
        "domains_per_shape": 1,
        "relative_weighting": 1,
        "initial_relative_weighting": 0.05,
        "procrustes_interval": 0,
        "procrustes_scaling": 0,
        "save_init_splits": 0,
        "verbosity": 0
}

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

def find_medoid_mesh():
    meshes_dir = f'{INPUT_DIR}/{DATASET}/meshes/'
    mesh_files = sorted(glob.glob(f'{meshes_dir}/*.vtk'))
    meshes = [sw.Mesh(mesh) for mesh in mesh_files]
    shape_names = [mesh.split('/')[-1].split('.vtk')[0] for mesh in mesh_files]

    print("Finding surface-to-surface distances for sorting...")
    distances = np.zeros((len(meshes),len(meshes)))
    for i in range(len(meshes)):
        for j in range(len(meshes)):
            if i != j:
                distances[i][j] = np.mean(meshes[i].distance(meshes[j])[0])
    median_index = np.argmin(np.sum(distances,axis=0) + np.sum(distances,axis=1))

    print(f'Medoid Mesh is {shape_names[median_index]}')
    out_dir = f'{INPUT_DIR}/{DATASET}/burn_in_model/'
    meshes[median_index].write(f'{out_dir}/{shape_names[median_index]}.medoid_mesh.vtk')

def get_medoid_mesh_points():
    burn_in_dir = f'{INPUT_DIR}/{DATASET}/burn_in_model/'
    mesh = sw.Mesh(f'{burn_in_dir}/medoid_mesh.vtk')
    points = mesh.Points()
    # sample 1024 points
    sampled_points = points[np.random.choice(points.shape[0], 1024, replace=False)]
    np.savetxt(f'{burn_in_dir}/medoid_mesh.particles')
    print('Particle set generated')

def create_burn_in_sw_project(dataset_name='supershapes_3_lobes', burn_in_dir_name='supershapes_3_lobes_particles'):
    mesh_dir = f'{INPUT_DIR}/{dataset_name}/meshes/'
    mesh_files = sorted(glob.glob(f'{mesh_dir}/*ply'))
    burn_in_dir = f'{INPUT_DIR}/{dataset_name}/{burn_in_dir_name}/'
    subjects = []
    for i in range(len(mesh_files)):
        subject = sw.Subject()
        subject.set_number_of_domains(1)
        subject.set_original_filenames([mesh_files[i]])
        subject.set_original_filenames([mesh_files[i]])
        rel_particle_file = f'{burn_in_dir}/{mesh_files[i].split("/")[-1].split(".ply")[0]}_local.particles'
        print(rel_particle_file)
        assert os.path.exists(rel_particle_file)
        subject.set_landmarks_filenames([rel_particle_file])
        subjects.append(subject)

    project = sw.Project()
    project.set_subjects(subjects)
    parameters = sw.Parameters()
    # Add param dictionary to spreadsheet
    for key in parameter_dictionary:
        parameters.set(key, sw.Variant([parameter_dictionary[key]]))
    parameters.set("domain_type",sw.Variant('mesh'))
    project.set_parameters("optimize", parameters)
    spreadsheet_file = f"{INPUT_DIR}/{dataset_name}/{dataset_name}_project.xlsx"
    project.save(spreadsheet_file)

create_burn_in_sw_project()