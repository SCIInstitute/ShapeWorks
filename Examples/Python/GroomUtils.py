import sys
import numpy as np
import io
from termcolor import colored, cprint
import glob
import os
import subprocess
import shutil
import xml.etree.ElementTree as ET
import itk
import vtk
import vtk.util.numpy_support

from shapeworks import *
from CommonUtils import *

def rename(inname, outDir, extension_addition, extension_change=''):
    """
    Takes inname path and replaces dir with outdir and adds extension before file type
    """
    initPath = os.path.dirname(inname)
    outname = inname.replace(initPath, outDir)
    current_extension = "." + inname.split(".")[-1]
    if extension_addition != '':
        outname = outname.replace(current_extension, '.' + extension_addition + current_extension)
    if extension_change != '':
        outname = outname.replace(current_extension, extension_change)
    cprint(("Input filename: " + inname), 'cyan')
    cprint(("Output filename: " + outname), 'yellow')
    return outname

def applyIsotropicResampling(outDir, inDataList, isoSpacing=1.0, isBinary=True):
    """
    This function takes in a filelist and produces the resampled files in the appropriate directory.
    """
    print("\n########### Isotropic Resampling ###############")
    if not os.path.exists(outDir):
        os.makedirs(outDir)
    outDataList = []
    for i in range(len(inDataList)):
        inname = inDataList[i]
        outname = rename(inname, outDir, 'isores')
        outDataList.append(outname)
        img = Image(inname)
        if isBinary:
            img.antialias()
        ImageUtils.isoresample(img, isoSpacing)
        if isBinary:
            img.binarize()
        img.write(outname)
    return outDataList

def center(outDir, inDataList):
    print("\n########### Centering ###############")
    if not os.path.exists(outDir):
        os.makedirs(outDir)
    outDataList = []
    for i in range(len(inDataList)):
        inname = inDataList[i]
        outname = rename(inname, outDir, 'center')
        outDataList.append(outname)
        img = Image(inname)
        img.recenter().write(outname)
    return outDataList

def applyPadding(outDir, inDataList, padSize, padValue=0):
    """
    This function takes in a filelist and produces the padded files in the appropriate directory.
    """
    print("\n########### Padding ###############")
    if not os.path.exists(outDir):
        os.makedirs(outDir)
    outDataList = []
    for i in range(len(inDataList)):
        inname = inDataList[i]
        outname = rename(inname, outDir, 'pad')
        outDataList.append(outname)
        img = Image(inname)
        img.pad(padSize, padValue).write(outname)
    return outDataList

def applyCOMAlignment(outDir, inDataListSeg, inDataListImg, processRaw=False):
    """
    This function takes in a filelist and produces the center of mass aligned
    files in the appropriate directory.
    """
    print("\n############# COM Alignment ###############")
    segDir = os.path.join(outDir, 'segmentations') if processRaw else outDir
    if not os.path.exists(segDir):
        os.makedirs(segDir)
    if processRaw:
        imageDir = os.path.join(outDir, 'images')
        if not os.path.exists(imageDir):
            os.makedirs(imageDir)

    outDataListImg = []
    outDataListSeg = []
    for i in range(len(inDataListSeg)):
        inname = inDataListSeg[i]
        outname = rename(inname, segDir, 'com')
        outDataListSeg.append(outname)
        img = Image(inname)
        T = img.centerOfMass() - img.center()

        # binarize result since linear interpolation makes image blurry again
        img.translate(T).binarize().write(outname)

        if processRaw:
            innameImg = inDataListImg[i]
            outnameImg = rename(innameImg, imageDir, 'com')
            outDataListImg.append(outnameImg)
            rawImg = Image(innameImg)
            rawImg.translate(T).write(outnameImg)

    return [outDataListSeg, outDataListImg] if processRaw else outDataListSeg

