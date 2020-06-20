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
    cprint(("Input Filename : ", inname), 'cyan')
    cprint(("Output Filename : ", outname), 'yellow')
    print("######################################\n")
    return outname

def applyIsotropicResampling(outDir, inDataList, isoSpacing=1.0, isBinary=True, printCmd = True):
    """
    This function takes in a filelist and produces the resampled files in the appropriate directory.
    """
    print("\n########### Resampling ###############")
    if not os.path.exists(outDir):
        os.makedirs(outDir)
    outDataList = []
    for i in range(len(inDataList)):
        inname = inDataList[i]
        outname = rename(inname, outDir, 'isores')
        outDataList.append(outname)
        cmd = ["shapeworks", 
               "read-image", "--name", inname]
        if isBinary:
            cmd.extend(["antialias"])
        cmd.extend(["resample", "--isospacing", str(isoSpacing)])  
        if isBinary:
            cmd.extend(["threshold"])
        cmd.extend(["write-image", "--name", outname])
        if printCmd:
            print("CMD: " + " ".join(cmd))
        subprocess.check_call(cmd)
    return outDataList

def center(outDir, inDataList, printCmd = True):
    print("\n########### Centering ###############")
    if not os.path.exists(outDir):
        os.makedirs(outDir)
    outDataList = []
    for i in range(len(inDataList)):
        inname = inDataList[i]
        outname = rename(inname, outDir, 'center')
        outDataList.append(outname)
        cmd = ["shapeworks", 
               "read-image", "--name", inname,
               "recenter",
               "write-image", "--name", outname]
        if printCmd:
            print("CMD: " + " ".join(cmd))
        subprocess.check_call(cmd)
    return outDataList

def applyPadding(outDir, inDataList, padSize, padValue=0, printCmd = True):
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
        cmd = ["shapeworks", 
               "read-image", "--name", inname,
               "pad", "--padding", str(padSize), "--value", str(padValue),
               "write-image", "--name", outname]
        if printCmd:
            print("CMD: " + " ".join(cmd))
        subprocess.check_call(cmd)
    return outDataList

def applyCOMAlignment(outDir, inDataList, raw=[], printCmd = True):
    """
    This function takes in a filelist and produces the center of mass aligned
    files in the appropriate directory.
    """
    print("\n############# COM Alignment ###############")
    if raw:
        imageDir = os.path.join(outDir, 'images')
        segDir = os.path.join(outDir, 'segmentations')
        if not os.path.exists(imageDir):
            os.makedirs(imageDir)
        if not os.path.exists(segDir):
            os.makedirs(segDir)
        outDataListImg = []
        outDataListSeg = []
        inDataListImg = raw
    else:
        if not os.path.exists(outDir):
            os.makedirs(outDir)
        outDataListSeg = []

    for i in range(len(inDataList)):
        inname = inDataList[i]
        if raw:
            outname = rename(inname, segDir, 'com')
        else:
            outname = rename(inname, outDir, 'com')
        outDataListSeg.append(outname)
        cmd = ["shapeworks",
               "read-image", "--name", inname,
               "info", "--centerofmass", str(True)]
        if printCmd:
            print("CMD: " + " ".join(cmd))
        com = subprocess.run(cmd, stdout=subprocess.PIPE).stdout.decode('utf-8')
        com = com.strip().replace("[", "").replace("]", "").replace(" ", "").split(":")
        com = com[1].split(",")
        cmd = ["shapeworks", 
               "readimage", "--name", inname, 
               "translate", "--applycenterofmass", str(True), "-x", com[0], "-y", com[1], "-z", com[2], 
               "write-image", "--name", outname]
        if printCmd:
            print("CMD: " + " ".join(cmd))
        subprocess.check_call(cmd)
        if raw:
            innameImg = inDataListImg[i]
            outnameImg = rename(innameImg, imageDir, 'com')
            outDataListImg.append(outnameImg)
            cmd = ["shapeworks", 
                   "readimage", "--name", innameImg, 
                   "translate", "--applycenterofmass", str(True), "-x", com[0], "-y", com[1], "-z", com[2], 
                   "write-image", "--name", outnameImg]
            if printCmd:
                print("CMD: " + " ".join(cmd))
            subprocess.check_call(cmd)

    if raw:
        return [outDataListSeg, outDataListImg]
    else:
        return outDataListSeg

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

