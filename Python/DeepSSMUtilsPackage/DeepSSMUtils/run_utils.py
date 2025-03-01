import random
import math
import os
import numpy as np
import json

import shapeworks as sw
from bokeh.util.terminal import trace
from shapeworks.utils import sw_message, sw_progress, sw_check_abort, getVTKtransform, getITKtransform

import DataAugmentationUtils
import DeepSSMUtils
import DeepSSMUtils.eval_utils as eval_utils
from DeepSSMUtils import image_utils

import itk

# if SW_FAST_PYTHON_IMPORT env var is set, then we don't need to import the ITK libraries here
if not os.environ.get('SW_FAST_PYTHON_STARTUP'):
    # this is here to cause the ITK libraries to be loaded at a safe time when other threads won't be doing file I/O
    # See #2315
    temporary_parameter_object = itk.ParameterObject.New()


def create_split(project, train, val, test):
    # Create split

    subjects = project.get_subjects()

    print(f"Creating split: {len(subjects)} subjects")

    # create an array of all the index numbers from 0 to the number of subjects
    subject_indices = []

    # first see if any subjects don't have an original mesh, but only an image
    # these will be forced into the test set
    for i in range(len(subjects)):
        filenames = subjects[i].get_original_filenames()
        if len(filenames) == 0 or filenames[0] == '':
            extra_values = subjects[i].get_extra_values()
            extra_values["split"] = "test"
            subjects[i].set_extra_values(extra_values)
        else:
            subject_indices.append(i)

    # set the random seed
    random.seed(972)

    # shuffle indices
    random.shuffle(subject_indices)

    train_indices = subject_indices[:math.floor(len(subject_indices) * train / 100)]
    val_indices = subject_indices[
                  math.floor(len(subject_indices) * train / 100):math.floor(len(subject_indices) * (train + val) / 100)]
    test_indices = subject_indices[math.floor(len(subject_indices) * (train + val) / 100):]

    sw_message(f"Creating split: train:{train}%, val:{val}%, test:{test}%")
    sw_message(f"Split sizes: train:{len(train_indices)}, val:{len(val_indices)}, test:{len(test_indices)}")

    for i in subject_indices:
        extra_values = subjects[i].get_extra_values()
        if i in test_indices:
            extra_values["split"] = "test"
        elif i in val_indices:
            extra_values["split"] = "val"
        else:
            extra_values["split"] = "train"
        subjects[i].set_extra_values(extra_values)

    project.set_subjects(subjects)
    project.save()


def optimize_training_particles(project):
    """ Optimize the particles for just the training data. """
    sw_message(f"Optimizing training particles")
    subjects = project.get_subjects()

    # Exclude all except training, set none to fixed
    for i in range(len(subjects)):
        extra_values = subjects[i].get_extra_values()
        subjects[i].set_excluded(extra_values["split"] != "train")
        subjects[i].set_fixed(False)

    project.set_subjects(subjects)
    project.save()

    optimize = sw.Optimize()
    optimize.SetUpOptimize(project)
    optimize.Run()
    project.save()


def groom_training_shapes(project):
    """ Prepare the data for grooming the training data only. """
    sw_message(f"Grooming training shapes")

    subjects = project.get_subjects()

    # Exclude all except training
    for i in range(len(subjects)):
        extra_values = subjects[i].get_extra_values()
        subjects[i].set_excluded(extra_values["split"] != "train")
        subjects[i].set_fixed(False)

    project.set_subjects(subjects)

    groom = sw.Groom(project)
    groom.run()


def groom_validation_shapes(project):
    """ Prepare the data for grooming the training data only. """
    sw_message(f"Grooming validation shapes")
    subjects = project.get_subjects()
    prep_project_for_val_particles(project)
    groom = sw.Groom(project)
    groom.run()


def prep_project_for_val_particles(project):
    """ Prepare the project for grooming the validation particles. """
    subjects = project.get_subjects()

    # Set training to fixed and
    # Set test to excluded
    for i in range(len(subjects)):
        extra_values = subjects[i].get_extra_values()
        subjects[i].set_excluded(extra_values["split"] == "test")
        subjects[i].set_fixed(extra_values["split"] == "train")

    project.set_subjects(subjects)


