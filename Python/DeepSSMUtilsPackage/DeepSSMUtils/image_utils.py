import itk
import SimpleITK
import numpy as np

def get_image_registration_transform(fixed_image_file, moving_image_file, transform_type='rigid'):
    # Import Default Parameter Map 
    parameter_object = itk.ParameterObject.New()
    parameter_map = parameter_object.GetDefaultParameterMap('rigid')
    if transform_type == 'similarity':
        parameter_map['Transform'] = ['SimilarityTransform']
    elif transform_type == 'translation':
        parameter_map['Transform'] = ['TranslationTransform']
    parameter_object.AddParameterMap(parameter_map)
    # Call registration function
    fixed_image = itk.imread(fixed_image_file, itk.F)
    moving_image = itk.imread(moving_image_file, itk.F)
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
        itk_transform = SimpleITK.TranslationTransform()
    else:
        print("Error: " + transform_type + " transform unimplemented.")
    itk_transform.SetParameters(transform_params)
    itk_transform_matrix = np.eye(4)
    itk_transform_matrix[:3,:3] = np.array(itk_transform.GetMatrix()).reshape(3,3)
    itk_transform_matrix[-1,:3] = np.array(itk_transform.GetTranslation())
    return itk_transform_matrix