def applyRigidAlignment(outDir, inDataListSeg, inDataListImg, refFile,
                        antialiasIterations=20, smoothingIterations=1, alpha=10.5, beta=10.0,
                        scaling=20.0, isoValue=0, icpIterations=10, processRaw=False, printCmd = True):
    """
    This function takes in a filelists(binary and raw) and produces rigid aligned
    files in the appropriate directory. If the process_raw flag is set True,
    then it also applys the same transformation on the corresponding list of
    raw files (MRI/CT ...)
    """
    print("\n############# Rigid Alignment #############")
    if not os.path.exists(outDir):
        os.makedirs(outDir)

    # identify the reference scan
    refDir = os.path.join(outDir, 'reference')
    if not os.path.exists(refDir):
        os.makedirs(refDir)
    initPath = os.path.dirname(refFile)
    newRefFile = refFile.replace(initPath, refDir)
    ref_dtnrrdfilename = newRefFile.replace('.nrrd', '.DT.nrrd')
    ref_tpdtnrrdfilename = newRefFile.replace('.nrrd', '.tpSmoothDT.nrrd')
    ref_isonrrdfilename = newRefFile.replace('.nrrd', '.ISO.nrrd')

    # reference image processing
    cmd = ["shapeworks",
           "read-image", "--name", refFile,
           "extract-label", "--label", str(1.0),
           "close-holes",
           "write-image", "--name", refFile,
           "antialias", "--iterations", str(antialiasIterations),
           "compute-dt", "--isovalue", str(isoValue),
           "write-image", "--name", ref_dtnrrdfilename,
           "curvature", "--iterations", str(smoothingIterations),
           "write-image", "--name", ref_tpdtnrrdfilename,
           "topo-preserving-smooth", "--scaling", str(scaling), "--alpha", str(alpha), "--beta", str(beta),
           "write-image", "--name", ref_isonrrdfilename]
    if printCmd:
        print("CMD: " + " ".join(cmd))
    subprocess.check_call(cmd)

    if processRaw:
        rawoutDir = os.path.join(outDir, 'images')
        binaryoutDir = os.path.join(outDir, 'segmentations')
        if not os.path.exists(rawoutDir):
            os.makedirs(rawoutDir)
        if not os.path.exists(binaryoutDir):
            os.makedirs(binaryoutDir)
        outRawDataList=[]
        outSegDataList=[]
        for i in range(len(inDataListSeg)):
            seginname = inDataListSeg[i]
            initPath = os.path.dirname(seginname)
            segoutname = seginname.replace(initPath, binaryoutDir)
            segoutname = segoutname.replace('.nrrd', '.aligned.nrrd')
            outSegDataList.append(segoutname)
            rawinname = inDataListImg[i]
            initPath = os.path.dirname(rawinname)
            rawoutname = rawinname.replace(initPath, rawoutDir)
            rawoutname = rawoutname.replace('.nrrd', '.aligned.nrrd')
            outRawDataList.append(rawoutname)
            dtnrrdfilename = segoutname.replace('.aligned.nrrd', '.aligned.DT.nrrd')
            tpdtnrrdfilename = segoutname.replace('.aligned.nrrd', '.aligned.tpSmoothDT.nrrd')
            isonrrdfilename = segoutname.replace('.aligned.nrrd', '.aligned.ISO.nrrd')
            cmd = ["shapeworks", 
                   "read-image", "--name", seginname, 
                   "extract-label", "--label", str(1.0),
                   "close-holes",
                   "write-image", "--name", seginname,
                   "antialias", "--iterations", str(antialiasIterations),
                   "compute-dt", "--isovalue", str(isoValue),
                   "write-image", "--name", dtnrrdfilename,
                   "curvature", "--iterations", str(smoothingIterations),
                   "write-image", "--name", tpdtnrrdfilename,
                   "topo-preserving-smooth", "--scaling", str(scaling), "--alpha", str(alpha), "--beta", str(beta),
                   "write-image", "--name", isonrrdfilename]
            if printCmd:
                print("CMD: " + " ".join(cmd))
            subprocess.check_call(cmd)

            cmd = ["shapeworks", 
                   "read-image", "--name", seginname,
                   "icp", "--target", ref_tpdtnrrdfilename, "--source", tpdtnrrdfilename, "--iterations", str(icpIterations),
                   "write-image", "--name", segoutname]
            if printCmd:
                print("CMD: " + " ".join(cmd))
            subprocess.check_call(cmd)

            cmd = ["shapeworks", 
                   "read-image", "--name", rawinname,
                   "icp", "--target", ref_tpdtnrrdfilename, "--source", tpdtnrrdfilename, "--iterations", str(icpIterations),
                   "write-image", "--name", rawoutname]
            if printCmd:
                print("CMD: " + " ".join(cmd))
            subprocess.check_call(cmd)
        return  [outSegDataList, outRawDataList]

    else:
        outDataList = []
        for i in range(len(inDataListSeg)):
            inname = inDataListSeg[i]
            initPath = os.path.dirname(inname)
            outname = inname.replace(initPath, outDir)
            outname = outname.replace('.nrrd', '.aligned.nrrd')
            outDataList.append(outname)
            dtnrrdfilename = outname.replace('.aligned.nrrd', '.aligned.DT.nrrd')
            tpdtnrrdfilename = outname.replace('.aligned.nrrd', '.aligned.tpSmoothDT.nrrd')
            isonrrdfilename = outname.replace('.aligned.nrrd', '.aligned.ISO.nrrd')
            cmd = ["shapeworks", 
                   "read-image", "--name", inname,
                   "extract-label", "--label", str(1.0),
                   "close-holes",
                   "write-image", "--name", inname,
                   "antialias", "--iterations", str(antialiasIterations),
                   "compute-dt", "--isovalue", str(isoValue),
                   "write-image", "--name", dtnrrdfilename,
                   "curvature", "--iterations", str(smoothingIterations),
                   "write-image", "--name", tpdtnrrdfilename,
                   "topo-preserving-smooth", "--scaling", str(scaling), "--alpha", str(alpha), "--beta", str(beta),
                   "write-image", "--name", isonrrdfilename]
            if printCmd:
                print("CMD: " + " ".join(cmd))
            subprocess.check_call(cmd)

            cmd = ["shapeworks", 
                   "read-image", "--name", inname,
                   "icp", "--source", tpdtnrrdfilename, "--target", ref_tpdtnrrdfilename, "--iterations", str(icpIterations),
                   "write-image", "--name", outname]
            if printCmd:
                print("CMD: " + " ".join(cmd))
            subprocess.check_call(cmd)
        return outDataList