def get_deepssm_dir(project):
    """ Get the directory for deepssm data"""
    project_path = project.get_project_path()
    deepssm_dir = project_path + "/deepssm/"
    # if dir project_path/deepssm doesn't exist, create it using python
    if not os.path.exists(deepssm_dir):
        os.makedirs(deepssm_dir)
    # if CWD is the same as project_path, then return "deepssm/"
    if os.getcwd() == project_path:
        return "deepssm/"
    return deepssm_dir


def get_split_indices(project, split):
    """ Get the indices of the subjects. """
    subjects = project.get_subjects()
    list = []
    for i in range(len(subjects)):
        extra_values = subjects[i].get_extra_values()
        match = extra_values["split"] == split
        if match:
            list.append(i)
    return list


def get_training_indices(project):
    """ Get the indices of the training subjects. """
    return get_split_indices(project, "train")


def get_training_bounding_box(project):
    """ Get the bounding box of the training subjects. """
    subjects = project.get_subjects()
    training_indices = get_training_indices(project)
    training_bounding_box = None
    train_mesh_list = []
    for i in training_indices:
        subject = subjects[i]
        mesh = subject.get_groomed_clipped_mesh()
        # apply transform
        alignment = convert_transform_to_numpy(subject.get_groomed_transforms()[0])
        mesh.applyTransform(alignment)
        train_mesh_list.append(mesh)

    bounding_box = sw.MeshUtils.boundingBox(train_mesh_list).pad(10)
    return bounding_box


def convert_transform_to_numpy(transform):
    transform = np.array(transform)
    transform = transform.reshape(4, 4)
    return transform


def groom_training_images(project, indices):
    """ Groom the training images """
    subjects = project.get_subjects()

    deepssm_dir = get_deepssm_dir(project)

    ref_index = sw.utils.get_reference_index(project)
    ref_image = sw.Image(sw.utils.get_image_filename(subjects[ref_index]))
    ref_mesh = sw.utils.load_mesh(subjects[ref_index].get_groomed_filenames()[0])

    # apply alignment transform
    alignment = convert_transform_to_numpy(subjects[ref_index].get_groomed_transforms()[0])

    # Extract rotation matrix (upper left 3x3)
    rotation_matrix = alignment[:3, :3]

    ref_mesh.applyTransform(alignment)
    ref_mesh.write(deepssm_dir + "reference_mesh.vtk")

    # extract translation from alignment transform
    ref_translate = alignment[:3, -1]

    # opposite
    ref_translate = -ref_translate

    # check if this subject needs reflection
    needs_reflection, axis = does_subject_need_reflection(project, subjects[ref_index])

    # apply reflection
    reflection = np.eye(4)
    if needs_reflection:
        reflection[axis, axis] = -1

    # Combine the rotation and reflection
    transform = np.eye(4)
    transform[:3, :3] = rotation_matrix @ reflection[:3, :3]

    # Apply the combined transform to the image
    ref_image.applyTransform(transform, meshTransform=True)

    params = project.get_parameters("deepssm")

    spacing = params.get("spacing")
    if spacing == "":
        spacing = [1, 1, 1]
    else:
        spacing = [float(item) for item in spacing.split(" ")]
    sw_message(f"Spacing: {spacing}")
    ref_image.resample(spacing, sw.InterpolationType.Linear)
    ref_image.setOrigin(ref_image.origin() - ref_translate)

    ref_image.write(deepssm_dir + "reference_image.nrrd")

    # determine bounding box of meshes
    bounding_box = get_training_bounding_box(project)

    bounding_box_string = bounding_box.to_string()
    bounding_box_file = deepssm_dir + "bounding_box.txt"
    with open(bounding_box_file, "w") as f:
        f.write(bounding_box_string)

    sw_message("Grooming training images")
    for i in indices:
        sw_message(f"Index: {i}")

        if sw_check_abort():
            sw_message("Aborted")
            return

        image_name = sw.utils.get_image_filename(subjects[i])
        sw_progress(i / (len(subjects) + 1), f"Grooming Training Image: {image_name}")
        image = sw.Image(image_name)
        subject = subjects[i]
        # get alignment transform
        alignment = convert_transform_to_numpy(subject.get_groomed_transforms()[0])
        # get procrustes transform
        procrustes = convert_transform_to_numpy(subject.get_procrustes_transforms()[0])

        combined_transform = np.matmul(procrustes, alignment)

        # apply combined transform
        image.applyTransform(combined_transform, ref_image.origin(), ref_image.dims(),
                             ref_image.spacing(), ref_image.coordsys(),
                             sw.InterpolationType.Linear, meshTransform=True)

        # apply bounding box
        image.fitRegion(bounding_box)

        # write image using the index of the subject
        sw_message("Saving to " + deepssm_dir + f"/images/{i}.nrrd")
        image.write(deepssm_dir + f"/images/{i}.nrrd")


