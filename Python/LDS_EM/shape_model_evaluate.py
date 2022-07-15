from Constants import *
import subprocess
import shapeworks as sw
import math
import json
from pathlib import Path
import matplotlib as mpl
import matplotlib.pyplot as plt
plt.style.use('plot_style.txt')

def reconstruct_meshes_command(input_meshes_files, local_point_files, out_dir):
        reconstructed_meshes_dir = f"{out_dir}/{RECONSTRUCTED_DIR_NAME}/"
        if not os.path.exists(reconstructed_meshes_dir):
            os.makedirs(reconstructed_meshes_dir)
        existing_reconst_meshes, _ = sort_files_from_dir(files_dir=reconstructed_meshes_dir)
        if len(existing_reconst_meshes) != 0:
            print(f'---------- Reconstructed Meshes Exist Already, Returning it ----------')
            return existing_reconst_meshes
        else:
            print('---------- Reconstructed Mesh Files not found, Reconstructing now -----------')
        median_shape_idx  = compute_median_shape(world_point_files=local_point_files)
        execCommand = ["shapeworks", "warp-mesh", "--reference_mesh", input_meshes_files[median_shape_idx], 
                    "--reference_points", local_point_files[median_shape_idx], "--save_dir", reconstructed_meshes_dir, "--target_points" ]
        for fl in local_point_files:
            execCommand.append(fl)
        execCommand.append('--')
        subprocess.check_call(execCommand)
        print(f"---------- Mesh Reconstruction done at {reconstructed_meshes_dir} ----------")
        reconstructed_mesh_files, _ = sort_files_from_dir(files_dir=reconstructed_meshes_dir)
        if len(reconstructed_mesh_files) == 0:
            raise ValueError('Mesh Reconstruction not done')
        return reconstructed_mesh_files

    
def compute_median_shape( world_point_files):
        """
            Utility function to find the median shape of the cohort, the one with Minimum L1 Norm
        """
        median_shape_idx = -1
        min_sum = 1e10
        shape_matrix = build_shape_matrix(world_point_files)
        num_shapes = shape_matrix.shape[1]
        for i in range(0, num_shapes):
            cur_sum = 0.0
            for j in range(0, num_shapes):
                if i != j:
                    cur_sum += find_norm(shape_matrix, i, j)
            if cur_sum < min_sum:
                min_sum = cur_sum
                median_shape_idx = i
        if median_shape_idx == -1:
            raise ValueError('Median shape not found for Reconstruction, Cannot proceed further with Mesh Warping')
        return median_shape_idx


def build_shape_matrix(world_point_files):
        """
            Utility function to build the shape matrix from the set of particles (world)
        """
        particle_sys = sw.ParticleSystem(world_point_files)
        return particle_sys.Particles()



def find_norm(shape_matrix, a, b):
        """
            Utility function to compute norm between two shape vectors 'a' and 'b' of the cohort
        """
        norm = 0.0
        for i in range(0, shape_matrix.shape[0]):
            norm += (math.fabs(shape_matrix[i, a] - shape_matrix[i, b]))
        return norm


def compute_distance(groomed_mesh_files, reconstructed_mesh_files):
        errors = []
        idx = 0
        for groomed_mesh_file, reconst_mesh_file in zip(groomed_mesh_files, reconstructed_mesh_files):
            groomed_mesh = sw.Mesh(groomed_mesh_file)
            reconst_mesh = sw.Mesh(reconst_mesh_file)
            if len(errors) == 0:
                errors = np.ones(shape=(len(groomed_mesh_files), len(reconst_mesh.points()))) * np.NaN
            dists_and_indexes = reconst_mesh.distance(target=groomed_mesh, method=sw.Mesh.DistanceMethod.PointToCell)
            distances = np.array(dists_and_indexes[0])
            errors[idx, :] = distances
            idx += 1
        mean_error = np.mean(errors, axis=1)
        print(f'mean error shape= {mean_error.shape}')
        return mean_error