def applyCropping(outDir, inDataList, path, paddingSize=10, printCmd = True):
    """
    This function takes in a filelist and crops them according to the largest
    bounding box which it discovers
    """
    print("\n############## Cropping ##############")
    if not os.path.exists(outDir):
        os.makedirs(outDir)
    outDataList = []
    for i in range(len(inDataList)):
        inname = inDataList[i]
        initPath = os.path.dirname(inname)
        outname = inname.replace(initPath, outDir)
        outname = outname.replace('.nrrd', '.cropped.nrrd')
        outDataList.append(outname)
        cmd = ["shapeworks",
               "boundingbox", "--names"] + glob.glob(path) + ["--", "--padding", str(paddingSize),
               "read-image", "--name", inname,
               "crop",
               "write-image", "--name", outname]
        if printCmd:
            print("CMD: " + " ".join(cmd))
        subprocess.check_call(cmd)
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

def applyDistanceTransforms(parentDir, inDataList, antialiasIterations=20, smoothingIterations=1, alpha=10.5, beta=10.0, scaling=20.0, isoValue=0, printCmd = True):
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
        cmd = ["shapeworks", 
               "read-image", "--name", inname,
               "extract-label", "--label", str(1.0),
               "close-holes",
               "write-image", "--name", inname,
               "antialias", "--iterations", str(antialiasIterations),
               "compute-dt", "--isovalue", str(isoValue),
               "write-image", "--name", dtnrrdfilename,
               "curvature", "--iterations", str(smoothingIterations),
               "write-image", "--name", tpdtnrrdfilename,
               "topo-preserving-smooth", "--scaling", str(scaling), "--alpha", str(alpha), "--beta", str(beta),
               "write-image", "--name", isonrrdfilename]
        if printCmd:
            print("CMD: " + " ".join(cmd))
        subprocess.check_call(cmd)
        shutil.copy(tpdtnrrdfilename, finalDTDir)
    return outDataList

### Mesh Grooming 

# Refelcts images and meshes to reference side
def anatomyPairsToSingles(outDir, seg_list, img_list, reference_side, printCmd = True):
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
    for img in img_list:
        img_name = os.path.basename(img)
        prefix = img_name.split("_")[0]
        if reference_side == 'right':
            ref = 'R'
            flip = 'L'
        elif reference_side == 'left':
            ref = 'L'
            flip = 'R'
        else:
            print("Error: reference side must be 'left' or 'right'.")
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
            img_out = img.replace(os.path.dirname(img), outImgDir)
            img_out = img_out.replace(prefix, ref_prefix)
            imageList.append(img_out)
            shutil.copy(img, img_out)
        # if we have a seg for the non-ref side, reflect it
        if flip_seg != 'None':
            print("\n############## Reflecting ###############")
            img_out = rename(img, outImgDir, 'reflect').replace(prefix, flip_prefix)
            imageList.append(img_out)
            centerFilename = os.path.join(outDir, prefix + "_origin.txt")
            cmd = ["shapeworks", 
                   "read-image", "--name", img,
                   "reflect", "--x", str(-1), "--y", str(1), "--z", str(1),
                   "write-image", "--name", img_out]
            if printCmd:
                print("CMD: " + " ".join(cmd))
            subprocess.check_call(cmd)
            seg_out = rename(flip_seg, outSegDir, 'reflect')
            meshList.append(seg_out)
            execCommand = ["ReflectMesh", "--inFilename", flip_seg, "--outFilename", seg_out, "--reflectCenterFilename", centerFilename, "--inputDirection", "0", "--meshFormat", flip_seg.split(".")[-1]]
            if printCmd:
                print("CMD: " + " ".join(execCommand))
            subprocess.check_call(execCommand)        
    return meshList, imageList

