from msilib.schema import Error
from xml.dom import ValidationErr
import scipy.io as sio
import ants
import numpy as np

INPUT_DIR = '/home/sci/nawazish.khan/non-linear-ssm-experiments/'
DATASET = 'Pancreas'
SEGMENTATION_DIR = f'{INPUT_DIR}/{DATASET}/segmentations-centered'

def deform_and_warp():
    seg_files = sorted(glob.glob(f'{SEGMENTATION_DIR}/*.nrrd'))
    burn_in_dir = f'{INPUT_DIR}/{DATASET}/burn_in_model/'
    medoid_seg_file = f'{burn_in_dir}/medoid_seg.nrrd'
    medoid_particles_file = f'{burn_in_dir}/medoid.particles'
    medoid_particles = np.loadtxt(medoid_particles_file)
    assert medoid_particles.shape[0] == 1024 and medoid_particles.shape[1] == 3
    fi = ants.image_read(medoid_seg_file)
    for seg_file in seg_files:
        mi = ants.image_read(seg_file)
        try:
            print(f'Registering {seg_file.split("/")[-1]}')
            mytx = ants.registration(fixed=fi, moving=mi, type_of_transform = 'SyN' )
        except:
            raise ValidationErr('Registration Error')
        mat_contents = sio.loadmat(mytx['fwdtransforms'][1])
        warp_matrix = mat_contents['AffineTransform_float_3_3']
        assert warp_matrix.shape[0] == 12 and warp_matrix.shape[1] == 1
        warp_matrix_4_4 = np.zeros((4, 4))
        warp_matrix_4_4[0:3, 0:3] = warp_matrix.reshape((3, 4))
        warp_matrix_4_4[3, 3] = 1
        warped_particles = np.zeros(medoid_particles.shape)
        for i in range(warped_particles.shape[0]):
            t = warp_matrix_4_4.
        assert

        