def run_data_augmentation(project, num_samples, num_dim, percent_variability, sampler, mixture_num=0, processes=1):
    """ Run data augmentation on the training images. """
    sw.utils.initialize_project_mesh_warper(project)
    deepssm_dir = get_deepssm_dir(project)
    aug_dir = deepssm_dir + "augmentation/"

    subjects = project.get_subjects()

    # Gather the training image filenames and training world particle filenames
    train_image_filenames = []
    train_world_particle_filenames = []
    for i in get_training_indices(project):
        train_image_filenames.append(f"deepssm/images/{i}.nrrd")
        particle_file = subjects[i].get_world_particle_filenames()[0]
        train_world_particle_filenames.append(particle_file)

    embedded_dim = DataAugmentationUtils.runDataAugmentation(aug_dir, train_image_filenames,
                                                             train_world_particle_filenames, num_samples,
                                                             num_dim, percent_variability,
                                                             sampler, mixture_num, processes)

    # store embedded_dim in file
    with open(deepssm_dir + "embedded_dim.txt", "w") as f:
        f.write(str(embedded_dim))

    # print("Dimensions retained: " + str(embedded_dim))
    return embedded_dim
    # aug_data_csv = aug_dir + "TotalData.csv"


def does_subject_need_reflection(project, subject):
    """ Determine if the subject needs to be reflected.  Return True/False and the Axis """

    params = project.get_parameters("groom")
    do_reflect = params.get("reflect") == "1" or params.get("reflect") == "True"
    if not do_reflect:
        return False, None

    reflect_column = params.get("reflect_column")
    reflect_choice = params.get("reflect_choice")
    reflect_axis = params.get("reflect_axis")
    if reflect_axis == "X":
        reflect_axis = 0
    elif reflect_axis == "Y":
        reflect_axis = 1
    elif reflect_axis == "Z":
        reflect_axis = 2

    extra_values = subject.get_extra_values()
    if extra_values[reflect_column] == reflect_choice:
        return True, reflect_axis
    else:
        return False, None


def transform_to_string(transform):
    """ Convert a transform to a string of numbers with spaces """

    # first convert to list
    transform_list = transform.flatten().tolist()

    # convert to string using join with spaces
    transform_string = " ".join([str(item) for item in transform_list])

    return transform_string


def rotation_matrix_to_euler_angles(R):
    # Assumes R is a proper rotation matrix, i.e., det(R) = 1
    # The following implementation returns Euler angles following the
    # Tait-Bryan angles convention of ZYX rotation: rotation about X, then Y, then Z

    sy = np.sqrt(R[0, 0] * R[0, 0] + R[1, 0] * R[1, 0])

    singular = sy < 1e-6

    if not singular:
        x = np.arctan2(R[2, 1], R[2, 2])
        y = np.arctan2(-R[2, 0], sy)
        z = np.arctan2(R[1, 0], R[0, 0])
    else:
        x = np.arctan2(-R[1, 2], R[1, 1])
        y = np.arctan2(-R[2, 0], sy)
        z = 0

    # Convert from radians to degrees
    return np.degrees(np.array([x, y, z]))


