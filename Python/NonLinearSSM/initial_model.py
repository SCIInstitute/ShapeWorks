import time
import scipy.io as sio
import ants
import numpy as np
import glob
import pandas as pd

INPUT_DIR = '/home/sci/nawazish.khan/non-linear-ssm-experiments/'
DATASET = 'Pancreas'
SEGMENTATION_DIR = f'{INPUT_DIR}/{DATASET}/segmentations-centered/'
M = 1024
d = 3

def perform_warp(particles, ants_tranform_ob, inverse=False):
    mytx=ants_tranform_ob
    if inverse:
        mat_contents = sio.loadmat(mytx['fwdtransforms'][1])  
    else:
        mat_contents = sio.loadmat(mytx['invtransforms'][0])  
    warp_matrix = mat_contents['AffineTransform_float_3_3']
    assert warp_matrix.shape[0] == 12 and warp_matrix.shape[1] == 1
    warp_matrix_4_4 = np.zeros((4, 4))
    warp_matrix_4_4[:-1] = warp_matrix.reshape((3, 4))
    warp_matrix_4_4[-1, ...] = [0, 0, 0, 1]
    warped_particles = particles.dot(warp_matrix_4_4.T)
    assert warped_particles.shape[0] == M and warped_particles.shape[1] == (d+1)
    warped_particles = np.delete(warped_particles, -1, axis=1)
    return warped_particles

def deform_and_warp():
    seg_files = sorted(glob.glob(f'{SEGMENTATION_DIR}/*.nrrd'))
    print(f'Loaded {len(seg_files)} ...')
    burn_in_dir = f'{INPUT_DIR}/{DATASET}/burn_in_model/'
    medoid_seg_file = f'{burn_in_dir}/medoid_seg.nrrd'
    medoid_particles_file = f'{burn_in_dir}/medoid_mesh.particles'
    medoid_particles = np.loadtxt(medoid_particles_file)
    assert medoid_particles.shape[0] == M and medoid_particles.shape[1] == d
    medoid_particles = np.insert(medoid_particles, d, 1, axis=-1)

    fi = ants.image_read(medoid_seg_file)
    for seg_file in seg_files:
        mi = ants.image_read(seg_file)
        shape_name = seg_file.split("/")[-1].split('.nrrd')[0]
        try:
            print(f'Registering {shape_name} with Medoid Shape')
            st = time.time()
            mytx = ants.registration(fixed=fi, moving=mi, type_of_transform = 'SyN' )
            end = time.time()
            print("Time=%s" % (end-st))
        except:
            raise ValueError('Registration Error')
        warped_particles_fwd = perform_warp(particles=medoid_particles, ants_tranform_ob=mytx, inverse=False)
        warped_particles_inv = perform_warp(particles=medoid_particles, ants_tranform_ob=mytx, inverse=True)
        np.savetxt(f'{burn_in_dir}/{shape_name}_fwd.particles', warped_particles_fwd)
        np.savetxt(f'{burn_in_dir}/{shape_name}_inv.particles', warped_particles_inv)

def deform_and_warp_new():
    seg_files = sorted(glob.glob(f'{SEGMENTATION_DIR}/*.nrrd'))

    seg_files.reverse()

    print(f'Loaded {len(seg_files)} ...')
    burn_in_dir = f'{INPUT_DIR}/{DATASET}/burn_in_model/'
    medoid_seg_file = f'{burn_in_dir}/medoid_seg.nrrd'
    medoid_particles_file = f'{burn_in_dir}/medoid_mesh.particles'
    medoid_particles = np.loadtxt(medoid_particles_file)
    assert medoid_particles.shape[0] == M and medoid_particles.shape[1] == d
    medoid_df = pd.DataFrame(medoid_particles, columns=['x', 'y', 'z'])

    fi = ants.image_read(medoid_seg_file)
    for idx, seg_file in enumerate(seg_files):
        print(f'******* File {len(seg_files) - idx} out of {len(seg_files)} ******')
        mi = ants.image_read(seg_file)
        shape_name = seg_file.split("/")[-1].split('.nrrd')[0]
        try:
            print(f'Registering {shape_name} with Medoid Shape')
            st = time.time()
            mytx = ants.registration(fixed=fi, moving=mi, type_of_transform = 'SyN' )
            end = time.time()
            print("Time=%s" % (end-st))
        except:
            raise ValueError('Registration Error')
        ptsw = ants.apply_transforms_to_points(3, medoid_df, mytx['fwdtransforms'])
        np.savetxt(f'{burn_in_dir}/{shape_name}_warped.particles', ptsw)

deform_and_warp_new()