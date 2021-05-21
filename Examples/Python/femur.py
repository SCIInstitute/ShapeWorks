# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Statistical Shape Modeling with ShapeWorks
====================================================================
The femur data set is comprised of segmented meshes of femurs and corresponding CT 
images that are not segmented.
The full images can be carried through every step of grooming. 
"""
import os
import glob
import numpy as np
import shapeworks as sw
import OptimizeUtils
import AnalyzeUtils
from GroomUtils import *

def Run_Pipeline(args):
    print("\nStep 1. Extract Data\n")
    """
    Step 1: EXTRACT DATA

    We define dataset_name which determines which dataset to download from 
    the portal and the directory to save output from the use case in. 
    """
    dataset_name = "femur-v0"
    output_directory = "Output/femur/"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    # If running a tiny_test, then download subset of the data
    if args.tiny_test:
        args.use_single_scale = 1
        sw.data.download_subset(args.use_case, dataset_name, output_directory)
        mesh_files = sorted(glob.glob(output_directory +
                            dataset_name + "/meshes/*.ply"))[:3]
        image_files = sorted(glob.glob(output_directory + 
                            dataset_name + "/images/*.nrrd"))[:3]
    # else download the entire dataset
    else:
        sw.data.download_and_unzip_dataset(dataset_name, output_directory)
        mesh_files = sorted(glob.glob(output_directory +
                            dataset_name + "/meshes/*.ply"))
        image_files = sorted(glob.glob(output_directory + dataset_name + "/images/*.nrrd"))


    # Select data if using subsample
    if args.use_subsample:
        sample_idx = sw.data.samplemeshes(mesh_files, int(args.num_subsample))
        mesh_files = [mesh_files[i] for i in sample_idx]

    # If skipping grooming, use the pregroomed distance transforms from the portal
    if args.skip_grooming:
        print("Skipping grooming.")
        dt_directory = output_directory + dataset_name + '/groomed/distance_transforms/'
        indices = []
        if args.tiny_test:
            indices = [0, 1, 2]
        elif args.use_subsample:
            indices = sample_idx
        dt_files = sw.data.get_file_list(
            dt_directory, ending=".nrrd", indices=indices)
    # Else groom the segmentations and get distance transforms for optimization
    else:
        print("\nStep 2. Groom - Data Pre-processing\n")
        """
        Step 2: GROOMING 
        
        The required grooming steps are: 
        1. Reflect images and meshes
        3. Turn meshes to volumes
        4. Isotropic resampling
        5. Padding
        6. Center of Mass Alignment
        7. Centering
        8. Rigid Alignment
        9. clip segementations with cutting plane
        10. find largest bounding box and crop

        For more information on grooming see docs/workflow/groom.md
        http://sciinstitute.github.io/ShapeWorks/workflow/groom.html
        """

        # Create a directory for groomed output
        groom_dir = output_directory + 'groomed/'
        if not os.path.exists(groom_dir):
            os.makedirs(groom_dir)
        
        # set name specific variables
        img_suffix = "1x_hip"
        reference_side = "left" # somewhat arbitrary, could be R for right
        # Set cutting plane
        cutting_plane_points = np.array([[-1.0, -1.0,-40.5],[1.0,-1.0,-40.5],[-1.0,1.0, -40.5]])
        cp_prefix = 'm03_L'

        # BEGIN GROOMING WITH IMAGES
        if args.groom_images and image_files:
            """
            Reflect - We have left and right femurs, so we reflect both image and mesh 
            for the non-reference side so that all of the femurs can be aligned.
            """
            reflectedmesh_files, reflectedFile_img = anatomyPairsToSingles(groom_dir + 'reflected', mesh_files, image_files, reference_side)

            """
            MeshesToVolumes - Shapeworks requires volumes so we need to convert 
            mesh segementaions to binary segmentations.
            """
            fileList_seg = MeshesToVolumesUsingImages(groom_dir + "volumes", reflectedmesh_files, reflectedFile_img)

            """
            Apply isotropic resampling
            The segmentation and images are resampled independently to have uniform spacing.
            """
            resampledFiles_segmentations = applyIsotropicResampling(groom_dir + "resampled/segmentations", fileList_seg, isBinary=True)
            resampledimage_filess = applyIsotropicResampling(groom_dir + "resampled/images", reflectedFile_img, isBinary=False)
            
            """
            Apply padding
            Both the segmentation and raw images are padded in case the seg lies on the image boundary.
            """
            paddedFiles_segmentations = applyPadding(groom_dir + "padded/segementations", resampledFiles_segmentations, 10)
            paddedimage_filess = applyPadding(groom_dir + "padded/images", resampledimage_filess, 10)

            """
            Apply center of mass alignment
            This function can handle both cases (processing only segmentation data or raw and segmentation data at the same time).
            """
            [comFiles_segmentations, comimage_filess] = applyCOMAlignment(groom_dir + "com_aligned", paddedFiles_segmentations, paddedimage_filess, processRaw=True)
            
            """
            Apply centering
            """
            centerFiles_segmentations = center(groom_dir + "centered/segmentations", comFiles_segmentations)
            centerimage_filess = center(groom_dir + "centered/images", comimage_filess)
            
            """
            Rigid alignment needs a reference file to align all the input files, FindReferenceImage function defines the median file as the reference.        
            """
            medianFile = FindReferenceImage(centerFiles_segmentations)
            
            """
            Rigid alignment
            """
            aligned_segmentations, aligned_images = applyRigidAlignment(groom_dir + "aligned", medianFile, centerFiles_segmentations, centerimage_filess)

            """
            Clip Binary Volumes - We have femurs of different shaft length so we will clip them all using the defined cutting plane.
            """
            clippedFiles_segmentations = ClipBinaryVolumes(groom_dir + 'clipped_segmentations', aligned_segmentations, cutting_plane_points.flatten())

            """Compute largest bounding box and apply cropping"""
            croppedFiles_segmentations = applyCropping(groom_dir + "cropped/segmentations", clippedFiles_segmentations, clippedFiles_segmentations)
            croppedimage_filess = applyCropping(groom_dir + "cropped/images", aligned_images, clippedFiles_segmentations)

            groomed_segmentations = croppedFiles_segmentations

        # BEGIN GROOMING WITHOUT IMAGES
        else:
            """
            First, we need to loop over the mesh files and load the meshes
            """
            # list of shape segmentations
            mesh_list = []
            # list of shape names (shape files prefixes) to be used for saving outputs
            mesh_names = []
            for mesh_filename in mesh_files:
                print('Loading: ' + mesh_filename)
                # get current shape name
                mesh_names.append(mesh_filename.split('/')
                                   [-1].replace('.ply', ''))
                # load mesh
                mesh = sw.Mesh(mesh_filename)
                # append to the mesh list
                mesh_list.append(mesh)

            """
            Now we can loop over the segmentations and apply the initial grooming steps to them
            """
            for mesh, mesh_name in zip(mesh_list, mesh_names):
                """
                Grooming Step 1: Reflect - We have left and right femurs, so we reflect the right 
                meshes so that all of the femurs can be aligned. 
                """
                if "R" in mesh_name:
                    print("Reflecting "+ mesh_name)
                    arr = mesh.center(); center = [arr[0], arr[1], arr[2]]
                    mesh.reflect(sw.X)


                """
                Grooming Step 2: Center of mass alignment 
                This step translates the center of mass of the mesh to [0,0,0]. It involves:
                    - Finding the center of mass of the mesh
                    - Defining the translation vector from the COM to [0,0,0]
                    - Applying the translation
                """
                print('Center of mass alignment: ' + mesh_name)
                # compute the center of mass of this mesh
                com = mesh.centerOfMass()
                # define the translation to move the shape to [0,0,0]
                translationVector = [0,0,0] - com
                # perform antialias-translate-binarize
                mesh.translate(translationVector)

            """
            Grooming Step 3: Select a reference
            This step requires breaking the loop to load all of the meshes at once so the shape
            closest to the mean can be found and selected as the reference. 
            """
            print('Selecting reference mesh...')
            ref_index = sw.find_reference_mesh_index(mesh_list)
            # Make a copy of the reference segmentation
            ref_mesh = mesh_list[ref_index].write(groom_dir + 'reference.ply')
            ref_name = mesh_names[ref_index]
            print("Reference found: " + ref_name)

            
            for mesh, mesh_name in zip(mesh_list, mesh_names):
                """
                Grooming Step 4: Rigid alignment
                This step rigidly aligns each mesh to the selected references. 
                There are two steps:
                    - computing the rigid transformation parameters that would align the 
                    mesh to the reference
                    - applying the rigid transformation to the meshes
                """
                rigid_transform = mesh.createTransform(ref_mesh, sw.TransformType.IterativeClosestPoint, sw.Mesh.AlignmentType.Rigid, 30)
                mesh.applyTransform(rigid_transform)
                mesh.clip(sw.Plane(cutting_plane_points))


            sw.utils.save_meshes(groom_dir, mesh_list, mesh_names)
            input()
        


        print("\nStep 3. Groom - Convert to distance transforms\n")
        """
        We convert the scans to distance transforms, this step is common for both the
        prepped as well as unprepped data, just provide correct filenames.
        """
        dtFiles = applyDistanceTransforms(groom_dir, groomed_segmentations)

    """
    ## OPTIMIZE : Particle Based Optimization

    Now that we have the distance transform representation of data we create
    the parameter files for the shapeworks particle optimization routine.
    For more details on the plethora of parameters for shapeworks please refer to
    '/Documentation/PDFs/ParameterDescription.pdf'

    We provide two different mode of operations for the ShapeWorks particle opimization;
    1- Single Scale model takes fixed number of particles and performs the optimization.
    2- Multi scale model optimizes for different number of particles in hierarchical manner.
    
    For more detail about the optimization steps and parameters please refer to
    '/docs/workflow/optimize.md'

    First we need to create a dictionary for all the parameters required by these
    optimization routines
    """
    print("\nStep 4. Optimize - Particle Based Optimization\n")
    point_dir = output_directory + 'shape_models/'
    if not os.path.exists(point_dir):
        os.makedirs(point_dir)

    parameterDictionary = {
        "number_of_particles" : 1024,
        "use_normals": 0,
        "normal_weight": 10.0,
        "checkpointing_interval" : 200,
        "keep_checkpoints" : 1,
        "iterations_per_split" : 4000,
        "optimization_iterations" : 4000,
        "starting_regularization" : 100,
        "ending_regularization" : 0.1,
        "recompute_regularization_interval" : 2,
        "domains_per_shape" : 1,
        "domain_type" : 'image',
        "relative_weighting" : 10,
        "initial_relative_weighting" : 1,
        "procrustes_interval" : 1,
        "procrustes_scaling" : 1,
        "save_init_splits" : 1,
        "verbosity" : 2,
        "use_statistics_in_init" : 0
    }
    if args.tiny_test:
        parameterDictionary["number_of_particles"] = 32
        parameterDictionary["optimization_iterations"] = 25
        parameterDictionary["iterations_per_split"] = 25

    if not args.use_single_scale:
        parameterDictionary["use_shape_statistics_after"] = 64

    """
    Now we execute the particle optimization function.
    """
    [localPointFiles, worldPointFiles] = runShapeWorksOptimize(point_dir, dtFiles, parameterDictionary)

    if args.tiny_test:
        print("Done with tiny test")
        exit()

    """
    ## ANALYZE : Shape Analysis and Visualization

    Shapeworks yields relatively sparse correspondence models that may be inadequate to reconstruct
    thin structures and high curvature regions of the underlying anatomical surfaces.
    However, for many applications, we require a denser correspondence model, for example,
    to construct better surface meshes, make more detailed measurements, or conduct biomechanical
    or other simulations on mesh surfaces. One option for denser modeling is
    to increase the number of particles per shape sample. However, this approach necessarily
    increases the computational overhead, especially when modeling large clinical cohorts.

    Here we adopt a template-deformation approach to establish an inter-sample dense surface correspondence,
    given a sparse set of optimized particles. To avoid introducing bias due to the template choice, we developed
    an unbiased framework for template mesh construction. The dense template mesh is then constructed
    by triangulating the isosurface of the mean distance transform. This unbiased strategy will preserve
    the topology of the desired anatomy  by taking into account the shape population of interest.
    In order to recover a sample-specific surface mesh, a warping function is constructed using the
    sample-level particle system and the mean/template particle system as control points.
    This warping function is then used to deform the template dense mesh to the sample space.

    Reconstruct the dense mean surface given the sparse correspondence model.
    """
    print("\nStep 5. Analysis - Reconstruct the dense mean surface given the sparse correspodence model.\n")
    launchShapeWorksStudio(point_dir, dtFiles, localPointFiles, worldPointFiles)