def create_project_file_for_time_point(time_point, model_name, mesh_files, particle_files_local, particle_files_world):
    print(f'Creating Project for model {model_name} and t = {time_point}')
    project_location = SHAPE_MODELS_DIR
    project_file_path = f'{SHAPE_MODELS_DIR}/{model_name}_T_{time_point}.xlsx'
    project_subjects = []
    pre = True
    for idx, mesh_file in enumerate(mesh_files):
        subject = sw.Subject()
        subject.set_number_of_domains(1)
        rel_mesh_files = sw.utils.get_relative_paths([mesh_file], project_location)
        rel_groom_files = sw.utils.get_relative_paths([mesh_file], project_location)
        rel_local_particle_files = sw.utils.get_relative_paths([particle_files_local[idx]], project_location)
        rel_world_particle_files = sw.utils.get_relative_paths([particle_files_world[idx]], project_location)
        if pre:
            group_val = 'pre'
            pre = False
        else:
            group_val = 'post'
            pre = True
        sub_group_dict = {"group_type": group_val}
        subject.set_group_values(sub_group_dict)
        subject.set_groomed_filenames(rel_groom_files)
        subject.set_original_filenames(rel_mesh_files)
        subject.set_world_particle_filenames(rel_world_particle_files)
        subject.set_local_particle_filenames(rel_local_particle_files)
        project_subjects.append(subject)
    shapeworks_project = sw.Project()
    shapeworks_project.set_subjects(project_subjects)
    shapeworks_project.save(project_file_path)
     
