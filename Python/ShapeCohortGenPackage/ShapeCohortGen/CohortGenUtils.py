import os
import numpy as np
import scipy
import shutil
import vtk
from vtk.util.numpy_support import vtk_to_numpy
import shapely
import matplotlib.pyplot as plt
from shapeworks import *

'''
Make folder
'''
def make_dir(dir_path):
    if os.path.exists(dir_path):
        shutil.rmtree(dir_path)
    os.makedirs(dir_path)

'''
Get list of full paths for files in dir
'''
def get_files(folder):
    file_list = []
    for file in os.listdir(folder):
        file_path = folder + file
        file_path = file_path.replace(" ","")
        file_list.append(file_path)
    file_list = sorted(file_list)
    return file_list

'''
Get files with specific extensions
'''
def get_file_with_ext(file_list,extension):
    extList =[]
    for file in file_list:
        ext = file.split(".")[-1]
        if(ext==extension):
            extList.append(file)
    extList = sorted(extList)
    return extList

'''
Takes inname path and replaces dir with outdir and adds extension before file type
'''
def rename(inname, outDir, extension_addition, extension_change=''):
    initPath = os.path.dirname(inname)
    outname = inname.replace(initPath, outDir)
    current_extension = "." + inname.split(".")[-1]
    if extension_addition != '':
        outname = outname.replace(current_extension, '.' + extension_addition + current_extension)
    if extension_change != '':
        outname = outname.replace(current_extension, extension_change)
    return outname

'''
Generate segmentations from mesh list:
 - by default these will be the size of the region containing ALL meshes (with some padding)
 - if allow_on_boundary, a random subset of these will be exactly the size of the mesh
 - if randomize_size, meshes not in allow_on_boundary subset will be ALL mesh region with different padding
 - this is an example of using Mesh.toImage, not a useful tool
'''
def generate_segmentations(meshList, out_dir, randomize_size=True, spacing=[1.0,1.0,1.0], allow_on_boundary=True):

    # get list of meshs to be converted
    segDir = out_dir + "segmentations/"
    make_dir(segDir)

    # get region that includes all of these meshes
    bball = MeshUtils.boundingBox(meshList)

    # randomly select 20% meshes for boundary touching samples
    numMeshes = len(meshList)
    meshIndexArray = np.array(list(range(numMeshes)))
    subSampleSize = int(0.2*numMeshes)
    randomBoundarySamples = np.random.choice(meshIndexArray,subSampleSize,replace=False)

    # loop through meshes and turn to images
    segList = []
    meshIndex = 0
    for mesh_ in meshList:
        print("Generating seg " + str(meshIndex + 1) + " out of " + str(len(meshList)))
        segFile = rename(mesh_, segDir, "", ".nrrd")
        segList.append(segFile)

        # load .ply mesh and get its bounding box
        mesh = Mesh(mesh_)
        bb = mesh.boundingBox()

        # if mesh isn't in the set for allow_on_boundary, add [random] padding
        if not (allow_on_boundary and (meshIndex in randomBoundarySamples)):
            bb = bball

            pad = 5
            if randomize_size:            
                pad = np.random.randint(5, high=15, size=3)
            else:
                pad = np.array([5,5,5])
            bb.min -= pad
            bb.max += pad

        # sample the given region of Mesh to an image
        image = mesh.toImage(region=bb, spacing=spacing)

        # write the result to disk and move to the next mesh
        image.write(segFile, compressed=True)
        meshIndex += 1

    # return list of new image filenames
    return segList