def FindReferenceImage(inDataList):
    """
    This find the median file between all the input files
    """
    x = y = z = 0
    for i in range(len(inDataList)):
        dim = itk.GetArrayFromImage(itk.imread(inDataList[i])).shape
        if dim[0] > x:
            x = dim[0]
        if dim[1] > y:
            y = dim[1]
        if dim[2] > z:
            z = dim[2]

    COM = np.zeros((x, y, z))
    for i in range(len(inDataList)):
        tmp = itk.GetArrayFromImage(itk.imread(inDataList[i]))
        COM += np.pad(tmp, (((x - tmp.shape[0]) // 2, (x - tmp.shape[0]) - (x - tmp.shape[0]) // 2),
                            ((y - tmp.shape[1]) // 2, (y - tmp.shape[1]) - (y - tmp.shape[1]) // 2),
                            ((z - tmp.shape[2]) // 2, (z - tmp.shape[2]) - (z - tmp.shape[2]) // 2)))
    COM /= len(inDataList)
    dist = np.inf
    idx = 0
    for i in range(len(inDataList)):
        tmp = itk.GetArrayFromImage(itk.imread(inDataList[i]))
        tmp_dist = np.linalg.norm(
            COM - np.pad(tmp, (((x - tmp.shape[0]) // 2, (x - tmp.shape[0]) - (x - tmp.shape[0]) // 2),
                               ((y - tmp.shape[1]) // 2, (y - tmp.shape[1]) - (y - tmp.shape[1]) // 2),
                               ((z - tmp.shape[2]) // 2, (z - tmp.shape[2]) - (z - tmp.shape[2]) // 2))))
        if tmp_dist < dist:
            idx = i
            dist = tmp_dist

    print(" ")
    print("############# Reference File #############")
    cprint(("The reference file for rigid alignment is found"), 'cyan')
    cprint(("Output Median Filename : ", inDataList[idx]), 'yellow')
    print("###########################################")
    print(" ")
    return inDataList[idx]

def applyResampling(outDir, refFile, inDataListSeg, inDataListImg=[]):
    """
    This function takes in a filelists(binary and raw) and makes the 
    size and spacing the same as the reference
    """
    print("\n############# Resample #############")

    # create output dirs
    segoutDir = os.path.join(outDir, 'segmentations') if inDataListImg else outDir
    if not os.path.exists(segoutDir):
        os.makedirs(segoutDir)
    if inDataListImg:
        rawoutDir = os.path.join(outDir, 'images')
        if not os.path.exists(rawoutDir):
            os.makedirs(rawoutDir)

    # apply rigid alignment
    outSegDataList = []
    outRawDataList = []

    # get reference image
    refImg = Image(refFile)

    for i in range(len(inDataListSeg)):
        segoutname = rename(inDataListSeg[i], segoutDir, 'resized')
        outSegDataList.append(segoutname)
        if inDataListImg:
            rawoutname = rename(inDataListImg[i], rawoutDir, 'resized')
            outRawDataList.append(rawoutname)

        # resize images to reference images
        img = Image(inDataListSeg[i])
        transform = createTransform(Matrix()) # create identity matrix
        img.resample(transform, refImg.origin(), refImg.dims(), refImg.spacing(), refImg.coordsys(), InterpolationType.NearestNeighbor).write(segoutname)
        if inDataListImg:
            img = Image(inDataListImg[i])
            img.resample(transform, refImg.origin(), refImg.dims(), refImg.spacing(), refImg.coordsys(), InterpolationType.NearestNeighbor).write(rawoutname)

    return [outSegDataList, outRawDataList] if inDataListImg else outSegDataList

def applyCropping(outDir, inDataList, path, paddingSize=10):
    """
    This function takes in a filelist and crops them according to the largest
    bounding box which it discovers
    """
    print("\n############## Cropping ##############")
    if not os.path.exists(outDir):
        os.makedirs(outDir)
    outDataList = []

    # find region by computing bounding box
    region = ImageUtils.boundingBox(glob.glob(path))
    region.pad(paddingSize)

    for i in range(len(inDataList)):
        inname = inDataList[i]
        initPath = os.path.dirname(inname)
        outname = rename(inname, outDir, 'cropped')
        outDataList.append(outname)
        img = Image(inname)
        img.crop(region).write(outname)
    return outDataList

def create_meshfromDT_xml(xmlfilename, tpdtnrrdfilename, vtkfilename):
    file = open(xmlfilename, "w+")
    file.write("<lsSmootherIterations>\n1.0\n</lsSmootherIterations>")
    file.write("<targetReduction>\n0.0001\n</targetReduction>")
    file.write("<featureAngle>\n30.0\n</featureAngle>")
    file.write("<preserveTopology>\n1\n</preserveTopology>")
    file.write("<inputs>\n" + str(tpdtnrrdfilename) +"\n</inputs>")
    file.write("<outputs>\n"+str(vtkfilename) + "\n</outputs>")
    file.close()

def applyDistanceTransforms(parentDir, inDataList, antialiasIterations=20, smoothingIterations=1, alpha=10.5, beta=10.0, scaling=20.0, isoValue=0):
    outDir = os.path.join(parentDir, 'groom_and_meshes')
    if not os.path.exists(outDir):
        os.makedirs(outDir)

    finalDTDir = os.path.join(parentDir, 'distance_transforms')
    if not os.path.exists(finalDTDir):
        os.makedirs(finalDTDir)

    outDataList = []
    for i in range(len(inDataList)):
        inname = inDataList[i]
        initPath = os.path.dirname(inDataList[i])
        outname = inname.replace(initPath, outDir)
        dtnrrdfilename = outname.replace('.nrrd', '.DT.nrrd')
        tpdtnrrdfilename = outname.replace('.nrrd', '.tpSmoothDT.nrrd')
        isonrrdfilename = outname.replace('.nrrd', '.ISO.nrrd')
        finalnm = tpdtnrrdfilename.replace(outDir, finalDTDir)
        outDataList.append(finalnm)
        img = Image(inname)
        img.extractLabel(1.0).closeHoles().write(inname)
        img.antialias(antialiasIterations).computeDT().write(dtnrrdfilename)
        img.applyCurvatureFilter(smoothingIterations).write(tpdtnrrdfilename)
        isoimg = ImageUtils.topologyPreservingSmooth(img, scaling, alpha, beta)
        isoimg.write(isonrrdfilename)
        shutil.copy(tpdtnrrdfilename, finalDTDir)
    return outDataList

### Mesh Grooming

# Reflects images and meshes to reference side
def anatomyPairsToSingles(outDir, seg_list, img_list, reference_side, printCmd=True):
    if reference_side == 'right':
        ref = 'R'
        flip = 'L'
    elif reference_side == 'left':
        ref = 'L'
        flip = 'R'
    else:
        raise Exception("reference_side must be 'left' or 'right'")

    if not os.path.exists(outDir):
        os.makedirs(outDir)
    outSegDir = os.path.join(outDir, "segmentations")
    if not os.path.exists(outSegDir):
        os.mkdir(outSegDir)
    outImgDir = os.path.join(outDir, "images")
    if not os.path.exists(outImgDir):
        os.mkdir(outImgDir)
    imageList = []
    meshList = []
    for image in img_list:
        img_name = os.path.basename(image)
        prefix = img_name.split("_")[0]
        # check if ref exists
        ref_prefix = prefix + "_" + ref
        flip_prefix = prefix + "_" + flip
        ref_seg = 'None'
        flip_seg = 'None'
        for seg in seg_list:
            if ref_prefix in seg:
                ref_seg = seg
            elif flip_prefix in seg:
                flip_seg = seg

        # if we have ref seg, copy image and seg over with appropriate name
        if ref_seg != 'None':
            seg_out = ref_seg.replace(os.path.dirname(ref_seg), outSegDir)
            meshList.append(seg_out)
            shutil.copy(ref_seg, seg_out)
            img_out = image.replace(os.path.dirname(image), outImgDir)
            img_out = img_out.replace(prefix, ref_prefix)
            imageList.append(img_out)
            shutil.copy(image, img_out)

        # if we have a seg for the non-ref side, reflect it
        if flip_seg != 'None':
            img_out = rename(image, outImgDir, 'reflect').replace(prefix, flip_prefix)
            imageList.append(img_out)
            centerFilename = os.path.join(outDir, prefix + "_origin.txt")
            img = Image(image)
            img.reflect("X").write(img_out)
            seg_out = rename(flip_seg, outSegDir, 'reflect')
            meshList.append(seg_out)
            execCommand = ["ReflectMesh", "--inFilename", flip_seg, "--outFilename", seg_out, "--reflectCenterFilename", centerFilename, "--inputDirection", "0", "--meshFormat", flip_seg.split(".")[-1]]
            if printCmd:
                print("CMD: " + " ".join(execCommand))
            subprocess.check_call(execCommand)
    return meshList, imageList

# Reflects meshes to reference side
def reflectMeshes(outDir, seg_list, reference_side, printCmd=True):
    if reference_side == 'right':
        ref = 'R'
        flip = 'L'
    elif reference_side == 'left':
        ref = 'L'
        flip = 'R'
    else:
        raise Exception("reference_side must be 'left' or 'right'")
    outSegDir = os.path.join(outDir, "segmentations")
    if not os.path.exists(outSegDir):
        os.makedirs(outSegDir)
    meshList = []
    for seg in seg_list:
        # if we have ref seg, copy seg over with appropriate name
        if ref in seg:
            seg_out = seg.replace(os.path.dirname(seg), outSegDir)
            shutil.copy(seg, seg_out)
        # if we have a seg for the non-ref side, reflect it
        else:
            seg_out = rename(seg, outSegDir, 'reflect')
            mesh_format = seg.split(".")[-1]
            centerFilename = seg_out.replace("." + mesh_format,"_origin.txt")
            execCommand = ["ReflectMesh", "--inFilename", seg, "--outFilename", seg_out, "--reflectCenterFilename", centerFilename, "--inputDirection", "0", "--meshFormat", mesh_format]
            if printCmd:
                print("CMD: " + " ".join(execCommand))
            subprocess.check_call(execCommand)
        meshList.append(seg_out)
    return meshList

# turns meshes in list into PLY format
def getVTKmeshes(meshList, printCmd=True):
    VTKmeshList = []
    for mesh in meshList:
        mesh_name = os.path.basename(mesh)
        extension = mesh_name.split(".")[-1]
        prefix = mesh_name.split("_")[0] + "_" + mesh_name.split("_")[1]
        if extension == "ply":
            mesh_old = mesh
            mesh = mesh[:-4] + ".vtk"
            execCommand = ["ply2vtk", mesh_old, mesh]
            if printCmd:
                print("CMD: " + " ".join(execCommand))
            subprocess.check_call(execCommand)
        elif extension == "stl":
            mesh_old = mesh
            mesh = mesh[:-4] + ".vtk"
            execCommand = ["stl2vtk", mesh_old, mesh]
            if printCmd:
                print("CMD: " + " ".join(execCommand))
            subprocess.check_call(execCommand)
        elif  extension == "vtk":
            pass
        else:
            print("Error: Mesh format unrecognized.")
            break
        VTKmeshList.append(mesh)
    return VTKmeshList

# turns meshes in list into PLY format
def getPLYmeshes(meshList, printCmd=True):
    PLYmeshList = []
    for mesh in meshList:
        mesh_name = os.path.basename(mesh)
        extension = mesh_name.split(".")[-1]
        prefix = mesh_name.split("_")[0] + "_" + mesh_name.split("_")[1]
        # change to ply if needed
        if extension == "vtk":
            mesh_vtk = mesh
            mesh = mesh[:-4] + ".ply"
            execCommand = ["vtk2ply", mesh_vtk, mesh]
            if printCmd:
                print("CMD: " + " ".join(execCommand))
            subprocess.check_call(execCommand)
        elif extension == "stl":
            mesh_stl = mesh
            mesh = mesh[:-4] + ".ply"
            execCommand = ["stl2ply", mesh_stl, mesh]
            if printCmd:
                print("CMD: " + " ".join(execCommand))
            subprocess.check_call(execCommand)
        elif  extension == "ply":
            pass
        else:
            print("error: Mesh format unrecognized.")
            break
        PLYmeshList.append(mesh)
    return PLYmeshList

# rasterization for meshes to DT
def MeshesToVolumesUsingImages(outDir, meshList, imgList, printCmd=True):
    segList= []
    if not os.path.exists(outDir):
        os.mkdir(outDir)

    PLYmeshList = getPLYmeshes(meshList)
    for mesh in PLYmeshList:
        mesh_name = os.path.basename(mesh)
        prefix = mesh_name.split("_")[0] + "_" + mesh_name.split("_")[1]

        # get image
        for image_file in imgList:
            if prefix in image_file:
                img = Image(image_file)

        # get origin, size, and spacing data
        data = {}
        data["origin"] = img.origin()
        data["size"] = img.dims()
        data["spacing"] = img.spacing()

        # write xml file
        infoPrefix = os.path.join(outDir, prefix)
        xmlfilename = infoPrefix + "_GenerateBinaryAndDT.xml"
        if os.path.exists(xmlfilename):
            os.remove(xmlfilename)
        xml = open(xmlfilename, "a")
        xml.write("<?xml version=\"1.0\" ?>\n")
        xml.write("<mesh>\n")
        xml.write(mesh+"\n")
        xml.write("</mesh>\n")

        # write origin, size, and spacing data
        for key,value in data.items():
            index = 0
            for dim in ["x","y","z"]:
                xml.write("<" + key + "_" + dim + ">" + str(value[index]) + "</" + key + "_" + dim + ">\n")
                index += 1
        xml.close()
        print("########### Turning Mesh To Volume ##############")
        segFile = rename(mesh, outDir, "", ".nrrd")

        # call generate binary and DT
        execCommand = ["GenerateBinaryAndDTImagesFromMeshes", xmlfilename]
        if printCmd:
            print("CMD: " + " ".join(execCommand))
        subprocess.check_call(execCommand)

        # save output volume
        output_volume = mesh.replace(".ply", ".rasterized_sp" + str(img.spacing()[0]) + ".nrrd")
        shutil.move(output_volume, segFile)
        segList.append(segFile)

        # save output DT
        output_DT =  mesh.replace(".ply", ".DT_sp" + str(img.spacing()[0]) + ".nrrd")
        dtFile = segFile.replace(".nrrd", "_DT.nrrd")
        shutil.move(output_DT, dtFile)

    return segList

# rasterization for meshes to DT
def MeshesToVolumes(outDir, meshList, spacing, printCmd=True):
    if not os.path.exists(outDir):
        os.mkdir(outDir)
    # get origin, size, and spacing data
    origin, size = getMeshInfo(outDir, meshList, spacing, printCmd)
    data = {}
    data["origin"] = origin
    data["size"] = size
    data["spacing"] = spacing

    segList= []
    PLYmeshList = getPLYmeshes(meshList)
    for mesh in PLYmeshList:
        mesh_name = os.path.basename(mesh)
        prefix = mesh_name.split("_")[0] + "_" + mesh_name.split("_")[1]
        # write xml file
        infoPrefix = os.path.join(outDir, prefix)
        xmlfilename = infoPrefix + "_GenerateBinaryAndDT.xml"
        if os.path.exists(xmlfilename):
            os.remove(xmlfilename)
        xml = open(xmlfilename, "a")
        xml.write("<?xml version=\"1.0\" ?>\n")
        xml.write("<mesh>\n")
        xml.write(mesh+"\n")
        xml.write("</mesh>\n")
        # write origin, size, and spacing data
        for key,value in data.items():
            index = 0
            for dim in ["x","y","z"]:
                xml.write("<" + key + "_" + dim + ">" + str(value[index]) + "</" + key + "_" + dim + ">\n")
                index += 1
        xml.close()
        print("########### Turning Mesh To Volume ##############")
        segFile = rename(mesh, outDir, "", ".nrrd")
        # call generate binary and DT
        execCommand = ["GenerateBinaryAndDTImagesFromMeshes", xmlfilename]
        if printCmd:
            print("CMD: " + " ".join(execCommand))
        subprocess.check_call(execCommand)
        # save output volume
        output_volume = mesh.replace(".ply", ".rasterized_sp" + str(spacing[0]) + ".nrrd")
        shutil.move(output_volume, segFile)
        segList.append(segFile)
        # save output DT
        output_DT =  mesh.replace(".ply", ".DT_sp" + str(spacing[0]) + ".nrrd")
        dtFile = segFile.replace(".nrrd", "_DT.nrrd")
        shutil.move(output_DT, dtFile)
    return segList

def getMeshInfo(outDir, meshList, spacing, printCmd=True):
    # get meshes in vtk format
    meshList = getVTKmeshes(meshList)
    meshListStr = ''
    for mesh in meshList:
        meshListStr += mesh + '\n'
    # Write XML
    xmlfilename = outDir + "MeshInfo.xml"
    out_origin = outDir + "origin.txt"
    out_size = outDir + "size.txt"
    xml = open(xmlfilename, "a")
    xml.write("<?xml version=\"1.0\" ?>\n")
    xml.write("<mesh>\n")
    xml.write(meshListStr+"\n")
    xml.write("</mesh>\n")
    xml.write("<spacing_x>\n" + str(spacing[0]) + "\n</spacing_x>\n")
    xml.write("<spacing_y>\n" + str(spacing[1]) + "\n</spacing_y>\n")
    xml.write("<spacing_z>\n" + str(spacing[2]) + "\n</spacing_z>\n")
    xml.write("<out_origin_filename>\n" + out_origin + "\n</out_origin_filename>\n")
    xml.write("<out_size_filename>\n" + out_size + "\n</out_size_filename>\n")
    xml.close()
    execCommand = ["ComputeRasterizationVolumeOriginAndSize", xmlfilename]
    if printCmd:
        print("CMD: " + " ".join(execCommand))
    subprocess.check_call(execCommand)
    # os.remove(xmlfilename)
    origin_file = open(out_origin, 'r')
    origin = np.array(origin_file.read().split()).astype(int)
    size_file = open(out_size, 'r')
    size = np.array(size_file.read().split()).astype(int)
    return origin, size

def ClipBinaryVolumes(outDir, segList, cutting_plane_points, printCmd=True):
    print("\n############## Clipping ##############")
    if not os.path.exists(outDir):
        os.makedirs(outDir)
    outListSeg = []
    for i in range(len(segList)):
        inname = segList[i]
        outname = rename(inname, outDir, "clipped")
        outListSeg.append(outname)
        img = Image(inname)
        img.clip(Point(cutting_plane_points[0], cutting_plane_points[1], cutting_plane_points[2]),
                 Point(cutting_plane_points[3], cutting_plane_points[4], cutting_plane_points[5]),
                 Point(cutting_plane_points[6], cutting_plane_points[7], cutting_plane_points[8]),0.0).write(outname)
    return outListSeg

def ShowCuttingPlanesOnImage(input_file, cutting_planes, printCmd=True):
    ## Get vtk format
    file_format = input_file.split(".")[-1]
    input_vtk = input_file.replace(file_format, "vtk")
    if file_format == "nrrd":
        print("\nCreating mesh from: " + input_file)
        print("\nSaving as: " + input_vtk)
        xml_filename = os.path.join(os.path.dirname(input_file), "cutting_plane_nrrd2vtk.xml")
        create_meshfromDT_xml(xml_filename, input_file, input_vtk)
        execCommand = ["MeshFromDistanceTransforms", xml_filename]
        if printCmd:
            print("CMD: " + " ".join(execCommand))
        subprocess.check_call(execCommand)
    elif file_format == "ply":
        execCommand = ["ply2vtk", input_file, input_vtk]
        if printCmd:
            print("CMD: " + " ".join(execCommand))
        subprocess.check_call(execCommand)
    elif file_format == "stl":
        execCommand = ["stl2vtk", input_file, input_vtk]
        if printCmd:
            print("CMD: " + " ".join(execCommand))
        subprocess.check_call(execCommand)
    elif file_format == "vtk":
        pass
    else:
        print("Error, file format unrecognized: " + input_file)

    ## VTK interactive window
    print('\n Use the interactive window to select your cutting plane. When you are content with your selection, simply close the window. \n')
    # read data from file
    reader = vtk.vtkPolyDataReader()
    reader.SetFileName(input_vtk)
    reader.ReadAllVectorsOn()
    reader.ReadAllScalarsOn()
    reader.Update()
    # get data
    data = reader.GetOutput()
    (xcenter, ycenter, zcenter) = data.GetCenter()
    #create mapper
    mapper = vtk.vtkPolyDataMapper()
    mapper.SetInputData(data)
    # The actor is a grouping mechanism
    actor = vtk.vtkActor()
    actor.SetMapper(mapper)
    # create camera
    camera = vtk.vtkCamera()
    camera.SetFocalPoint(xcenter, ycenter, zcenter)
    camera.SetPosition(100, -300, -50)
    camera.SetViewUp(0,0,1)
    # create a renderer
    renderer = vtk.vtkRenderer()
    renderer.SetActiveCamera(camera)
    renderer.SetBackground(0.2, 0.2, 0.5)
    renderer.SetBackground2(0.4, 0.4, 1.0)
    renderer.SetGradientBackground(True)
    renderer.AddActor(actor)
    # create a render_window
    render_window = vtk.vtkRenderWindow()
    render_window.AddRenderer(renderer)
    render_window.SetSize(1000,1000)
    # create a renderwindowiren
    iren = vtk.vtkRenderWindowInteractor()
    iren.SetRenderWindow(render_window)
    iren.Initialize()
    # Create a vtkImagePlaneWidget and activate it
    reps = []
    plane_widgets = []
    print(len(cutting_planes))
    for plane in cutting_planes:
        print(plane)
        A = np.array(plane[0])
        B = np.array(plane[1])
        C = np.array(plane[2])
        n = np.cross(B-A, C-A)
        n = n/np.linalg.norm(n)
        d = -np.dot(A, n)
        print(n, d)
        rep = vtk.vtkImplicitPlaneRepresentation()
        rep.SetPlaceFactor(1.5)
        rep.SetOrigin(A)
        rep.PlaceWidget(actor.GetBounds())
        rep.SetNormal(n[0],n[1],n[2])
        reps.append(rep)
        plane_widget = vtk.vtkImplicitPlaneWidget2()
        plane_widget.SetInteractor(iren)
        plane_widget.SetRepresentation(rep)
        plane_widget.On()
        plane_widgets.append(plane_widget)
    iren.Initialize()
    iren.Start()
    '''
    # use orgin as one point and use normla to solve for two others
    (o1,o2,o3) = rep.GetOrigin()
    (n1,n2,n3) = rep.GetNormal()
    # using x = 1 and y =-1 solve for z
    pt1_z = (-n1+(n1*o1)+n2+(n2*o2)+(n3*o3))/n3
    # using x = -1 and y = 1 solve for z
    pt2_z = (n1+(n1*o1)-n2+(n2*o2)+(n3*o3))/n3
    # fix 0 edge case
    if o1 == 0 and o2 == 0:
        o1 = -1
        o2 = -1
    return np.array([[o1, o2, o3], [1, -1, pt1_z], [-1, 1, pt2_z]])
    '''

def SelectCuttingPlane(input_file, printCmd=True):
    ## Get vtk format
    file_format = input_file.split(".")[-1]
    input_vtk = input_file.replace(file_format, "vtk")
    if file_format == "nrrd":
        print("\nCreating mesh from: " + input_file)
        print("\nSaving as: " + input_vtk)
        xml_filename = os.path.join(os.path.dirname(input_file), "cutting_plane_nrrd2vtk.xml")
        create_meshfromDT_xml(xml_filename, input_file, input_vtk)
        execCommand = ["MeshFromDistanceTransforms", xml_filename]
        if printCmd:
            print("CMD: " + " ".join(execCommand))
        subprocess.check_call(execCommand)
    elif file_format == "ply":
        execCommand = ["ply2vtk", input_file, input_vtk]
        if printCmd:
            print("CMD: " + " ".join(execCommand))
        subprocess.check_call(execCommand)
    elif file_format == "stl":
        execCommand = ["stl2vtk", input_file, input_vtk]
        if printCmd:
            print("CMD: " + " ".join(execCommand))
        subprocess.check_call(execCommand)
    elif file_format == "vtk":
        pass
    else:
        print("Error, file format unrecognized: " + input_file)

    ## VTK interactive window
    print('\n Use the interactive window to select your cutting plane. When you are content with your selection, simply close the window. \n')
    # read data from file
    reader = vtk.vtkPolyDataReader()
    reader.SetFileName(input_vtk)
    reader.ReadAllVectorsOn()
    reader.ReadAllScalarsOn()
    reader.Update()
    # get data
    data = reader.GetOutput()
    (xcenter, ycenter, zcenter) = data.GetCenter()
    #create mapper
    mapper = vtk.vtkPolyDataMapper()
    mapper.SetInputData(data)
    # The actor is a grouping mechanism
    actor = vtk.vtkActor()
    actor.SetMapper(mapper)
    # create camera
    camera = vtk.vtkCamera()
    camera.SetFocalPoint(xcenter, ycenter, zcenter)
    camera.SetPosition(100, -300, -50)
    camera.SetViewUp(0,0,1)
    # create a renderer
    renderer = vtk.vtkRenderer()
    renderer.SetActiveCamera(camera)
    renderer.SetBackground(0.2, 0.2, 0.5)
    renderer.SetBackground2(0.4, 0.4, 1.0)
    renderer.SetGradientBackground(True)
    renderer.AddActor(actor)
    # create a render_window
    render_window = vtk.vtkRenderWindow()
    render_window.AddRenderer(renderer)
    render_window.SetSize(1000,1000)
    # create a renderwindowiren
    iren = vtk.vtkRenderWindowInteractor()
    iren.SetRenderWindow(render_window)
    iren.Initialize()
    rep = vtk.vtkImplicitPlaneRepresentation()
    rep.SetPlaceFactor(1.25)
    rep.PlaceWidget(actor.GetBounds())
    rep.SetNormal(0,0,1)
    # Create a vtkImagePlaneWidget and activate it
    plane_widget = vtk.vtkImplicitPlaneWidget2()
    plane_widget.SetInteractor(iren)
    plane_widget.SetRepresentation(rep)
    plane_widget.On()
    iren.Initialize()
    iren.Start()
    # use orgin as one point and use normla to solve for two others
    (o1,o2,o3) = rep.GetOrigin()
    (n1,n2,n3) = rep.GetNormal()
    # using x = 1 and y =-1 solve for z
    pt1_z = (-n1+(n1*o1)+n2+(n2*o2)+(n3*o3))/n3
    # using x = -1 and y = 1 solve for z
    pt2_z = (n1+(n1*o1)-n2+(n2*o2)+(n3*o3))/n3
    # fix 0 edge case
    if o1 == 0 and o2 == 0:
        o1 = -1
        o2 = -1
    return np.array([[o1, o2, o3], [1, -1, pt1_z], [-1, 1, pt2_z]])