def format_matrix(matrix, decimals=2):
    """
    Formats a matrix by rounding its elements to a specified number of decimal places,
    ensures consistent padding such that the negative signs are accounted for, and
    returns a string representation without scientific notation.

    Parameters:
    - matrix (np.ndarray): The input matrix to format.
    - decimals (int): Number of decimal places for rounding (default is 2).

    Returns:
    - str: A string representation of the formatted matrix with padding.
    """
    # Round the matrix to the specified number of decimal places
    rounded_matrix = np.round(matrix, decimals)

    # Determine the maximum width for each element to format the numbers with padding
    str_matrix = np.array2string(
        rounded_matrix,
        formatter={'float_kind': lambda x: f"{x:.{decimals}f}"},
        separator=','
    )

    # Strip the extra characters from array2string and convert to a regular list of lists
    rows = str_matrix.replace('[', '').replace(']', '').strip().split('\n')
    rows_list = [list(map(str.strip, row.split(','))) for row in rows]

    # Find the maximum length of the elements for uniform padding
    max_length = max(len(num) for row in rows_list for num in row)

    # Create the final formatted string with consistent width and alignment
    formatted_matrix = "\n".join(
        "[" + " ".join(f"{num:>{max_length}}" for num in row) + "]" for row in rows_list
    )

    return formatted_matrix


