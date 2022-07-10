from Constants import *
import subprocess
import shapeworks as sw
import matplotlib.pyplot as plt
import math

def reconstruct_meshes_command(input_meshes_files, local_point_files, world_point_files, out_dir):
        reconstructed_meshes_dir = f"{out_dir}/reconstructed_meshes/"
        if not os.path.exists(reconstructed_meshes_dir):
            os.makedirs(reconstructed_meshes_dir)
        existing_reconst_meshes, _ = sort_files_from_dir(files_dir=reconstructed_meshes_dir)
        median_shape_idx  = compute_median_shape(world_point_files=local_point_files)
        if len(existing_reconst_meshes) != 0:
            print(f'---------- Reconstructed Meshes Exist Already, Returning it ----------')
            return existing_reconst_meshes, median_shape_idx
        else:
            print('---------- Reconstructed Mesh Files not found, Reconstructing now -----------')
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
        return reconstructed_mesh_files, 

    
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



def find_norm(self, shape_matrix, a, b):
        """
            Utility function to compute norm between two shape vectors 'a' and 'b' of the cohort
        """
        norm = 0.0
        for i in range(0, shape_matrix.shape[0]):
            norm += (math.fabs(shape_matrix[i, a] - shape_matrix[i, b]))
        return norm


def compute_surface_surface_distance_metric(groomed_mesh_files, local_point_files, world_point_files, out_dir):
        reconstructed_mesh_files, mean_idx = reconstruct_meshes_command(input_meshes_files=groomed_mesh_files, local_point_files=local_point_files, world_point_files=world_point_files, out_dir=out_dir)
        reconstruct_meshes_with_distance_dir = f"{out_dir}/reconstructed_meshes_with_distance/"
        if not os.path.exists(reconstruct_meshes_with_distance_dir):
            os.makedirs(reconstruct_meshes_with_distance_dir)
        print('-----Mesh Warping done, Now computing Surface-Surface distance metric-----')

        errors = []
        idx = 0
        for groomed_mesh_file, reconst_mesh_file in zip(groomed_mesh_files, reconstructed_mesh_files):
            groomed_mesh = sw.Mesh(groomed_mesh_file)
            reconst_mesh = sw.Mesh(reconst_mesh_file)
            if len(errors) == 0:
                errors = np.ones(shape=(len(groomed_mesh_files), len(reconst_mesh.points()))) * np.NaN
            dists_and_indexes = reconst_mesh.distance(target=groomed_mesh, method=sw.Mesh.DistanceMethod.PointToCell)
            distances = np.array(dists_and_indexes[0])
            fn = Path(reconst_mesh_file).stem
            out_file_name = f"{reconstruct_meshes_with_distance_dir}/{fn}.vtk"
            reconst_mesh.setField('distance', distances, sw.Mesh.Point)
            reconst_mesh.write(out_file_name)
            errors[idx, :] = distances
            idx += 1

        print('---------Reconstructed meshes written with distance----------')
        print('Global error (Reconstructed vs. groomed) mean={} rms={} max={}'.format(np.mean(errors), np.sqrt(np.mean(errors**2)), np.max(errors)))
        mean_model_path = reconstructed_mesh_files[mean_idx]
        mean_mesh = sw.Mesh(mean_model_path)
        mean_mesh.setField('mean_error_map', np.mean(errors, axis=0), sw.Mesh.Point)
        mean_mesh.setField('rms_error_map', np.sqrt(np.mean(errors**2, axis=0)), sw.Mesh.Point)
        mean_mesh.setField('max_error_map', np.max(errors, axis=0), sw.Mesh.Point)
        mean_with_dist_fn = f"{out_dir}/mean_mesh_with_distance.vtk"
        mean_mesh.write(mean_with_dist_fn)
        print(f"---------- Reconstructed mesh written with all error maps at {mean_model_path} -----------")
        mean_error = np.mean(errors, axis=0)
        return mean_error