# rasterization for meshes to DT
def MeshesToVolumes(outDir, meshList, imgList, printCmd = True):
    segList= []
    if not os.path.exists(outDir):
        os.mkdir(outDir)
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
        if extension == "stl":
            mesh_vtk = mesh
            mesh = mesh[:-4] + ".ply"
            execCommand = ["stl2ply", mesh_vtk, mesh]
            if printCmd:
                print("CMD: " + " ".join(execCommand))
            subprocess.check_call(execCommand) 
        # get image
        for image_file in imgList:
            if prefix in image_file:
                image = image_file
         # write origin, size, and spacing info to text file
        infoPrefix = os.path.join(outDir, prefix)
        cmd = ["shapeworks",
               "readimage", "--name", image,
               "info", "--origin", str(True)]
        if printCmd:
            print("CMD: " + " ".join(cmd))
        origin = subprocess.run(cmd, stdout=subprocess.PIPE).stdout.decode('utf-8')
        origin = origin.strip().replace("[", "").replace("]", "").replace(" ", "").split(":")
        origin = origin[1].split(",")

        cmd = ["shapeworks",
               "readimage", "--name", image,
               "info", "--dims", str(True)]
        if printCmd:
            print("CMD: " + " ".join(cmd))
        size = subprocess.run(cmd, stdout=subprocess.PIPE).stdout.decode('utf-8')
        size = size.strip().replace("[", "").replace("]", "").replace(" ", "").split(":")
        size = size[1].split(",")

        cmd = ["shapeworks",
               "readimage", "--name", image,
               "info", "--spacing", str(True)]
        if printCmd:
            print("CMD: " + " ".join(cmd))
        spacing = subprocess.run(cmd, stdout=subprocess.PIPE).stdout.decode('utf-8')
        spacing = spacing.strip().replace("[", "").replace("]", "").replace(" ", "").split(":")
        spacing = spacing[1].split(",")
        # get origin, size, and spacing data
        data = {}
        data["origin"] = origin
        data["size"] = size
        data["spacing"] = spacing
        spacingX = spacing[0]
        # write xml file
        xmlfilename=infoPrefix + "_GenerateBinaryAndDT.xml"
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
        print("########### Turning  Mesh To Volume ##############")
        segFile = rename(mesh, outDir, "", ".nrrd")
        # call generate binary and DT
        execCommand = ["GenerateBinaryAndDTImagesFromMeshes", xmlfilename]
        if printCmd:
            print("CMD: " + " ".join(execCommand))
        subprocess.check_call(execCommand)
         # save output volume
        output_volume = mesh.replace(".ply", ".rasterized_sp" + str(spacingX) + ".nrrd")
        shutil.move(output_volume, segFile)
        segList.append(segFile)
        #save output DT
        output_DT =  mesh.replace(".ply", ".DT_sp" + str(spacingX) + ".nrrd")
        dtFile = segFile.replace(".nrrd", "_DT.nrrd")
        shutil.move(output_DT, dtFile)                    
    return segList

def ClipBinaryVolumes(outDir, segList, cutting_plane_points, printCmd = True):
    if not os.path.exists(outDir):
        os.makedirs(outDir)
    outListSeg = []
    for seg in segList:
        print("\n############## Clipping ##############")
        seg_out = rename(seg, outDir, "clipped")
        outListSeg.append(seg_out)
        cmd = ["shapeworks", "read-image", "--name", seg,
               "clip", "--x1", str(cutting_plane_points[0]), "--y1", str(cutting_plane_points[3]), "--z1", str(cutting_plane_points[6]),
                       "--x2", str(cutting_plane_points[1]), "--y2", str(cutting_plane_points[4]), "--z2", str(cutting_plane_points[7]),
                       "--x3", str(cutting_plane_points[2]), "--y3", str(cutting_plane_points[5]), "--z3", str(cutting_plane_points[8]),
               "write-image", "--name", seg_out]
        if printCmd:
            print("CMD: " + " ".join(cmd))
        subprocess.check_call(cmd)
    return outListSeg

def SelectCuttingPlane(input_file, printCmd = True):
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
