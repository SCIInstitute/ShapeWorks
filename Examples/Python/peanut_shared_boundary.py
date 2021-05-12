# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Shared Boundaries in ShapeWorks
====================================================================

Demonstrates how to obtain a shape model for two spheres with a shared boundary. The
shared boundary mesh is extracted, and the contour. Shape statistics for each of these
pieces are then jointly optimized
"""
import os
import shutil
from GroomUtils import *
from OptimizeUtils import *
from AnalyzeUtils import *
import CommonUtils

def Run_Pipeline(args):
    datasetName = "peanut-v0"
    outputDirectory = "Output/peanut_shared_boundary/"
    groomDir = outputDirectory + datasetName + "/groomed/"
    if not os.path.exists(outputDirectory):
        os.makedirs(outputDirectory)
    if not os.path.exists(groomDir):
        os.makedirs(groomDir)

    CommonUtils.download_and_unzip_dataset(datasetName, outputDirectory)
    meshFiles = sorted(glob.glob(outputDirectory + datasetName + "/meshes/*.stl"))

    # 01. Remesh
    # TODO This has to be done using mmgtools, but since this is not yet available
    # it is already done using MeshLab and stored in the appropriate folder. This
    # code is commented out because pymeshlab's Qt conflicts with ShapeWorks' Qt
    remeshDir = groomDir + "01_remesh/"
    if not os.path.exists(remeshDir):
        os.makedirs(remeshDir)
    """
    for fname in meshFiles:
        ms = pymeshlab.MeshSet()
        ms.load_new_mesh(fname)
        targetlen = 0.4 # Data specific
        ms.remeshing_isotropic_explicit_remeshing(targetlen=targetlen)
        new_name = remeshDir + fname.split('/')[-1]
        ms.save_current_mesh(new_name)
    """

    # 02. Extract shared boundaries
    # TODO Uses the shared_boundary_extractor script that has to be consolidated into shapework.Mesh
    extractedDir = groomDir + "02_extracted/"
    if not os.path.exists(extractedDir):
        os.makedirs(extractedDir)
    for i in range(15):
        fname_l = f'{remeshDir}/{i:02d}_l.ply'
        fname_r = f'{remeshDir}/{i:02d}_r.ply'
        tol = '1e-3' # distance threshold to consider two surfaces as "shared"

        fname_out_l = f'{extractedDir}/{i:02d}_l.ply'
        fname_out_r = f'{extractedDir}/{i:02d}_r.ply'
        fname_out_m = f'{extractedDir}/{i:02d}_m.ply'
        cmd = [
            args.shapeworks_path+'/shared_boundary_extractor',
            fname_l, fname_r,
            fname_out_l, fname_out_r, fname_out_m,
            tol
        ]
        print(f'Extraction Command: {" ".join(cmd)}')
        subprocess.check_call(cmd)

    # 03. Smooth
    # TODO Use shapeworks.Mesh() https://github.com/SCIInstitute/ShapeWorks/issues/1234
    # For now, use meshes pre-smoothed using MeshLab. These were smoothed using "Laplacian smooth"
    # with default parameters
    smoothDir = groomDir + "03_smooth/"
    if not os.path.exists(smoothDir):
        os.makedirs(smoothDir)
    """
    for fname in glob.glob(f'{extractedDir}/*.ply')
        ms = pymeshlab.MeshSet()
        ms.load_new_mesh(fname)
        ms.laplacian_smooth()
        new_name = smoothDir + fname.split('/')[-1]
        ms.save_current_mesh(new_name)
    """

    # 04. Extract boundary loop
    # TODO This uses the boundary_loop_extractor script which has to eventually be added
    # to shapeworks.Mesh. Contour I/O has to potentially be supported in shapeworks.Mesh
    finalDir = groomDir + "04_final/"
    if not os.path.exists(finalDir):
        os.makedirs(finalDir)
    for i in range(15):
        fname_l = f'{smoothDir}/{i:02d}_l.ply'
        fname_r = f'{smoothDir}/{i:02d}_r.ply'
        fname_m = f'{smoothDir}/{i:02d}_m.ply'

        fname_out_l = f'{finalDir}/{i:02d}_l.ply'
        fname_out_r = f'{finalDir}/{i:02d}_r.ply'
        fname_out_m = f'{finalDir}/{i:02d}_m.ply'
        fname_out_c = f'{finalDir}/{i:02d}_c.vtp'

        # Copy L, R, M as is
        shutil.copy(fname_l, fname_out_l)
        shutil.copy(fname_r, fname_out_r)
        shutil.copy(fname_m, fname_out_m)

        # Extract boundary loop from M
        cmd = [
            args.shapeworks_path+'/boundary_loop_extractor',
            fname_m,
            fname_out_c,
        ]
        print(f'Boundary Extraction Command: {" ".join(cmd)}')
        subprocess.check_call(cmd)

    # Copy everything to shape model directory
    shapeModelDir = outputDirectory + "shape_model/"
    if not os.path.exists(shapeModelDir):
        os.makedirs(shapeModelDir)
    for fname in glob.glob(f'{finalDir}/*'):
        new_name = shapeModelDir + fname.split('/')[-1]
        shutil.copy(fname, new_name)

    # TODO Use OptimizeUtils.py once Multidomain PR is merged https://github.com/SCIInstitute/ShapeWorks/pull/1143
    shutil.copy(f'{outputDirectory}/{datasetName}/shape_models/peanut_shared_boundary/shared_with_contours.xlsx',
                f'{outputDirectory}/shape_model/')
    optimizeCmd = 'shapeworks optimize --name Output/peanut_shared_boundary/shape_model/shared_with_contours.xlsx'.split()
    subprocess.check_call(optimizeCmd)

    # TODO Use AnalyzeUtils.py once Multidomain PR is merged https://github.com/SCIInstitute/ShapeWorks/pull/1143
    analyzeCmd = 'ShapeWorksStudio Output/peanut_shared_boundary/shape_model/shared_with_contours.xlsx'.split()
    subprocess.check_call(analyzeCmd)