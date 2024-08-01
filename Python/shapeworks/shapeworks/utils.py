import numpy as np
import os
import sys
import vtk
import glob
import shutil
import zipfile
import shapeworks as sw

# Global shapeworks logger object (e.g. attached to Studio)
sw_logger = None

sw_warp = None


# helper function to determine the best grid size (rows and columns) given the number of samples in a dataset.
def positive_factors(num_samples):
    factors = []

    for whole_number in range(1, num_samples + 1):
        if num_samples % whole_number == 0:
            factors.append(whole_number)

    return factors


def num_subplots(num_samples):
    factors = positive_factors(num_samples)
    cols = min(int(np.ceil(np.sqrt(num_samples))), max(factors))
    rows = int(np.ceil(num_samples / cols))

    return rows, cols


# a helper function that saves a list of shapeworks images in a directory
# this could be used to save final and intermediate results (if needed)
def save_images(outDir,  # path to the directory where we want to save the images
                swImageList,  # list of shapeworks images to be saved
                swImageNames,  # list of image names to be used as filenames
                extension='nrrd',
                compressed=True,  # use false to load in paraview
                verbose=True):
    if (len(swImageList) != len(swImageNames)):
        print('swImageNames list is not consistent with number of images in swImageList')
        return

    # create the output directory in case it does not exist
    if not os.path.exists(outDir):
        os.makedirs(outDir)

    filenames = []
    for curImg, curName in zip(swImageList, swImageNames):
        filename = outDir + curName + '.' + extension
        if verbose:
            print('Writing: ' + filename)
        if not os.path.exists(os.path.dirname(filename)):
            os.makedirs(os.path.dirname(filename))
        curImg.write(filename, compressed=compressed)
        filenames.append(filename)
    return filenames


# a helper function that saves a list of shapeworks meshes in a directory
# this could be used to save final and intermediate results (if needed)
def save_meshes(outDir,  # path to the directory where we want to save the images
                swMeshList,  # list of shapeworks images to be saved
                swMeshNames,  # list of image names to be used as filenames
                extension='ply',
                compressed=True,
                verbose=True):
    if (len(swMeshList) != len(swMeshNames)):
        print('swMeshNames list is not consistent with number of images in swMeshList')
        return

    # create the output directory in case it does not exist
    if not os.path.exists(outDir):
        os.makedirs(outDir)

    filenames = []
    for curMesh, curName in zip(swMeshList, swMeshNames):
        filename = outDir + curName + '.' + extension
        if verbose:
            print('Writing: ' + filename)
        if not os.path.exists(os.path.dirname(filename)):
            os.makedirs(os.path.dirname(filename))
        curMesh.write(filename)
        filenames.append(filename)
    return filenames


# a helper function to get list of files with specific extensions from a given file list
def get_file_with_ext(file_list, extension):
    extList = []
    for file in file_list:
        ext = file.split(".")[-1]
        if (ext == extension):
            extList.append(file)
    extList = sorted(extList)
    return extList


# a helper function to convert absolute paths to a relative path from a base dir
def get_relative_paths(files, base):
    relative_paths = []
    for f in files:
        path = os.path.relpath(f, base)
        relative_paths.append(path)
    return relative_paths


def find_reference_image_index(inDataList, domains_per_shape=1):
    mesh_list = []
    for img in inDataList:
        mesh = img.toMesh(0.5)
        mesh_list.append(mesh)
    return find_reference_mesh_index(mesh_list, domains_per_shape)


def find_reference_mesh_index(mesh_list, domains_per_shape=1):
    if (domains_per_shape == 1):
        return sw.MeshUtils.findReferenceMesh(mesh_list)
    else:
        combined_mesh = sw.data.combine_domains(mesh_list, domains_per_shape)
        index = sw.MeshUtils.findReferenceMesh(combined_mesh)
        return index, combined_mesh


def save_contour_as_vtp(points, lines, filename):
    """
    Generates a .vtp file for the given contour to use in ShapeWorks optimizer

    points:   Nx3 np.ndarray of points in the contour
    lines:    Mx2 np.ndarray of lines in the contour
    filename: output .vtp filename
    """
    vtk_pts = vtk.vtkPoints()
    n = points.shape[0]
    for j in range(n):
        x, y, z = points[j]
        vtk_pts.InsertNextPoint((x, y, z))

    vtk_lines = vtk.vtkCellArray()
    m = lines.shape[0]
    for j in range(m):
        vtk_line = vtk.vtkLine()
        vtk_line.GetPointIds().SetId(0, lines[j][0])
        vtk_line.GetPointIds().SetId(1, lines[j][1])
        vtk_lines.InsertNextCell(vtk_line)

    polydata = vtk.vtkPolyData()
    polydata.SetPoints(vtk_pts)
    polydata.SetLines(vtk_lines)

    writer = vtk.vtkXMLPolyDataWriter()
    writer.SetFileName(filename)
    writer.SetInputData(polydata)
    writer.Write()