def groom_val_test_image(project, image_filename, output_filename, needs_reflection=False, reflection_axis=None,
                         max_translation=None,
                         max_rotation=None,
                         max_iterations=1024,
                         centroid_file=None):
    deepssm_dir = get_deepssm_dir(project)
    # Make dirs
    val_test_images_dir = deepssm_dir + 'images/'
    print(f"val_test_images_dir: {val_test_images_dir}")
    if not os.path.exists(val_test_images_dir):
        os.makedirs(val_test_images_dir)

    # Get reference image
    ref_image_file = deepssm_dir + 'reference_image.nrrd'
    print("Using reference image: " + ref_image_file)
    ref_image = sw.Image(ref_image_file)
    ref_center = ref_image.center()  # get center

    # Read bounding box
    bounding_box_file = deepssm_dir + "bounding_box.txt"
    with open(bounding_box_file, "r") as f:
        bounding_box_string = f.read()

    bounding_box = sw.PhysicalRegion(bounding_box_string)
    large_bb = sw.PhysicalRegion(bounding_box.min, bounding_box.max).pad(80)
    medium_bb = sw.PhysicalRegion(bounding_box.min, bounding_box.max).pad(20)

    print("Loading image: " + image_filename)
    image = sw.Image(image_filename)

    image_file = output_filename

    print(f"Image file: {image_file}")
    axis = reflection_axis

    large_cropped_ref_image_file = deepssm_dir + 'large_cropped_reference_image.nrrd'
    large_cropped_ref_image = sw.Image(large_cropped_ref_image_file)
    medium_cropped_ref_image_file = deepssm_dir + 'medium_cropped_reference_image.nrrd'
    medium_cropped_ref_image = sw.Image(medium_cropped_ref_image_file)
    cropped_ref_image_file = deepssm_dir + 'cropped_reference_image.nrrd'
    cropped_ref_image = sw.Image(cropped_ref_image_file)

    # 1. Apply reflection
    reflection = np.eye(4)
    if needs_reflection:
        reflection[axis, axis] = -1
        # account for offset
        reflection[-1][0] = 2 * image.center()[0]

    image.applyTransform(reflection)
    transform = sw.utils.getVTKtransform(reflection)

    reference_centroid_file = deepssm_dir + "reference_centroid.txt"

    if centroid_file is not None and not os.path.exists(reference_centroid_file):
        print(f"Reference centroid file {reference_centroid_file} does not exist")
        print(f"Current pwd: {os.getcwd()}")

    use_centroid = False
    if centroid_file is not None and os.path.exists(reference_centroid_file):
        use_centroid = True

    # override
    max_translation = 10
    if use_centroid:
        centroid = np.loadtxt(centroid_file)
        ref_center = np.loadtxt(reference_centroid_file)

        # this aligns the centroid of the image to the centroid of the reference image
        translation = ref_center - centroid
        image.setOrigin(image.origin() + translation).write(image_file)
        transform[:3, -1] += translation
        print("Using centroid translation")
        image.write("/tmp/1_center.nrrd")
        image.write("/tmp/2_translate.nrrd")
        max_translation = 0
    else:
        # 2. Translate to have ref center to make rigid registration easier
        translation = ref_center - image.center()
        image.setOrigin(image.origin() + translation).write(image_file)
        transform[:3, -1] += translation
        print(f"Using translation:\n{format_matrix(transform)}")

        image.write("/tmp/1_center.nrrd")

        # 3. Translate with respect to slightly cropped ref
        image = sw.Image(image_file).fitRegion(large_bb).write(image_file)
        itk_translation_transform = image_utils.get_image_registration_transform(large_cropped_ref_image_file,
                                                                                 image_file,
                                                                                 transform_type='translation',
                                                                                 max_translation=max_translation,
                                                                                 max_rotation=max_rotation,
                                                                                 max_iterations=max_iterations)
        print("\nTranslation transform:\n" + str(itk_translation_transform))
        # 4. Apply transform
        image.applyTransform(itk_translation_transform,
                             large_cropped_ref_image.origin(), large_cropped_ref_image.dims(),
                             large_cropped_ref_image.spacing(), large_cropped_ref_image.coordsys(),
                             sw.InterpolationType.Linear, meshTransform=False)
        image.write("/tmp/2_translate.nrrd")
        #vtk_translation_transform = sw.utils.getVTKtransform(itk_translation_transform)
        vtk_translation_transform = itk_translation_transform

        print("\nITK Translation transform:\n" + format_matrix(itk_translation_transform))

        print("\nVTK Translation transform:\n" + format_matrix(vtk_translation_transform))

        print("\nTranspose of itk translation transform:\n" + format_matrix(np.transpose(itk_translation_transform)))

        transform = np.matmul(vtk_translation_transform, transform)

    # transform centroid by the transform and confirm that it now sits at the ref_center
    if use_centroid:
        centroid = np.loadtxt(centroid_file)
        print(f"1: Initial centroid: {centroid}")
        centroid = np.append(centroid, 1)
        centroid = np.matmul(transform, centroid)
        print(f"1: Transformed centroid: {centroid}")
        print(f"1: Reference centroid: {ref_center}")

    print(f"Initial VTK Translation matrix:\n{transform}")

    for iteration in range(1):
        # 5. Crop with medium bounding box and find rigid transform
        image.fitRegion(medium_bb).write(image_file)
        print("max_translation: " + str(max_translation))

        print(f"-----------------------------------------------------")
        print(f"Rigid Registration")

        fixed_image_file = medium_cropped_ref_image_file
        moving_image_file = image_file
        if use_centroid:
            print(f"Ref center: {ref_center}")
            # We want to do a rotation around the rec_center, so we need to translate both images to have ref_center be at 0,0,0
            moving_image = sw.Image(moving_image_file)
            moving_image.setOrigin(moving_image.origin() - ref_center)
            moving_image.write(f"/tmp/moving.nrrd")
            moving_image_file = f"/tmp/moving.nrrd"
            fixed_image = sw.Image(fixed_image_file)
            fixed_image.setOrigin(fixed_image.origin() - ref_center)
            fixed_image.write(f"/tmp/fixed.nrrd")
            fixed_image_file = f"/tmp/fixed.nrrd"

        itk_rigid_transform = image_utils.get_image_registration_transform(fixed_image_file,
                                                                           moving_image_file, transform_type='rigid',
                                                                           max_translation=max_translation,
                                                                           max_rotation=max_rotation,
                                                                           max_iterations=max_iterations)

        print(f"\nRotation transform:\n{format_matrix(itk_rigid_transform)}\n")

        if use_centroid:
            # need to add translate, the rotation, and then translate back
            translation = np.eye(4)
            translation[:3, 3] = -ref_center
            # transpose the translation matrix, not invert
            # translation = np.transpose(translation)
            # translation = getITKtransform(translation)

            print(f"Translation matrix:\n{format_matrix(translation)}")
            print(f"Inverse translation matrix:\n{format_matrix(np.linalg.inv(translation))}")
            itk_rigid_transform = np.linalg.inv(translation) @ np.linalg.inv(itk_rigid_transform) @ translation

        print(f"\nRigid transform after update:\n{format_matrix(itk_rigid_transform)}\n")

        # disable
        # itk_rigid_transform = np.eye(4)

        # sum up translation as a magnitude, itk_rigid_transform is a 4x4, not an ITK object
        translation_vector = itk_rigid_transform[-1, :3]
        print(f"Translation vector: {translation_vector}")
        translation = np.linalg.norm(translation_vector)
        print(f"Translation amount: {translation}")

        # decompose the transform into rotation and translation
        rotation_matrix = itk_rigid_transform[:3, :3]
        rotation = np.arccos((np.trace(rotation_matrix) - 1) / 2) * 180 / np.pi
        print(f"Rotation amount: {rotation}")

        euler_angles = rotation_matrix_to_euler_angles(rotation_matrix)
        print(f"Euler angles (degrees): {euler_angles}")

        # 6. Apply transform
        image.applyTransform(itk_rigid_transform,
                             medium_cropped_ref_image.origin(), medium_cropped_ref_image.dims(),
                             medium_cropped_ref_image.spacing(), medium_cropped_ref_image.coordsys(),
                             sw.InterpolationType.Linear, meshTransform=True)
        print("\nRigid transform:\n" + str(itk_rigid_transform))
        image.write(f"/tmp/3_rigid_{iteration}.nrrd")

        # vtk_rigid_transform = sw.utils.getVTKtransform(itk_rigid_transform)
        vtk_rigid_transform = itk_rigid_transform
        print("\nVTK Rigid transform:\n" + format_matrix(vtk_rigid_transform))
        transform = np.matmul(vtk_rigid_transform, transform)
        print("\nCombined VTK Transform:\n" + format_matrix(transform))

        # transform ref_center using the

    # transform centroid by the transform and confirm that it now sits at the ref_center
    if use_centroid:
        centroid = np.loadtxt(centroid_file)
        print(f"2: Initial centroid: {centroid}")
        centroid = np.append(centroid, 1)
        centroid = np.matmul(transform, centroid)
        print(f"2: Transformed centroid: {centroid}")
        print(f"2: Reference centroid: {ref_center}")

    # import sys
    # sys.exit(0)

    image.fitRegion(bounding_box).write(image_file)

    if not use_centroid:
        # 7. Get similarity transform from image registration and apply

        itk_similarity_transform = image_utils.get_image_registration_transform(cropped_ref_image_file,
                                                                                image_file,
                                                                                transform_type='similarity',
                                                                                max_translation=max_translation,
                                                                                max_rotation=max_rotation,
                                                                                max_iterations=max_iterations)

        image.applyTransform(itk_similarity_transform,
                             cropped_ref_image.origin(), cropped_ref_image.dims(),
                             cropped_ref_image.spacing(), cropped_ref_image.coordsys(),
                             sw.InterpolationType.Linear, meshTransform=False)
        print("\nSimilarity transform:\n" + str(itk_similarity_transform))

        # decompose the transform into rotation and translation and scale
        rotation_matrix = itk_similarity_transform[:3, :3]
        rotation = np.arccos((np.trace(rotation_matrix) - 1) / 2) * 180 / np.pi
        print(f"Rotation amount: {rotation}")
        translation_vector = itk_similarity_transform[-1, :3]
        print(f"Translation vector: {translation_vector}")
        translation = np.linalg.norm(translation_vector)
        print(f"Translation amount: {translation}")
        scale = np.linalg.norm(rotation_matrix)
        print(f"Scale amount: {scale}")

        image.write("/tmp/4_similarity.nrrd")
        image.write(f"/tmp/4_similarity_{iteration}.nrrd")
        #vtk_similarity_transform = sw.utils.getVTKtransform(itk_similarity_transform)
        vtk_similarity_transform = itk_similarity_transform
        transform = np.matmul(vtk_similarity_transform, transform)

    print(f"Image file written: {image_file}")
    image.write(image_file)

    print(f"Final Transform:\n{format_matrix(transform)}")
    return transform