'''
Generate 2D segmentations from contour list:
 - by default these will be the size of the region containing ALL contours (with some padding)
 - if allow_on_boundary, a random subset of these will be exactly the size of the contour
 - if randomize_size, contours not in allow_on_boundary subset will be ALL contour region with different padding
'''
def generate_2Dsegmentations(contour_list, out_dir, randomize_size=True, spacing=[1.0,1.0,1.0], allow_on_boundary=True):

    # get list of meshs to be converted
    segDir = out_dir + "segmentations/"
    make_dir(segDir)

    # get region that includes all of these contours
    bball = get_contours_bounding_box(contour_list)

    # randomly select 20% contours for boundary touching samples
    num_contours = len(contour_list)
    contourIndexArray = np.array(list(range(num_contours)))
    subSampleSize = int(0.2*num_contours)
    randomBoundarySamples = np.random.choice(contourIndexArray,subSampleSize,replace=False)

    # loop through meshes and turn to images
    segList = []
    contourIndex = 0
    for contour in contour_list:
        print("Generating seg " + str(contourIndex + 1) + " out of " + str(len(contour_list)))
        segFile = rename(contour, segDir, "", ".png")
        segList.append(segFile)

        reader = vtk.vtkXMLPolyDataReader()
        reader.SetFileName(contour)
        reader.Update()
        polydata = reader.GetOutput()
        points = polydata.GetPoints()
        array = points.GetData()
        point_coordinates = vtk_to_numpy(array)

        bb = [np.max(point_coordinates[:,0]), np.min(point_coordinates[:,0]), np.max(point_coordinates[:,1]), np.min(point_coordinates[:,1])]
        if not (allow_on_boundary and (contourIndex in randomBoundarySamples)):
            bb = bball
            padx = 5
            pady = 5
            if randomize_size:            
                padx = np.random.randint(5, high=15)
                pady = np.random.randint(5, high=15)
            bb[0] += padx
            bb[1] -= padx
            bb[2] += pady
            bb[3] -= pady

        poly = list()
        for ii in range(point_coordinates.shape[0]):
            poly.append((point_coordinates[ii,0], point_coordinates[ii,1]))
        polygon = shapely.geometry.Polygon(poly)

        X = np.arange(bb[1], bb[0])
        Y = np.arange(bb[3], bb[2])
        mask = np.zeros((Y.shape[0],X.shape[0]))
        for ii in range(X.shape[0]):
            for jj in range(Y.shape[0]):
                point = shapely.geometry.Point(X[ii], Y[jj])
                if polygon.contains(point):
                    mask[jj,ii] = 1

        plt.imsave(segFile, mask, cmap="gray")
        contourIndex += 1

    # return list of new image filenames
    return segList

'''
Returns smallest bounding box that contains all contours
'''
def get_contours_bounding_box(contour_list, pad=5):
    max_x = 0
    max_y = 0
    min_x = np.inf 
    min_y = np.inf
    for contour in contour_list:
        reader = vtk.vtkXMLPolyDataReader()
        reader.SetFileName(contour)
        reader.Update()
        polydata = reader.GetOutput()
        points = polydata.GetPoints()
        array = points.GetData()
        point_coordinates = vtk_to_numpy(array)
        if max_x < np.max(point_coordinates[:,0]):
            max_x = np.max(point_coordinates[:,0])
        if min_x > np.min(point_coordinates[:,0]):
            min_x = np.min(point_coordinates[:,0])
        if max_y < np.max(point_coordinates[:,1]):
            max_y = np.max(point_coordinates[:,1])
        if min_y > np.min(point_coordinates[:,1]):
            min_y = np.min(point_coordinates[:,1])
    return [max_x, min_x, max_y, min_y]

'''
Generates image by blurring and adding noise to segmentation
'''
def generate_images(segs, outDir, blur_factor, foreground_mean, foreground_var, background_mean, background_var):
    imgDir = outDir + 'images/'
    make_dir(imgDir)
    index = 1
    for seg in segs:
        print("Generating image " + str(index) + " out of " + str(len(segs)))
        name = seg.replace('segmentations/','images/').replace('_seg.nrrd', '_blur' + str(blur_factor) + '.nrrd')
        img = Image(seg)
        img_array = blur(img.toArray(), blur_factor)
        img_array = apply_noise(img_array, foreground_mean, foreground_var, background_mean, background_var)
        img_array = np.float32(img_array)
        img = Image(np.float32(img_array))
        img.write(name,compressed=True)
        index += 1
    return get_files(imgDir)

'''
Generates image by blurring and adding noise to segmentation
'''
def generate_2Dimages(segs, outDir, blur_factor, foreground_mean, foreground_var, background_mean, background_var):
    imgDir = outDir + 'images/'
    make_dir(imgDir)
    index = 1
    for seg in segs:
        print("Generating image " + str(index) + " out of " + str(len(segs)))
        name = seg.replace('segmentations/','images/').replace('_seg.png', '_blur' + str(blur_factor) + '.png')
        img_array = plt.imread(seg, format="png")
        img_array = img_array/np.max(img_array)
        img_array = blur(img_array, blur_factor)
        img_array = apply_noise(img_array, foreground_mean, foreground_var, background_mean, background_var)
        img_array = img_array.astype(np.uint8)
        plt.imsave(name, img_array, cmap="gray")
        index += 1
    return get_files(imgDir)

'''
get_image helper
'''
def blur(img, size):
    blur = scipy.ndimage.filters.gaussian_filter(img, size)
    return blur

'''
get_image helper
'''
def apply_noise(img, foreground_mean, foreground_var, background_mean, background_var):
    background_indices = np.where(img < 0.5)
    foreground_indices = np.where(img > 0.5)
    img = img*(foreground_mean-background_mean)
    img = img + np.ones(img.shape)*background_mean
    foreground_noise = np.random.normal(0, foreground_var**0.5, img.shape)
    foreground_noise[background_indices] = 0
    background_noise = np.random.normal(0, background_var**0.5, img.shape)
    background_noise[foreground_indices] = 0
    noisy_img = img + foreground_noise + background_noise
    return noisy_img

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