def compute_line_indices(n, is_closed=True):
    """
    Given a number of points, return indices for lines(as np.ndarray) between successive pairs of points.

    n:         number of points
    is_closed: whether or not the last vertex is to to be connected to the first vertex
    """
    lines = np.zeros((n if is_closed else n - 1, 2), dtype=int)
    for i in range(lines.shape[0]):
        lines[i] = [i, (i + 1) % n]

    return lines


def get_api_version():
    return "6.6"


def set_sw_logger(log_object):
    """Set the shapeworks logger object"""
    global sw_logger
    sw_logger = log_object


def sw_message(str):
    """If sw_logger is set, use it to log a message, otherwise print to console"""
    global sw_logger
    if sw_logger is not None:
        sw_logger.log(str)
    else:
        print(str)


def sw_check_abort():
    """If sw_logger is set, use it to check if abort has been called"""
    global sw_logger
    if sw_logger is not None:
        return sw_logger.check_abort()
    else:
        return False


def sw_progress(progress, message=""):
    """If sw_logger is set, use it, otherwise do nothing"""
    global sw_logger
    if sw_logger is not None:
        sw_logger.progress(progress, message)


def test(name, failure=False):
    if failure:
        try:
            if name():
                print(name.__name__ + " failed")
                return False
            else:
                return True
        except Exception as e:
            print(name.__name__ + " unexpected failure (exception): " + str(e))
            return False
    else:
        try:
            if name():
                return True
            else:
                print(name.__name__ + " failed")
                return False
        except Exception as e:
            print(name.__name__ + " failed (exception): " + str(e))
            return False


def expectException(name, etype):
    try:
        name()
        print(name.__name__ + " failed (expected an exception)")
        return False
    except etype:
        return True
    except Exception as e:
        print(name.__name__ + " failed (expected a different kind of exception): " + str(e))
        return False


def getVTKtransform(itkTransform):
    flippedITKtransform = np.copy(itkTransform)
    lastColumn = itkTransform[:, -1]
    lastRow = itkTransform[-1, :]
    flippedITKtransform[:, -1] = lastRow
    flippedITKtransform[-1, :] = lastColumn
    vtkTransform = np.linalg.inv(flippedITKtransform)

    return vtkTransform


def getITKtransform(vtkTransform):
    inverseVTKtransform = np.linalg.inv(vtkTransform)
    itkTransform = np.copy(inverseVTKtransform)
    lastColumn = inverseVTKtransform[:, -1]
    lastRow = inverseVTKtransform[-1, :]
    itkTransform[:, -1] = lastRow
    itkTransform[-1, :] = lastColumn

    return itkTransform


def verify(args, world_point_files):
    # compare against baseline particles
    files = world_point_files
    ps1 = sw.ParticleSystem(files)

    print(f"Comparing results in:")
    for file in files:
        print(file)

    # Copy to results dir
    check_dir = f"Output/Results/{args.use_case.lower()}/{args.option_set}/"
    if not os.path.exists(check_dir):
        os.makedirs(check_dir)

    for file in files:
        shutil.copy(file, check_dir)

    verification_dir = f"Data/Verification/{args.use_case.lower()}/{args.option_set}/"

    baseline = [verification_dir + os.path.basename(f) for f in files]

    print(f"\nBaseline:")
    for file in baseline:
        print(file)

    for file in baseline:
        if not os.path.exists(file):
            print(f"Error: baseline file {file} does not exist")
            return False

    ps2 = sw.ParticleSystem(baseline)

    if not ps1.EvaluationCompare(ps2):
        print("Error: particle system did not match ground truth")
        return False
    return True


def check_results(args, project_spreadsheet):
    # If tiny test or verify, check results and exit
    particle_dir = project_spreadsheet.replace(".xlsx", "_particles/").replace(".swproj", "_particles/")
    world_point_files = []
    for file in sorted(os.listdir(particle_dir)):
        if "world" in file:
            world_point_files.append(particle_dir + file)
    if args.tiny_test or args.verify:
        # if verification dir doesn't exist, unzip verification.zip
        if not os.path.exists("Data/Verification"):
            print("Unzipping verification data")
            # unzip using python
            with zipfile.ZipFile("Data/Verification.zip", 'r') as zip_ref:
                zip_ref.extractall("Data/")

        print("Verifying shape model")
        if not verify(args, world_point_files):
            exit(-1)
        print("Done with test, verification succeeded.")
        exit()