def groom_val_test_images(project, indices, max_translation=None, max_rotation=None, max_iterations=1024):
    """ Groom the validation and test images """
    subjects = project.get_subjects()
    deepssm_dir = get_deepssm_dir(project)

    print(f"max_translation: {max_translation}")
    print(f"max_rotation: {max_rotation}")
    print(f"max_iterations: {max_iterations}")

    # Get reference image
    ref_image_file = deepssm_dir + 'reference_image.nrrd'
    ref_image = sw.Image(ref_image_file)
    ref_center = ref_image.center()  # get center

    # Read bounding box
    bounding_box_file = deepssm_dir + "bounding_box.txt"
    with open(bounding_box_file, "r") as f:
        bounding_box_string = f.read()
    bounding_box = sw.PhysicalRegion(bounding_box_string)

    # Slightly cropped ref image
    large_bb = sw.PhysicalRegion(bounding_box.min, bounding_box.max).pad(80)
    large_cropped_ref_image_file = deepssm_dir + 'large_cropped_reference_image.nrrd'
    large_cropped_ref_image = sw.Image(ref_image_file).fitRegion(large_bb).write(large_cropped_ref_image_file)
    # Further cropped ref image
    medium_bb = sw.PhysicalRegion(bounding_box.min, bounding_box.max).pad(20)
    medium_cropped_ref_image_file = deepssm_dir + 'medium_cropped_reference_image.nrrd'
    medium_cropped_ref_image = sw.Image(ref_image_file).fitRegion(medium_bb).write(medium_cropped_ref_image_file)
    # Fully cropped ref image
    cropped_ref_image_file = deepssm_dir + 'cropped_reference_image.nrrd'
    cropped_ref_image = sw.Image(ref_image_file).fitRegion(bounding_box).write(cropped_ref_image_file)

    # Make dirs
    val_test_images_dir = deepssm_dir + 'images/'
    if not os.path.exists(val_test_images_dir):
        os.makedirs(val_test_images_dir)

    val_test_indices = indices

    val_test_transforms = []
    val_test_image_files = []

    count = 1
    for i in val_test_indices:
        print(f"Index: {i}")
        if sw_check_abort():
            sw_message("Aborted")
            return

        image_name = sw.utils.get_image_filename(subjects[i])
        sw_progress(count / (len(val_test_indices) + 1),
                    f"Grooming val/test image {image_name} ({count}/{len(val_test_indices)})")
        count = count + 1
        needs_reflection, axis = does_subject_need_reflection(project, subjects[i])

        output_filename = val_test_images_dir + f"{i}.nrrd"
        transform = groom_val_test_image(project, image_name, output_filename, needs_reflection, axis, max_translation,
                                         max_rotation,
                                         max_iterations)

        # 8. Save transform
        val_test_transforms.append(transform)
        extra_values = subjects[i].get_extra_values()
        extra_values["registration_transform"] = transform_to_string(transform)

        subjects[i].set_extra_values(extra_values)
    project.set_subjects(subjects)


