import itk
import SimpleITK
import numpy as np


def clamp_translation(result_transform_parameters, max_translation):
    clamped_params = list(result_transform_parameters)

    if len(clamped_params) == 6:
        translation_indices = range(len(clamped_params) - 3, len(clamped_params))
    elif len(clamped_params) == 3:
        translation_indices = range(len(clamped_params))
    elif len(clamped_params) == 7:  # similarity
        translation_indices = range(len(clamped_params) - 4, len(clamped_params) - 1)
    else:
        raise ValueError(f"Unexpected number of parameters: {len(clamped_params)}")

    for i in translation_indices:
        clamped_params[i] = max(min(clamped_params[i], max_translation), -max_translation)
    return tuple(clamped_params)


def get_image_registration_transform(fixed_image_file, moving_image_file, transform_type='rigid',
                                     max_translation=None, max_rotation=None, max_iterations=2048):
    print(f"\nRunning image registration with transform type {transform_type}")
    # Prepare parameter map
    parameter_object = itk.ParameterObject.New()
    parameter_map = parameter_object.GetDefaultParameterMap('rigid')

    # Set specific transform type in parameter map
    if transform_type == 'similarity':
        parameter_map['Transform'] = ['SimilarityTransform']
    elif transform_type == 'translation':
        parameter_map['Transform'] = ['TranslationTransform']

    if max_iterations is None:
        max_iterations = 1024

    parameter_map['MaximumNumberOfIterations'] = [str(max_iterations)]

    # if max_rotation is not None and transform_type in ['rigid', 'similarity']:
    #     # Assume rotation is in degrees and we limit optimization scales
    #     rotation_constraints = [str(np.deg2rad(max_rotation))] * (3 if transform_type == 'rigid' else 6)
    #     parameter_map['OptimizerScales'] = rotation_constraints

    parameter_object.AddParameterMap(parameter_map)

    # Load images
    fixed_image = itk.imread(fixed_image_file, itk.F)
    moving_image = itk.imread(moving_image_file, itk.F)

    # Call registration method
    result_image, result_transform_parameters = itk.elastix_registration_method(
        fixed_image, moving_image, parameter_object=parameter_object)

    # Get transform matrix
    parameter_map = result_transform_parameters.GetParameterMap(0)
    transform_params = np.array(parameter_map['TransformParameters'], dtype=float)

    if max_translation is not None:
        clamped_params = clamp_translation(transform_params, max_translation)
        transform_params = clamped_params

    if transform_type == 'rigid':
        itk_transform = SimpleITK.Euler3DTransform()
    elif transform_type == 'similarity':
        itk_transform = SimpleITK.Similarity3DTransform()
    elif transform_type == 'translation':
        itk_transform = SimpleITK.TranslationTransform(3)
    else:
        raise NotImplementedError("Error: " + transform_type + " transform unimplemented.")
    itk_transform.SetParameters(transform_params)
    itk_transform_matrix = np.eye(4)

    if transform_type == 'translation':
        itk_transform_matrix[-1, :3] = np.array(itk_transform.GetOffset())
    else:
        itk_transform_matrix[:3, :3] = np.array(itk_transform.GetMatrix()).reshape(3, 3)
        itk_transform_matrix[-1, :3] = np.array(itk_transform.GetTranslation())

    return itk_transform_matrix


def get_image_registration_transform_old(fixed_image_file, moving_image_file, transform_type='rigid'):
    # Prepare parameter map 
    parameter_object = itk.ParameterObject.New()
    parameter_map = parameter_object.GetDefaultParameterMap('rigid')
    if transform_type == 'similarity':
        parameter_map['Transform'] = ['SimilarityTransform']
    elif transform_type == 'translation':
        parameter_map['Transform'] = ['TranslationTransform']
    parameter_map['MaximumNumberOfIterations'] = ['1024']
    parameter_object.AddParameterMap(parameter_map)

    # Load images
    fixed_image = itk.imread(fixed_image_file, itk.F)
    moving_image = itk.imread(moving_image_file, itk.F)

    # Call registration method
    result_image, result_transform_parameters = itk.elastix_registration_method(
        fixed_image, moving_image, parameter_object=parameter_object)

    # Get transform matrix
    parameter_map = result_transform_parameters.GetParameterMap(0)
    transform_params = np.array(parameter_map['TransformParameters'], dtype=float)
    if transform_type == 'rigid':
        itk_transform = SimpleITK.Euler3DTransform()
    elif transform_type == 'similarity':
        itk_transform = SimpleITK.Similarity3DTransform()
    elif transform_type == 'translation':
        itk_transform = SimpleITK.TranslationTransform(3)
    else:
        raise NotImplementedError("Error: " + transform_type + " transform unimplemented.")
    itk_transform.SetParameters(transform_params)
    itk_transform_matrix = np.eye(4)
    if transform_type == 'translation':
        itk_transform_matrix[-1, :3] = np.array(itk_transform.GetOffset())
    else:
        itk_transform_matrix[:3, :3] = np.array(itk_transform.GetMatrix()).reshape(3, 3)
        itk_transform_matrix[-1, :3] = np.array(itk_transform.GetTranslation())
    return itk_transform_matrix
