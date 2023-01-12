import shapeworks as sw
import ants
import glob

WORKING_DIR = '/home/sci/nawazish.khan/non-linear-ssm-experiments/data/'

MESHES_DIR = f'{WORKING_DIR}/meshes/'

mesh_file_paths = glob.glob(f'{MESHES_DIR}/groomed/*.vtk')

fi = ants.image_read(mesh_file_paths[0])
fi = ants.resample_image(fi, (60,60), 1, 0)

print("A")
mi = ants.image_read(mesh_file_paths[3])
mi = ants.resample_image(mi, (60,60), 1, 0)

print("B")
mytx = ants.registration(fixed=fi, moving=mi, type_of_transform = 'SyN')
print("C")

# fi = ants.image_read(ants.get_ants_data('r16'))
# mi = ants.image_read(ants.get_ants_data('r64'))
# fi = ants.resample_image(fi, (60,60), 1, 0)
# mi = ants.resample_image(mi, (60,60), 1, 0)
# mytx = ants.registration(fixed=fi, moving=mi, type_of_transform = 'SyN' )