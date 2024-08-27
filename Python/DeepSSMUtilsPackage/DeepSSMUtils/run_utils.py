import random
import math
import os
import numpy as np
import json

import shapeworks as sw
from bokeh.util.terminal import trace
from shapeworks.utils import sw_message, sw_progress, sw_check_abort

import DataAugmentationUtils
import DeepSSMUtils
import DeepSSMUtils.eval_utils as eval_utils

import itk
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


def groom_training_images(project):
    """ Groom the training images """
    subjects = project.get_subjects()

    deepssm_dir = get_deepssm_dir(project)

    ref_index = sw.utils.get_reference_index(project)
    ref_image = sw.Image(sw.utils.get_image_filename(subjects[ref_index]))
    ref_mesh = sw.utils.load_mesh(subjects[ref_index].get_groomed_filenames()[0])

    # apply alignment transform
    alignment = convert_transform_to_numpy(subjects[ref_index].get_groomed_transforms()[0])
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
    ref_image.applyTransform(reflection)
    params = project.get_parameters("deepssm")

    spacing = params.get("spacing")
    if (spacing == ""):
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
    for i in get_training_indices(project):

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
        image.write(deepssm_dir + f"/train_images/{i}.nrrd")


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
        train_image_filenames.append(f"deepssm/train_images/{i}.nrrd")
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


def groom_val_test_images(project, indices):
    """ Groom the validation and test images """
    subjects = project.get_subjects()
    deepssm_dir = get_deepssm_dir(project)

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
    val_test_images_dir = deepssm_dir + 'val_and_test_images/'
    if not os.path.exists(val_test_images_dir):
        os.makedirs(val_test_images_dir)

    val_test_indices = indices

    val_test_transforms = []
    val_test_image_files = []

    count = 1
    for i in val_test_indices:
        if sw_check_abort():
            sw_message("Aborted")
            return

        image_name = sw.utils.get_image_filename(subjects[i])
        sw_progress(count / (len(val_test_indices) + 1),
                    f"Grooming val/test image {image_name} ({count}/{len(val_test_indices)})")
        count = count + 1
        image = sw.Image(image_name)

        image_file = val_test_images_dir + f"{i}.nrrd"

        # check if this subject needs reflection
        needs_reflection, axis = does_subject_need_reflection(project, subjects[i])

        # 1. Apply reflection
        reflection = np.eye(4)
        if needs_reflection:
            reflection[axis, axis] = -1
            # account for offset
            reflection[-1][0] = 2 * image.center()[0]

        image.applyTransform(reflection)
        transform = sw.utils.getVTKtransform(reflection)

        # 2. Translate to have ref center to make rigid registration easier
        translation = ref_center - image.center()
        image.setOrigin(image.origin() + translation).write(image_file)
        transform[:3, -1] += translation

        # 3. Translate with respect to slightly cropped ref
        image = sw.Image(image_file).fitRegion(large_bb).write(image_file)
        itk_translation_transform = DeepSSMUtils.get_image_registration_transform(large_cropped_ref_image_file,
                                                                                  image_file,
                                                                                  transform_type='translation')
        # 4. Apply transform
        image.applyTransform(itk_translation_transform,
                             large_cropped_ref_image.origin(), large_cropped_ref_image.dims(),
                             large_cropped_ref_image.spacing(), large_cropped_ref_image.coordsys(),
                             sw.InterpolationType.Linear, meshTransform=False)
        vtk_translation_transform = sw.utils.getVTKtransform(itk_translation_transform)
        transform = np.matmul(vtk_translation_transform, transform)

        # 5. Crop with medium bounding box and find rigid transform
        image.fitRegion(medium_bb).write(image_file)
        itk_rigid_transform = DeepSSMUtils.get_image_registration_transform(medium_cropped_ref_image_file,
                                                                            image_file, transform_type='rigid')

        # 6. Apply transform
        image.applyTransform(itk_rigid_transform,
                             medium_cropped_ref_image.origin(), medium_cropped_ref_image.dims(),
                             medium_cropped_ref_image.spacing(), medium_cropped_ref_image.coordsys(),
                             sw.InterpolationType.Linear, meshTransform=False)
        vtk_rigid_transform = sw.utils.getVTKtransform(itk_rigid_transform)
        transform = np.matmul(vtk_rigid_transform, transform)

        # 7. Get similarity transform from image registration and apply
        image.fitRegion(bounding_box).write(image_file)
        itk_similarity_transform = DeepSSMUtils.get_image_registration_transform(cropped_ref_image_file,
                                                                                 image_file,
                                                                                 transform_type='similarity')
        image.applyTransform(itk_similarity_transform,
                             cropped_ref_image.origin(), cropped_ref_image.dims(),
                             cropped_ref_image.spacing(), cropped_ref_image.coordsys(),
                             sw.InterpolationType.Linear, meshTransform=False)
        image.write(image_file)
        vtk_similarity_transform = sw.utils.getVTKtransform(itk_similarity_transform)
        transform = np.matmul(vtk_similarity_transform, transform)

        # 8. Save transform
        val_test_transforms.append(transform)
        extra_values = subjects[i].get_extra_values()
        extra_values["registration_transform"] = transform_to_string(transform)

        subjects[i].set_extra_values(extra_values)
    project.set_subjects(subjects)


def prepare_data_loaders(project, batch_size, split="all"):
    """ Prepare PyTorch laoders """
    deepssm_dir = get_deepssm_dir(project)
    loader_dir = deepssm_dir + 'torch_loaders/'
    if not os.path.exists(loader_dir):
        os.makedirs(loader_dir)

    if split == "all" or split == "val":
        val_image_files = []
        val_world_particles = []
        val_indices = get_split_indices(project, "val")
        for i in val_indices:
            val_image_files.append(deepssm_dir + f"/val_and_test_images/{i}.nrrd")
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
            test_image_files.append(deepssm_dir + f"/val_and_test_images/{i}.nrrd")
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