def prepare_data_loaders(project, batch_size, split="all"):
    """ Prepare PyTorch loaders """
    deepssm_dir = get_deepssm_dir(project)
    loader_dir = deepssm_dir + 'torch_loaders/'
    if not os.path.exists(loader_dir):
        os.makedirs(loader_dir)

    if split == "all" or split == "val":
        val_image_files = []
        val_world_particles = []
        val_indices = get_split_indices(project, "val")
        for i in val_indices:
            val_image_files.append(deepssm_dir + "images/" + f"{i}.nrrd")
            particle_file = project.get_subjects()[i].get_world_particle_filenames()[0]
            val_world_particles.append(particle_file)
        DeepSSMUtils.getValidationLoader(loader_dir, val_image_files, val_world_particles)

    if split == "all" or split == "train":
        aug_dir = deepssm_dir + "augmentation/"
        aug_data_csv = aug_dir + "TotalData.csv"
        DeepSSMUtils.getTrainLoader(loader_dir, aug_data_csv, batch_size)

    if split == "all" or split == "test":
        test_image_files = []
        test_indices = get_split_indices(project, "test")
        for i in test_indices:
            test_image_files.append(deepssm_dir + "images/" + f"{i}.nrrd")
        DeepSSMUtils.getTestLoader(loader_dir, test_image_files)