def check_results_pattern(args, project_spreadsheet, pattern):
    # If tiny test or verify, check results and exit
    particle_dir = project_spreadsheet.replace(".xlsx", "_particles/").replace(".swproj", "_particles/")
    world_point_files = []
    for file in sorted(os.listdir(particle_dir)):
        if pattern in file:
            world_point_files.append(particle_dir + file)
    if args.tiny_test or args.verify:
        print("Verifying shape model")
        if not verify(args, world_point_files):
            exit(-1)
        print("Done with test, verification succeeded.")
        exit()


def findMeanShape(shapeModelDir):
    fileList = sorted(glob.glob(shapeModelDir + '/*local.particles'))
    for i in range(len(fileList)):
        if i == 0:
            meanShape = np.loadtxt(fileList[i])
        else:
            meanShape += np.loadtxt(fileList[i])
    meanShape = meanShape / len(fileList)
    nmMS = shapeModelDir + '/meanshape_local.particles'
    np.savetxt(nmMS, meanShape)


def transformParticles(particles, transform, inverse=False):
    if inverse:
        transform = np.linalg.inv(transform)
    transformed_particles = []
    for particle in particles:
        transformed_particles.append(np.matmul(transform, np.append(particle, 1))[:3])
    transformed_particles = np.array(transformed_particles)
    return transformed_particles


def load_mesh(filename):
    accepted_extensions = ['.vtk', '.vtp', '.ply', '.obj', '.stl']
    extension = os.path.splitext(filename)[1]
    if extension in accepted_extensions:
        return sw.Mesh(filename)
    else:
        return sw.Image(filename).toMesh(0.5)


def initialize_mesh_warper(template_mesh, template_particles):
    """
    This function initializes a MeshWarper object using a template mesh and corresponding particles.

    Parameters:
    template_mesh_filename (shapeworks.Mesh): The template mesh.
    template_particles (numpy_ndarray): The template particles.

    Returns:
    None
    """
    global sw_warp
    sw_warp = sw.MeshWarper()
    sw_warp.generateWarp(template_mesh, template_particles)


def initialize_mesh_warper_from_files(template_mesh_filename, template_particles):
    """
    This function initializes a MeshWarper object using a template mesh and corresponding particles.

    Parameters:
    template_mesh_filename (str): The filename of the template mesh.
    template_particles (str): The filename of the corresponding particles.

    Returns:
    None
    """
    sw_mesh = get_mesh_from_file(template_mesh_filename)
    sw_particles = np.loadtxt(template_particles)
    initialize_mesh_warper(sw_mesh, sw_particles)


def initialize_project_mesh_warper(project):
    """
    This function initializes a MeshWarper object using the template mesh and particles from a given project.

    Parameters:
    project (shapeworks.Project): The project to be used for mesh warping.

    Returns:
    None
    """
    subjects = project.get_subjects()
    project_path = project.get_project_path() + "/"
    reference_index = sw.utils.get_reference_index(project)
    template_mesh = project_path + subjects[reference_index].get_groomed_filenames()[0]
    template_particles = project_path + subjects[reference_index].get_local_particle_filenames()[0]
    initialize_mesh_warper_from_files(template_mesh, template_particles)


def reconstruct_mesh(particles):
    """
    This function uses the global MeshWarper object to build a mesh from a given set of particles.

    Parameters:
    particles (numpy.ndarray): The particles to be used for mesh reconstruction.

    Returns:
    shapeworks.Mesh: The reconstructed mesh.

    Raises:
    Exception: If the MeshWarper object has not been initialized.
    """
    if sw_warp is None:
        raise Exception("Mesh warper has not been initialized")
    return sw_warp.buildMesh(particles)


def get_mesh_from_DT(DT_list, mesh_dir):
    if not os.path.exists(mesh_dir):
        os.makedirs(mesh_dir)
    mesh_files = []
    for input_file in DT_list:
        print('	' + get_prefix(input_file))
        output_vtk = mesh_dir + "original_" + get_prefix(input_file) + ".vtk"
        image = sw.Image(input_file)
        image.toMesh(isovalue=0).write(output_vtk)
        mesh_files.append(output_vtk)
    return sorted(mesh_files)


def get_mesh_from_file(filename, iso_value=0):
    mesh_types = ['.vtk', '.vtp', '.ply', '.obj', '.stl']

    is_mesh = False
    for ext in mesh_types:
        if filename.lower().endswith(ext):
            is_mesh = True
            break

    if is_mesh:
        return sw.Mesh(filename)

    image = sw.Image(filename)
    return image.toMesh(iso_value)


def get_reference_index(project):
    """ Get the index of the reference subject chosen by grooming alignment."""
    params = project.get_parameters("groom")
    reference_index = params.get("alignment_reference_chosen")
    return int(reference_index)


def get_image_filename(subject):
    """ Get the image filename for a subject. """
    image_map = subject.get_feature_filenames()
    # get the first image
    image_name = list(image_map.values())[0]
    return image_name
