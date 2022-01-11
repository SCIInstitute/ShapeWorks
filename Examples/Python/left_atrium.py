# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Statistical Shape Modeling with ShapeWorks
====================================================================

In this example, we provide unpre-processed left atrial dataset,
which consists of the LGE scan and its segmentation across some number of subjects.
Since the data set is not pre-processed, it requires all re-processing steps;
resampling, padding, alignment, etc. To run the ShapeWorks particle-based optimization,
the processed segmentation files are converted to the signed distance transform.
This example is set to serve as a test case for users who want to process the raw(gray-sclae)
images as well as their binary segmentation images.

In this example, the raw and segmentation data are sharing the same functions for pre-processing,
such as the same transformation matrices for the center of mass and rigid alignment or same bounding box for cropping.

First import the necessary modules
"""
import os
import glob
import shapeworks as sw
import OptimizeUtils
import AnalyzeUtils


def Run_Pipeline(args):
    print("\nStep 1. Extract Data\n")
    """
    Step 1: EXTRACT DATA
    We define dataset_name which determines which dataset to download from 
    the portal and the directory to save output from the use case in. 
    """
    dataset_name = "left_atrium-v0"
    output_directory = "Output/left_atrium/"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    # If running a tiny_test, then download subset of the data
    if args.tiny_test:
        args.use_single_scale = 1
        sw.data.download_subset(args.use_case, dataset_name, output_directory)
        file_list_img = sorted(
            glob.glob(output_directory + dataset_name + "/images/*.nrrd"))[:3]
        file_list_seg = sorted(
            glob.glob(output_directory + dataset_name + "/segmentations/*.nrrd"))[:3]

    # Else download the entire dataset
    else:
        sw.data.download_and_unzip_dataset(dataset_name, output_directory)
        file_list_img = sorted(
            glob.glob(output_directory + dataset_name + "/images/*.nrrd"))
        file_list_seg = sorted(
            glob.glob(output_directory + dataset_name + "/segmentations/*.nrrd"))

        # Select representative data if using subsample
        if args.use_subsample:
            inputSegs =[sw.Image(filename) for filename in file_list_seg]
            sample_idx = sw.data.sample_images(inputSegs, int(args.num_subsample))
            file_list_seg = [file_list_seg[i] for i in sample_idx]
            file_list_img = [file_list_img[i] for i in sample_idx]
        else:
            sample_idx = []

    # If skipping grooming, use the pregroomed distance transforms from the portal
    if args.skip_grooming:
        print("Skipping grooming.")
        dtDirecory = output_directory + dataset_name + '/groomed/distance_transforms/'
        indices = []
        if args.tiny_test:
            indices = [0, 1, 2]
        elif args.use_subsample:
            indices = sample_idx
        dt_files = sw.data.get_file_list(
            dtDirecory, ending=".nrrd", indices=indices)

    # Else groom the segmentations and get distance transforms for optimization
    else:
        print("\nStep 2. Groom - Data Pre-processing\n")
        """
        Step 2: GROOMING 
        
        The required grooming steps are: 
        1. Isotropic resampling
        2. Centering 
        3. Padding
        4. Center of Mass Alignment
        5. Find refernce image
        6. Rigid Alignment
        7. Find largest bounding box
        8. Apply cropping and padding
        9. Create smooth signed distance transforms

        For more information on grooming see docs/workflow/groom.md
        http://sciinstitute.github.io/ShapeWorks/workflow/groom.html
        """

        # Create a directory for groomed output
        groom_dir = output_directory + 'groomed/'
        if not os.path.exists(groom_dir):
            os.makedirs(groom_dir)

        """
        First, we need to loop over the shape segmentation files and load the segmentations
        """
        # list of shape segmentations
        shape_seg_list = []
        # list of shape names (shape files prefixes) to be used for saving outputs
        shape_seg_names = []
        for shape_filename in file_list_seg:
            print('Loading: ' + shape_filename)
            # get current shape name
            shape_seg_names.append(shape_filename.split('/')
                                   [-1].replace('.nrrd', ''))
            # load segmentation
            shape_seg = sw.Image(shape_filename)
            # append to the shape list
            shape_seg_list.append(shape_seg)

        """
        If we are grooming with images, we need to loop over the image files and laod the images
        """
        if args.groom_images and file_list_img:

            # list of shape images
            shape_img_list = []
            # list of shape names (shape files prefixes) to be used for saving outputs
            shape_img_names = []
            for shape_filename in file_list_img:
                print('Loading: ' + shape_filename)
                # get current shape name
                shape_img_names.append(shape_filename.split('/')
                                       [-1].replace('.nrrd', ''))
                # load image
                shape_image = sw.Image(shape_filename)
                # append to the shape image list
                shape_img_list.append(shape_image)
            """
            If we are grooming with images, we will perform the same grooming operations
            on images

            """

            """
            Now we can loop over the images and apply the initial grooming steps to themm
            """
            print("\nPerforming resampling, centering,and padding operation on images\n")
            for shape_img, shape_name in zip(shape_img_list, shape_img_names):

                """
                Grooming Step 1: Resample images to have isotropic (uniform) spacing
                    - Antialiase the images to convert it to a smooth continuous-valued 
                    image for interpolation
                    - Resample the antialiased image using the same voxel spacing for all dimensions
                    - Binarize the resampled images to results in a binary image with an 
                    isotropic voxel spacing
                """
                print('Resampling image: ' + shape_name)
                # antialias for 30 iterations
                antialias_iterations = 30
                shape_img.antialias(antialias_iterations)
                # resample to isotropic spacing using linear interpolation
                iso_spacing = [1, 1, 1]
                shape_img.resample(iso_spacing, sw.InterpolationType.Linear)
                # make segmetnation binary again
                shape_img.binarize()

                """
                Grooming Step 2:Recenter the image

                """
                print('Recentering image: ' + shape_name)
                shape_img.recenter()

                """
                Grooming Step 3: Padding the image

                """
                print('Padding image: ' + shape_name)
                # parameters for padding
                padding_size = 10  # number of voxels to pad for each dimension
                padding_value = 0  # the constant value used to pad the segmentations
                shape_img.pad(padding_size, padding_value)

        """
        Now we can loop over the segmentations and apply the initial grooming steps to them
        """
        print("\nPerforming resampling, centering,and padding operation on segmentations\n")
        for shape_seg, shape_name in zip(shape_seg_list, shape_seg_names):

            """
            Grooming Step 1: Resample segmentations to have isotropic (uniform) spacing
                - Antialiase the binary segmentation to convert it to a smooth continuous-valued 
                image for interpolation
                - Resample the antialiased image using the same voxel spacing for all dimensions
                - Binarize the resampled images to results in a binary segmentation with an 
                isotropic voxel spacing
            """
            print('Resampling segmentation: ' + shape_name)
            # antialias for 30 iterations
            antialias_iterations = 30
            shape_seg.antialias(antialias_iterations)
            # resample to isotropic spacing using linear interpolation
            iso_spacing = [1, 1, 1]
            shape_seg.resample(iso_spacing, sw.InterpolationType.Linear)
            # make segmetnation binary again
            shape_seg.binarize()

            """
            Grooming Step 2:Recenter the segmentation image

            """
            print('Recentering segmentation: ' + shape_name)
            shape_seg.recenter()

            """
            Grooming Step 3: Padding the segmentation image

            """
            print('Padding segmentation: ' + shape_name)
            # parameters for padding
            padding_size = 10  # number of voxels to pad for each dimension
            padding_value = 0  # the constant value used to pad the segmentations
            shape_seg.pad(padding_size, padding_value)

        """
        Grooming Step 4: Center of mass alignment 
        This step translates the center of mass of the shape to the center of the 3D volume space
        as a precursor for rigid alignment. It involves:
            - Finding the center of mass of the segmentation
            - Finding the center of the image domain
            - Defining the translation vector
            - Applying the translation by antialiasing, translating, and binarizing. 
            (Because this step involves interpolation, we must antialias before and 
            binarize after as we did when resampling.)
        """
        print('\n')
        for i in range(len(shape_seg_list)):
            shape_name = shape_seg_names[i]
            shape_seg = shape_seg_list[i]
            print('Center of mass alignment: ' + shape_name)
            # compute the center of mass of this segmentation
            shape_center = shape_seg.centerOfMass()
            # get the center of the image domain
            image_center = shape_seg.center()
            # define the translation to move the shape to its center
            translationVector = image_center - shape_center
            # perform antialias-translate-binarize
            shape_seg.antialias(antialias_iterations).translate(
                translationVector).binarize()
            # if we are grooming the images, appply the COM alignment to the corresponding images
            if(args.groom_images):
                shape_img_name = shape_img_names[i]
                shape_img = shape_img_list[i]
                print(
                    "Center of mass alignment for corresponding image: " + shape_img_name)

                # perform antialias-translate-binarize
                shape_img.antialias(antialias_iterations).translate(
                    translationVector).binarize()

        """
        Grooming Step 5: Select a reference
        This step requires breaking the loop to load all of the segmentations at once so the shape
        closest to the mean can be found and selected as the reference. 
        """
        print('\n')
        ref_index = sw.find_reference_image_index(shape_seg_list)
        # Make a copy of the reference segmentation
        ref_seg = shape_seg_list[ref_index].write(groom_dir + 'reference.nrrd')
        ref_name = shape_seg_names[ref_index]
        print("Reference found: " + ref_name)

        """
        Grooming Step 6: Rigid alignment
        This step rigidly aligns each shape to the selected references. 
        Rigid alignment involves interpolation, hence we need to convert binary segmentations 
        to continuous-valued images again. There are two steps:
            - computing the rigid transformation parameters that would align a segmentation 
            to the reference shape
            - applying the rigid transformation to the segmentation
            - save the aligned images for the next step
        """
        print('\n')
        # First antialias the reference segmentation
        ref_seg.antialias(antialias_iterations)
        # Set the alignment parameters
        iso_value = 1e-20
        icp_iterations = 200
        # Now loop through all the segmentations and apply rigid alignment
        for i in range(len(shape_seg_list)):
            shape_name = shape_seg_names[i]
            shape_seg = shape_seg_list[i]
            print('Aligning ' + shape_name + ' to ' + ref_name)
            # compute rigid transformation
            shape_seg.antialias(antialias_iterations)
            rigidTransform = shape_seg.createRigidRegistrationTransform(
                ref_seg, iso_value, icp_iterations)
            # second we apply the computed transformation, note that shape_seg has
            # already been antialiased, so we can directly apply the transformation
            shape_seg.applyTransform(rigidTransform,
                                     ref_seg.origin(),  ref_seg.dims(),
                                     ref_seg.spacing(), ref_seg.coordsys(),
                                     sw.InterpolationType.Linear)
            # then turn antialized-tranformed segmentation to a binary segmentation
            shape_seg.binarize()
            # If we are grooming the iamges, apply the rigid alignment to the corresponding images
            if(args.groom_images):
                shape_img_name = shape_img_names[i]
                shape_img = shape_img_list[i]
                print('Aligning the corresponding image: ' +
                      shape_img_name + ' to ' + ref_name)
                # second we apply the computed transformation,first we antialias and then
                # we can directly apply the transformation
                shape_img.antialias(antialias_iterations)
                shape_img.applyTransform(rigidTransform,
                                         ref_seg.origin(),  ref_seg.dims(),
                                         ref_seg.spacing(), ref_seg.coordsys(),
                                         sw.InterpolationType.Linear)
                # then turn antialized-tranformed segmentation to a binary image
                shape_img.binarize()

        """
        Grooming Step 7: Finding the largest bounding box
        We want to crop all of the segmentations to be the same size, so we need to find 
        the largest bounding box as this will contain all the segmentations. This requires 
        loading all segmentation files at once.
        """
        print('\n')
        # Compute bounding box - aligned segmentations are binary images, so an good iso_value is 0.5
        iso_value = 0.5
        segs_bounding_box = sw.ImageUtils.boundingBox(
            shape_seg_list, iso_value)

        """
        Grooming Step 8: Apply cropping and padding
        Now we need to loop over the segmentations and crop them to the size of the bounding box.
        To avoid cropped segmentations to touch the image boundaries, we will crop then 
        pad the segmentations.
            - Crop to bounding box size
            - Pad segmentations
        """

        # parameters for padding
        padding_size = 10  # number of voxels to pad for each dimension
        padding_value = 0  # the constant value used to pad the segmentations
        # loop over segs to apply cropping and padding
        for i in range(len(shape_seg_list)):
            shape_name = shape_seg_names[i]
            shape_seg = shape_seg_list[i]
            print('Cropping & padding segmentation: ' + shape_name)
            shape_seg.crop(segs_bounding_box).pad(padding_size, padding_value)

            # if we are grooming images, apply same cropping and padding to images
            if(args.groom_images):
                shape_img_name = shape_img_names[i]
                shape_img = shape_img_list[i]
                print('Cropping & padding corresponding image: ' + shape_img_name)
                shape_img.crop(segs_bounding_box).pad(
                    padding_size, padding_value)

        # Save groomed images
        if args.groom_images:
            print('\nSaving groomed images\n')
            sw.utils.save_images(groom_dir + 'images', shape_img_list,
                                 shape_img_names, extension='nrrd', compressed=True, verbose=True)
        """
        Grooming Step 9: Converting segmentations to smooth signed distance transforms.
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
        print('\n')
        # Define distance transform parameters
        iso_value = 0
        sigma = 1.3
        # Loop over segs and compute smooth DT
        for shape_seg, shape_name in zip(shape_seg_list, shape_seg_names):
            print('Compute DT for segmentation: ' + shape_name)
            shape_seg.antialias(antialias_iterations).computeDT(
                iso_value).gaussianBlur(sigma)
        # Save distance transforms
        dt_files = sw.utils.save_images(groom_dir + 'distance_transforms/', shape_seg_list,
                                        shape_seg_names, extension='nrrd', compressed=True, verbose=True)

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
    # Create a dictionary for all the parameters required by optimization
    parameter_dictionary = {
        "number_of_particles": 512,
        "use_normals": 0,
        "normal_weight": 10.0,
        "checkpointing_interval": 200,
        "keep_checkpoints": 0,
        "iterations_per_split": 4000,
        "optimization_iterations": 4000,
        "starting_regularization": 1000,
        "ending_regularization": 10,
        "recompute_regularization_interval": 2,
        "domains_per_shape": 1,
        "relative_weighting": 10,
        "domain_type": 'image',
        "initial_relative_weighting": 0.1,
        "procrustes_interval": 1,
        "procrustes_scaling": 1,
        "save_init_splits": 0,
        "verbosity": 0
    }
    # If running a tiny test, reduce some parameters
    if args.tiny_test:
        parameter_dictionary["number_of_particles"] = 32
        parameter_dictionary["optimization_iterations"] = 25
        parameter_dictionary["iterations_per_split"] = 25

    # Run multiscale optimization unless single scale is specified
    if not args.use_single_scale:
        parameter_dictionary["use_shape_statistics_after"] = 128

    # Get data input (meshes if running in mesh mode, else distance transforms)
    parameter_dictionary["domain_type"], input_files = sw.data.get_optimize_input(dt_files, args.mesh_mode)

    # Execute the optimization function on distance transforms
    [local_point_files, world_point_files] = OptimizeUtils.runShapeWorksOptimize(
        point_dir, input_files, parameter_dictionary)

    # Prepare analysis XML
    analyze_xml = point_dir + "/left_atrium_analyze.xml"
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

    print("\nStep 5. Analysis - Launch ShapeWorksStudio.\n")
    AnalyzeUtils.launch_shapeworks_studio(analyze_xml)