def get_test_alignment_transform(project, index):
    subjects = project.get_subjects()
    alignment = subjects[index].get_extra_values()["registration_transform"]
    # parse alignment from space separated string to 4x4 matrix
    alignment = alignment.split()
    alignment = [float(i) for i in alignment]
    alignment = np.array(alignment)
    alignment = alignment.reshape(4, 4)
    return alignment


def process_test_predictions(project, config_file):
    """ Process the test predictions, generate distance meshes and CSV file. """
    print("Processing test predictions")
    deepssm_dir = get_deepssm_dir(project)
    subjects = project.get_subjects()
    project_path = project.get_project_path() + "/"

    with open(config_file) as json_file:
        parameters = json.load(json_file)
        model_dir = parameters["paths"]["out_dir"] + parameters["model_name"] + '/'
        model_name = parameters["model_name"]

    pred_dir = f"{deepssm_dir}/{model_name}/test_predictions/"
    world_predictions_dir = f"{pred_dir}/world_predictions/"
    local_predictions_dir = f"{pred_dir}/local_predictions/"
    if not os.path.exists(local_predictions_dir):
        os.makedirs(local_predictions_dir)
    if not os.path.exists(world_predictions_dir):
        os.makedirs(world_predictions_dir)

    reference_index = sw.utils.get_reference_index(project)
    template_mesh = project_path + subjects[reference_index].get_groomed_filenames()[0]
    template_particles = project_path + subjects[reference_index].get_local_particle_filenames()[0]

    test_indices = get_split_indices(project, "test")

    predicted_test_local_particles = []
    predicted_test_world_particles = []
    test_transforms = []
    test_mesh_files = []

    for index in test_indices:
        world_particle_file = f"{world_predictions_dir}/{index}.particles"
        print(f"world_particle_file: {world_particle_file}")
        predicted_test_world_particles.append(world_particle_file)

        transform = get_test_alignment_transform(project, index)

        original_filenames = subjects[index].get_original_filenames()
        if len(original_filenames) > 0:
            test_mesh_files.append(project_path + subjects[index].get_original_filenames()[0])
        else:
            test_mesh_files.append("")

        particles = np.loadtxt(world_particle_file)
        local_particle_file = world_particle_file.replace("world_predictions/", "local_predictions/")
        local_particles = sw.utils.transformParticles(particles, transform, inverse=True)
        np.savetxt(local_particle_file, local_particles)
        predicted_test_local_particles.append(local_particle_file)

    distances = eval_utils.get_mesh_distances(predicted_test_local_particles, test_mesh_files,
                                              template_particles, template_mesh, pred_dir)

    print("Distances: ", distances)
    print("Mean distance: ", np.mean(distances))

    # write to csv file in deepssm_dir
    csv_file = f"{deepssm_dir}/test_distances.csv"
    with open(csv_file, "w") as f:
        f.write("Name,Distance\n")
        for i in range(len(test_indices)):
            display_name = subjects[test_indices[i]].get_display_name()
            distance = distances[i]
            if display_name == "":
                display_name = f"{test_indices[i]}"
            if distance < 0:
                distance = "N/A"
            f.write(f"{display_name},{distance}\n")

    # now transform the local meshes to the world space
    for index in test_indices:
        local_mesh_file = f"{pred_dir}/local_predictions/{index}.vtk"
        world_mesh_file = f"{pred_dir}/world_predictions/{index}.vtk"
        transform = get_test_alignment_transform(project, index)
        mesh = sw.Mesh(local_mesh_file)
        mesh.applyTransform(transform)
        mesh.write(world_mesh_file)
