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
import pickle
import numpy as np
import shapeworks as sw
import OptimizeUtils
import AnalyzeUtils

def Run_Pipeline(args):
    print("\nStep 1. Extract Data\n")
    """
    Step 1: EXTRACT DATA

    We define dataset_name which determines which dataset to download from
    the portal and the directory to save output from the use case in.
    This data is comprised of femur meshes and corresponding unsegmented hip CT scans.
    """
    dataset_name = "femur-v0"
    output_directory = "Output/femur_cut/"
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
        image_files = sorted(
            glob.glob(output_directory + dataset_name + "/images/*.nrrd"))

        # Select data if using subsample
        if args.use_subsample:
            inputMeshes =[sw.Mesh(filename) for filename in mesh_files]
            sample_idx = sw.data.sample_meshes(inputMeshes, int(args.num_subsample))
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
        1. Reflect meshes
        2. Turn meshes to volumes
        3. Isotropic resampling
        4. Padding
        5. Center of Mass Alignment
        6. Centering
        7. Reference selection
        8. Rigid Alignment
        9. Find largest bounding box
        10. Crop
        11. Convert to distance transform

        For more information on grooming see docs/workflow/groom.md
        http://sciinstitute.github.io/ShapeWorks/workflow/groom.html
        """

        # Create a directory for groomed output
        groom_dir = output_directory + 'groomed/'
        if not os.path.exists(groom_dir):
            os.makedirs(groom_dir)

        # Set reference side
        ref_side = "R" # selected so there are less to reflect
        if args.tiny_test:
            ref_side = "L" # selected so reflection happens in tiny test
        # Set cutting plane
        cutting_plane_points = np.array(
            [[-1.0, -1.0, -36.0], [1.0, -1.0, -36.0], [-1.0, 1.0, -36.0]])

        # BEGIN GROOMING WITH IMAGES
        if args.groom_images and image_files:
            """
            First, we need to loop over the mesh files and load the meshes
            """
            # list of meshes
            mesh_list = []
            # list of shape names (shape files prefixes) to be used for saving outputs
            names = []
            for mesh_filename in mesh_files:
                print('Loading: ' + mesh_filename)
                # get current shape name
                names.append(os.path.basename(mesh_filename).replace('.ply', ''))
                # load mesh
                mesh = sw.Mesh(mesh_filename)
                # append to the mesh list
                mesh_list.append(mesh)

            image_dir = groom_dir + 'images/'
            if not os.path.exists(image_dir):
                os.makedirs(image_dir)
            """
            Now we can loop over the meshes and apply the initial grooming steps to them
            """
            image_list = []
            for mesh, name in zip(mesh_list, names):
                """
                Grooming Step 1: Reflect - We have left and right femurs, so we reflect the non-reference side
                meshes so that all of the femurs can be aligned.
                """
                prefix = name.split("_")[0]
                for index in range(len(image_files)):
                    if prefix in image_files[index]:
                        corresponding_image_file = image_files[index]
                        break
                if ref_side in name:
                    print("Reflecting " + name)
                    # Get corresponding image and reflect
                    img1 = sw.Image(corresponding_image_file)
                    img2 = sw.Image(corresponding_image_file)
                    img2.recenter()
                    center = img2.origin() - img1.origin()
                    img1.reflect(sw.X)
                    image_list.append(img1)
                    # Reflect mesh
                    mesh.reflect(sw.X, center)
                else:
                    # Get image
                    img = sw.Image(corresponding_image_file)
                    image_list.append(img)

            seg_list = []
            seg_dir = groom_dir + 'segmentations/'
            if not os.path.exists(seg_dir):
                os.makedirs(seg_dir)
            for mesh, image, name in zip(mesh_list, image_list, names):

                """
                Grooming Step 2: Turn meshes into segmentations
                """
                print('Turning ' + name + ' mesh to segmentation.')
                seg_file = seg_dir + name + '.nrrd'
                seg = mesh.toImage(spacing=image.spacing()).resize(
                    image.dims()).binarize()

                """
                Grooming Step 3: Apply isotropic resampling
                The segmentation and images are resampled independently to have uniform spacing.
                """
                print('Resampling segmentation: ' + name)
                # antialias for 30 iterations
                antialias_iterations = 30
                seg.antialias(antialias_iterations)
                # resample to isotropic spacing using linear interpolation
                iso_spacing = [1, 1, 1]
                seg.resample(iso_spacing, sw.InterpolationType.Linear)
                # make segmetnation binary again
                seg.binarize()
                print('Resampling image: ' + name)
                image.resample(iso_spacing, sw.InterpolationType.Linear)

                """
                Grooming Step 4: Apply padding
                """
                print('Padding segmentation: ' + name)
                seg.pad(10, 0)
                print('Padding image: ' + name)
                image.pad(10, 0)

                """
                Grooming Step 5: Center of mass alignment
                """
                print('Center of mass alignment for segmentation: ' + name)
                # compute the center of mass of this segmentation
                shape_center = seg.centerOfMass()
                # get the center of the image domain
                image_center = seg.center()
                # define the translation to move the shape to its center
                translation_vector = image_center - shape_center
                # perform antialias-translate-binarize
                seg.antialias(antialias_iterations).translate(
                    translation_vector).binarize()
                print('Center of mass alignment for image: ' + name)
                image.translate(translation_vector)

                """
                Grooming Step 6: Centering
                """
                print('Centering segmentation: ' + name)
                old_center = seg.center()
                seg.recenter()
                print('Centering image: ' + name)
                image.setOrigin(image.origin()-old_center)

                seg_list.append(seg)

            """
            Grooming Step 7: Select a reference
            This step requires breaking the loop to load all of the segmentations at once so the shape
            closest to the mean can be found and selected as the reference.
            """
            ref_index = sw.find_reference_image_index(seg_list)
            # Make a copy of the reference segmentation
            ref_seg = seg_list[ref_index].write(
                groom_dir + 'reference.nrrd')
            ref_name = names[ref_index]
            print("Reference found: " + ref_name)

            """
            Grooming Step 8: Rigid alignment
            This step rigidly aligns each shape to the selected references.
            Rigid alignment involves interpolation, hence we need to convert binary segmentations
            to continuous-valued images again. There are two steps:
                - computing the rigid transformation parameters that would align a segmentation
                to the reference shape
                - applying the rigid transformation to the segmentation
                - save the aligned images for the next step
            """

            # First antialias the reference segmentation
            ref_seg.antialias(antialias_iterations)
            # Set the alignment parameters
            iso_value = 1e-20
            icp_iterations = 200
            # Now loop through all the segmentations and apply rigid alignment
            for seg, image, name in zip(seg_list, image_list, names):
                print('Aligning ' + name + ' to ' + ref_name)
                # compute rigid transformation
                seg.antialias(antialias_iterations)
                rigidTransform = seg.createRigidRegistrationTransform(
                    ref_seg, iso_value, icp_iterations)
                # second we apply the computed transformation, note that shape_seg has
                # already been antialiased, so we can directly apply the transformation
                seg.applyTransform(rigidTransform,
                                         ref_seg.origin(),  ref_seg.dims(),
                                         ref_seg.spacing(), ref_seg.coordsys(),
                                         sw.InterpolationType.Linear)
                # then turn antialized-tranformed segmentation to a binary segmentation
                seg.binarize()

                image.applyTransform(rigidTransform,
                                         ref_seg.origin(),  ref_seg.dims(),
                                         ref_seg.spacing(), ref_seg.coordsys(),
                                         sw.InterpolationType.Linear)

            pickle.dump( [cutting_plane_points], open( output_directory + "groomed/groomed_pickle.p", "wb" ) )

            """
            Grooming Step 9: Finding the largest bounding box
            We want to crop all of the segmentations to be the same size, so we need to find
            the largest bounding box as this will contain all the segmentations. This requires
            loading all segmentation files at once.
            """
            # Compute bounding box - aligned segmentations are binary images, so an good iso_value is 0.5
            iso_value = 0.5
            seg_bounding_box = sw.ImageUtils.boundingBox(seg_list, iso_value)

            """
            Grooming Step 10: Apply cropping and padding
            Now we need to loop over the segmentations and crop them to the size of the bounding box.
            To avoid cropped segmentations to touch the image boundaries, we will crop then
            pad the segmentations.
                - Crop to bounding box size
                - Pad segmentations
            """
            # loop over segs to apply cropping and padding
            for seg, image, name in zip(seg_list, image_list, names):
                print('Cropping & padding: ' + name)
                seg.crop(seg_bounding_box).pad(10, 0)
                image.crop(seg_bounding_box).pad(10, 0)

                seg.write(seg_dir + name + '.nrrd')
                image.write(image_dir + name + '.nrrd')

            """
            Grooming Step 11: Converting segmentations to smooth signed distance transforms.
            The computeDT API needs an iso_value that defines the foreground-background interface, to create
            a smoother interface we first antialiasing the segmentation then compute the distance transform
            at the zero-level set. We then need to smooth the DT as it will have some remaining aliasing effect
            of binarization.
            So the steps are:
                - Antialias
                - Compute distance transform
                - Apply smoothing
                - Save the distance transform
            """

            # Define distance transform parameters
            iso_value = 0
            sigma = 1.3
            # Loop over segs and compute smooth DT
            for seg, name in zip(seg_list, names):
                print('Compute DT for segmentation: ' + name)
                seg.antialias(antialias_iterations).computeDT(
                    iso_value).gaussianBlur(sigma)
            # Save distance transforms
            dt_files = sw.utils.save_images(groom_dir + 'distance_transforms/', seg_list,
                                            names, extension='nrrd', compressed=True, verbose=True)

        # BEGIN GROOMING WITHOUT IMAGES
        else:
            """
            First, we need to loop over the mesh files and load the meshes
            """
            # list of shape segmentations
            mesh_list = []
            # list of shape names (shape files prefixes) to be used for saving outputs
            names = []
            for mesh_filename in mesh_files:
                print('Loading: ' + mesh_filename)
                # get current shape name
                names.append(os.path.basename(mesh_filename).replace('.ply', ''))
                # load mesh
                mesh = sw.Mesh(mesh_filename)
                # append to the mesh list
                mesh_list.append(mesh)

            """
            Now we can loop over the segmentations and apply the initial grooming steps to them
            """
            for mesh, name in zip(mesh_list, names):
                """
                Grooming Step 1: Reflect - We have left and right femurs, so we reflect the right
                meshes so that all of the femurs can be aligned.
                """
                if ref_side in name:
                    print("Reflecting " + name)
                    arr = mesh.center(); center = [arr[0], arr[1], arr[2]]
                    mesh.reflect(sw.X)

            seg_list = []
            seg_dir = groom_dir + 'segmentations/'
            if not os.path.exists(seg_dir):
                os.makedirs(seg_dir)
            for mesh, name in zip(mesh_list, names):

                """
                Grooming Step 2: Turn meshes into segmentations
                """
                print('Turning ' + name + ' mesh to segmentation.')
                seg_file = seg_dir + name + '.nrrd'
                seg = mesh.toImage(spacing=[1, 1, 1]).binarize()

                """
                Grooming Step 3: Apply padding
                """
                print('Padding segmentation: ' + name)
                seg.pad(10, 0)

                """
                Grooming Step 4: Center of mass alignment
                """
                print('Center of mass alignment for segmentation: ' + name)
                antialias_iterations = 30
                # compute the center of mass of this segmentation
                shape_center = seg.centerOfMass()
                # get the center of the image domain
                image_center = seg.center()
                # define the translation to move the shape to its center
                translation_vector = image_center - shape_center
                # perform antialias-translate-binarize
                seg.antialias(antialias_iterations).translate(
                    translation_vector).binarize()

                """
                Grooming Step 5: Centering
                """
                print('Centering segmentation: ' + name)
                seg.recenter()

                seg_list.append(seg)

            """
            Grooming Step 6: Select a reference
            This step requires breaking the loop to load all of the segmentations at once so the shape
            closest to the mean can be found and selected as the reference.
            """
            ref_index = sw.find_reference_image_index(seg_list)
            # Make a copy of the reference segmentation
            ref_seg = seg_list[ref_index].write(
                groom_dir + 'reference.nrrd')
            ref_name = names[ref_index]
            print("Reference found: " + ref_name)

            """
            Grooming Step 7: Rigid alignment
            This step rigidly aligns each shape to the selected references.
            Rigid alignment involves interpolation, hence we need to convert binary segmentations
            to continuous-valued images again. There are two steps:
                - computing the rigid transformation parameters that would align a segmentation
                to the reference shape
                - applying the rigid transformation to the segmentation
                - save the aligned images for the next step
            """

            # First antialias the reference segmentation

            ref_seg.antialias(antialias_iterations)
            # Set the alignment parameters
            iso_value = 1e-20
            icp_iterations = 200
            # Now loop through all the segmentations and apply rigid alignment
            for seg, name in zip(seg_list, names):
                print('Aligning ' + name + ' to ' + ref_name)
                # compute rigid transformation
                seg.antialias(antialias_iterations)
                rigidTransform = seg.createRigidRegistrationTransform(
                    ref_seg, iso_value, icp_iterations)
                # second we apply the computed transformation, note that shape_seg has
                # already been antialiased, so we can directly apply the transformation
                seg.applyTransform(rigidTransform,
                                         ref_seg.origin(),  ref_seg.dims(),
                                         ref_seg.spacing(), ref_seg.coordsys(),
                                         sw.InterpolationType.Linear)
                # then turn antialized-tranformed segmentation to a binary segmentation
                seg.binarize()

            pickle.dump( [cutting_plane_points], open( output_directory + "groomed/groomed_pickle.p", "wb" ) )

            """
            Grooming Step 8: Finding the largest bounding box
            We want to crop all of the segmentations to be the same size, so we need to find
            the largest bounding box as this will contain all the segmentations. This requires
            loading all segmentation files at once.
            """
            # Compute bounding box - aligned segmentations are binary images, so an good iso_value is 0.5
            iso_value = 0.5
            seg_bounding_box = sw.ImageUtils.boundingBox(seg_list, iso_value)

            """
            Grooming Step 9: Apply cropping and padding
            Now we need to loop over the segmentations and crop them to the size of the bounding box.
            To avoid cropped segmentations to touch the image boundaries, we will crop then
            pad the segmentations.
                - Crop to bounding box size
                - Pad segmentations
            """
            # loop over segs to apply cropping and padding
            for seg, name in zip(seg_list, names):
                print('Cropping & padding segmentation: ' + name)
                seg.crop(seg_bounding_box).pad(10, 0)

                seg.write(seg_dir + name + '.nrrd')

            """
            Grooming Step 10: Converting segmentations to smooth signed distance transforms.
            The computeDT API needs an iso_value that defines the foreground-background interface, to create
            a smoother interface we first antialiasing the segmentation then compute the distance transform
            at the zero-level set. We then need to smooth the DT as it will have some remaining aliasing effect
            of binarization.
            So the steps are:
                - Antialias
                - Compute distance transform
                - Apply smoothing
                - Save the distance transform
            """

            # Define distance transform parameters
            iso_value = 0
            sigma = 1.3
            # Loop over segs and compute smooth DT
            for seg, name in zip(seg_list, names):
                print('Compute DT for segmentation: ' + name)
                seg.antialias(antialias_iterations).computeDT(
                    iso_value).gaussianBlur(sigma)
            # Save distance transforms
            dt_files = sw.utils.save_images(groom_dir + 'distance_transforms/', seg_list,
                                            names, extension='nrrd', compressed=True, verbose=True)

    print("\nStep 3. Optimize - Particle Based Optimization\n")
    """
    Step 3: OPTIMIZE - Particle Based Optimization

    Now that we have the distance transform representation of data we create 
    the parameter files for the shapeworks particle optimization routine.
    For more details on the plethora of parameters for shapeworks please refer 
    to docs/workflow/optimze.md
    http://sciinstitute.github.io/ShapeWorks/workflow/optimize.html
    """

    # Make directory to save optimization output
    point_dir = output_directory + 'shape_models/' + args.option_set
    if not os.path.exists(point_dir):
        os.makedirs(point_dir)

    # Cutting planes
    cutting_planes = []
    cutting_plane_counts = []
    for i in range(len(dt_files)):
        cutting_planes.append(cutting_plane_points)
        cutting_plane_counts.append(1)

    # Create a dictionary for all the parameters required by optimization
    parameter_dictionary = {
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
        "debug_projection" : 0,
        "verbosity" : 0,
        "use_statistics_in_init" : 0,
        "adaptivity_mode": 0,
        "cutting_plane_counts": cutting_plane_counts,
        "cutting_planes": cutting_planes
    }   
    # If running a tiny test, reduce some parameters
    if args.tiny_test:
        parameter_dictionary["number_of_particles"] = 32
        parameter_dictionary["optimization_iterations"] = 25
        parameter_dictionary["iterations_per_split"] = 25
    # Run multiscale optimization unless single scale is specified
    if not args.use_single_scale:
        parameter_dictionary["use_shape_statistics_after"] = 64

    # Get data input (meshes if running in mesh mode, else distance transforms)
    parameter_dictionary["domain_type"], input_files = sw.data.get_optimize_input(dt_files, args.mesh_mode)

    # Execute the optimization function on distance transforms
    [local_point_files, world_point_files] = OptimizeUtils.runShapeWorksOptimize(
        point_dir, input_files, parameter_dictionary)

    # Prepare analysis XML
    analyze_xml = point_dir + "/femur_cut_analyze.xml"
    AnalyzeUtils.create_analyze_xml(analyze_xml, input_files, local_point_files, world_point_files)

    # If tiny test or verify, check results and exit
    AnalyzeUtils.check_results(args, world_point_files)

    print("\nStep 4. Analysis - Launch ShapeWorksStudio - sparse correspondence model.\n")
    """
    Step 4: ANALYZE - Shape Analysis and Visualization

    Now we launch studio to analyze the resulting shape model.
    For more information about the analysis step, see docs/workflow/analyze.md
    http://sciinstitute.github.io/ShapeWorks/workflow/analyze.html
    """
    AnalyzeUtils.launch_shapeworks_studio(analyze_xml)
