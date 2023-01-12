import numpy as np
import os
import sys
import vtk
import glob
import shutil
import shapeworks as sw

# Global shapeworks logger object (e.g. attached to Studio)
sw_logger = None

# helper function to determine the best grid size (rows and columns) given the number of samples in a dataset.
def postive_factors(num_samples):
    factors = []

    for whole_number in range(1, num_samples + 1):
        if num_samples % whole_number == 0:
            factors.append(whole_number)

    return factors

def num_subplots(num_samples):
    factors = postive_factors(num_samples)
    cols    = min(int(np.ceil(np.sqrt(num_samples))),max(factors))
    rows    = int(np.ceil(num_samples/cols))

    return rows, cols

# a helper function that saves a list of shapeworks images in a directory
# this could be used to save final and intermediate results (if needed)
def save_images(outDir,        # path to the directory where we want to save the images
                swImageList,   # list of shapeworks images to be saved
                swImageNames,  # list of image names to be used as filenames
                extension        = 'nrrd',
                compressed       = True, # use false to load in paraview
                verbose          = True):

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
def save_meshes(outDir,        # path to the directory where we want to save the images
                swMeshList,   # list of shapeworks images to be saved
                swMeshNames,  # list of image names to be used as filenames
                extension        = 'ply',
                compressed       = True,
                verbose          = True):

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
def get_file_with_ext(file_list,extension):

    extList =[]
    for file in file_list:
        ext = file.split(".")[-1]
        if(ext==extension):
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
        
def find_reference_image_index(inDataList,domains_per_shape=1):
    mesh_list = []
    for img in inDataList:
        mesh = img.toMesh(0.5)
        mesh_list.append(mesh)
    return find_reference_mesh_index(mesh_list, domains_per_shape)

def find_reference_mesh_index(mesh_list,domains_per_shape=1):
    if(domains_per_shape==1):
        return sw.MeshUtils.findReferenceMesh(mesh_list)
    else:
        combined_mesh = sw.data.combine_domains(mesh_list,domains_per_shape)
        index = sw.MeshUtils.findReferenceMesh(combined_mesh) 
        return index,combined_mesh


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
        vtk_pts.InsertNextPoint((x,y,z))

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
    lines = np.zeros((n if is_closed else n-1, 2), dtype=int)
    for i in range(lines.shape[0]):
        lines[i] = [i, (i+1)%n]

    return lines

def get_api_version():
    return "6.4"

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

def sw_progress(progress):
    """If sw_logger is set, use it, otherwise do nothing"""
    global sw_logger
    if sw_logger is not None:
        sw_logger.progress(progress)

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
    lastColumn = itkTransform[:,-1]
    lastRow = itkTransform[-1,:]
    flippedITKtransform[:,-1] = lastRow
    flippedITKtransform[-1,:] = lastColumn
    vtkTransform = np.linalg.inv(flippedITKtransform)

    return vtkTransform

def getITKtransform(vtkTransform):
    
    inverseVTKtransform = np.linalg.inv(vtkTransform)
    itkTransform = np.copy(inverseVTKtransform)
    lastColumn = inverseVTKtransform[:,-1]
    lastRow = inverseVTKtransform[-1,:]
    itkTransform[:,-1] = lastRow
    itkTransform[-1,:] = lastColumn

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
        print("Verifying shape model")
        if not verify(args, world_point_files):
            exit(-1)
        print("Done with test, verification succeeded.")
        exit()

def check_results_pattern(args, project_spreadsheet, pattern):
    # If tiny test or verify, check results and exit
    particle_dir = project_spreadsheet.replace(".xlsx", "_particles/")
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