def reconstruction_error_from_shape_model(shapes_desc_fn=f'{PROJECT_DIR}/description.json', compute_distance_metrics=False):
    """
        Model A: Shape Model generated from Image Registration
        Model B: Proposed Shape Model with Cross-Entropy 
    """
    with open(shapes_desc_fn) as json_file:
        shapes_desc_file = json.load(json_file)
    model_A_meshes_dir = f'{PROJECT_DIR}/{MODEL_A}_meshes/'
    model_A_particles_dir = f'{SHAPE_MODELS_DIR}/{MODEL_A}_particles/'
    model_A_errors = []
    model_B_meshes_dir = f'{PROJECT_DIR}/{MODEL_B}_meshes/'
    model_B_particles_dir = f'{SHAPE_MODELS_DIR}/{MODEL_B}_particles/'
    model_B_errors = []

    model_A_mesh_files_all, _ = sort_files_from_dir(files_dir=model_A_meshes_dir, mid_sub_string='.shift', ext='.vtk' ,exclude_keyword='base')
    model_A_local_point_files, _ = sort_files_from_dir(files_dir=model_A_particles_dir, mid_sub_string='_groomed_local', ext='.particles', exclude_keyword='base')

    model_B_mesh_files_all, _ = sort_files_from_dir(files_dir=model_B_meshes_dir, ext='.vtk', exclude_keyword='base')
    model_B_local_point_files, _ = sort_files_from_dir(files_dir=model_B_particles_dir, mid_sub_string='_local', ext='.particles')
    print(f'{len(model_A_mesh_files_all)} {len(model_B_mesh_files_all)} {len(model_A_local_point_files)} {len(model_B_local_point_files)} ')
    assert len(model_A_mesh_files_all) == len(model_B_mesh_files_all)
    assert len(model_A_local_point_files) == len(model_B_local_point_files)

    print('Reconstructing meshes')
    _ = reconstruct_meshes_command(input_meshes_files=model_A_mesh_files_all, local_point_files=model_A_local_point_files, out_dir=model_A_particles_dir)
    _ = reconstruct_meshes_command(input_meshes_files=model_B_mesh_files_all, local_point_files=model_B_local_point_files, out_dir=model_B_particles_dir)


    for time_point in range(NUM_TIME_POINTS):
        print(f'-------Time Point = {time_point}--------')
        model_A_groomed_files , model_B_groomed_files, model_A_reconstructed_files, model_B_reconstructed_files = [], [], [], []
        model_A_particle_files_local, model_A_particle_files_world, model_B_particle_files_local, model_B_particle_files_world = [], [], [], []
        for subject in shapes_desc_file['ALL_SUBJECTS']:
            pre_fn = Path(shapes_desc_file[subject]['PRE_ABLATION_TIME_POINTS'][time_point]).stem
            post_fn = Path(shapes_desc_file[subject]['POST_ABLATION_TIME_POINTS'][time_point]).stem
            model_A_groomed_files.append(f'{model_A_meshes_dir}/{pre_fn}.vtk') # Model A meshes are saved as .shift
            model_A_groomed_files.append(f'{model_A_meshes_dir}/{post_fn}.vtk')
            model_A_reconstructed_files.append(f'{model_A_particles_dir}/{RECONSTRUCTED_DIR_NAME}/{pre_fn}_groomed_local.vtk')
            model_A_reconstructed_files.append(f'{model_A_particles_dir}/{RECONSTRUCTED_DIR_NAME}/{post_fn}_groomed_local.vtk')
            model_A_particle_files_local.append(f'{model_A_particles_dir}/{pre_fn}_groomed_local.particles')
            model_A_particle_files_local.append(f'{model_A_particles_dir}/{post_fn}_groomed_local.particles')
            model_A_particle_files_world.append(f'{model_A_particles_dir}/{pre_fn}_groomed_world.particles')
            model_A_particle_files_world.append(f'{model_A_particles_dir}/{post_fn}_groomed_world.particles')

            model_B_groomed_files.append(f'{model_B_meshes_dir}/{pre_fn}.vtk')
            model_B_groomed_files.append(f'{model_B_meshes_dir}/{post_fn}.vtk')
            model_B_reconstructed_files.append(f'{model_B_particles_dir}/{RECONSTRUCTED_DIR_NAME}/{pre_fn}_local.vtk')
            model_B_reconstructed_files.append(f'{model_B_particles_dir}/{RECONSTRUCTED_DIR_NAME}/{post_fn}_local.vtk')
            model_B_particle_files_local.append(f'{model_B_particles_dir}/{pre_fn}_local.particles')
            model_B_particle_files_local.append(f'{model_B_particles_dir}/{post_fn}_local.particles')
            model_B_particle_files_world.append(f'{model_B_particles_dir}/{pre_fn}_world.particles')
            model_B_particle_files_world.append(f'{model_B_particles_dir}/{post_fn}_world.particles')

        create_project_file_for_time_point(time_point=time_point, model_name=MODEL_A, mesh_files=model_A_groomed_files, 
                                            particle_files_local=model_A_particle_files_local, particle_files_world=model_A_particle_files_world)
        create_project_file_for_time_point(time_point=time_point, model_name=MODEL_B, mesh_files=model_B_groomed_files, 
                                            particle_files_local=model_B_particle_files_local, particle_files_world=model_B_particle_files_world)
        if compute_distance_metrics:
            model_A_errors.append(compute_distance(groomed_mesh_files=model_A_groomed_files, reconstructed_mesh_files=model_A_reconstructed_files))
            model_B_errors.append(compute_distance(groomed_mesh_files=model_B_groomed_files, reconstructed_mesh_files=model_B_reconstructed_files))


    if compute_distance_metrics:
        model_A_errors = np.array(model_A_errors)
        model_B_errors = np.array(model_B_errors)
        np.savetxt(f'{PROJECT_DIR}/{MODEL_A}_reconst_error.txt', model_A_errors)
        np.savetxt(f'{PROJECT_DIR}/{MODEL_B}_reconst_error.txt', model_B_errors)
        print('Results Saved')

reconstruction_error_from_shape_model(compute_distance_metrics=False)