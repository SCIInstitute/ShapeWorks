# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Statistical Shape Modeling with ShapeWorks
====================================================================
This example is set to serve as a test case for new ShapeWorks users, and each
step is explained in the shapeworks including the pre-processing, the 
optimization and, the post ShapeWorks visualization.

First import the necessary modules
"""
import os
import glob
import shapeworks as sw
from OptimizeUtils import *
from AnalyzeUtils import *
import CommonUtils
import NewUtils

def Run_Pipeline(args):
    """
    Unzip the data for this tutorial.

    The data is inside the Ellipsoids.zip, run the following function to unzip the 
    data and create necessary supporting files into the Data/ directory. 
    The files will be Extracted in a newly created directory: Output/Ellipsoids.
    """

    print("\nStep 1. Extract Data\n")
    if int(args.interactive) != 0:
        input("Press Enter to continue")
    # Get data
    datasetName = "ellipsoid-v1"
    outputDirectory = "Output/ellipsoid/"
    if not os.path.exists(outputDirectory):
        os.makedirs(outputDirectory)
    #If tiny_test then download subset of the data
    if args.tiny_test:
        args.use_single_scale = 1
        CommonUtils.download_subset(args.use_case,datasetName, outputDirectory)
        fileList = sorted(glob.glob(outputDirectory + datasetName + "/segmentations/*.nrrd"))[:3]
    #else download the entire dataset
    else:
        CommonUtils.download_and_unzip_dataset(datasetName, outputDirectory)

        fileList = sorted(glob.glob(outputDirectory + datasetName + "/segmentations/*.nrrd"))
    
    # Select data if using subsample
    if args.use_subsample:
        sample_idx = sampledata(fileList, int(args.num_subsample))
        fileList = [fileList[i] for i in sample_idx]
    else:
        sample_idx = []

    if args.skip_grooming:
        print("Skipping grooming.")
        dtDirecory = outputDirectory + datasetName + '/groomed/distance_transforms/'
        indices = []
        if args.tiny_test:
            indices = [0,1,2]
        elif args.use_subsample:
            indices = sample_idx
        dtFiles = CommonUtils.get_file_list(dtDirecory, ending=".nrrd", indices=indices)
    else:
        print("\nStep 2. Groom - Data Pre-processing\n")
        """
        The required grooming steps are: 
        1. Isotropic resampling
        2. Center of Mass Alignment
        3. Reference selection
        4. Rigid Alignment
        5. Find largest bounding box
        6. Apply cropping and padding
        7. Create smooth signed distance transforms
        For a detailed explanation of grooming steps see: /docs/workflow/groom.md
        """
        groomDir = outputDirectory + 'groomed/'
        if not os.path.exists(groomDir):
            os.makedirs(groomDir)

        """
        First, we need to loop over the shape segmentation files and load the segmentations
        """
        # list of shape segmentations
        shapeSegList = []
        # list of shape names (shape files prefixes) to be used for saving outputs
        shapeNames   = []
        for shapeFilename in fileList:
            print('Loading: ' + shapeFilename)
            # get current shape name
            shapeNames.append(shapeFilename.split('/')[-1].replace('.nrrd', ''))
            # load segmentation
            shapeSeg = sw.Image(shapeFilename)
            # append to the shape list
            shapeSegList.append(shapeSeg)

        """
        Now we can loop over the segmentations and apply the initial grooming steps to themm
        """
        for shapeSeg, shapeName in zip(shapeSegList, shapeNames):
            
            """
            Step 1: Resample segmentations to have isotropic (uniform) spacing
                - Antialiase the binary segmentation to convert it to a smooth continuous-valued image for interpolation
                - Resample the antialiased image using the same voxel spacing for all dimensions
                - Binarize the resampled images to results in a binary segmentation with an isotropic voxel spacing
            """
            print('Resampling segmentation: ' + shapeName)
            # antialias for 30 iterations
            antialias_iterations = 30   
            shapeSeg.antialias(antialias_iterations)
            # resample to isotropic spacing using linear interpolation
            iso_spacing = [1,1,1]
            shapeSeg.resample(iso_spacing, sw.InterpolationType.Linear)
            # make segmetnation binary again
            shapeSeg.binarize()

            """
            Step 2: Center of mass alignment 
            This step translates the center of mass of the shape to the center of the 3D volume space
            as a precursor for rigid alignment. It involves:
                - Finding the center of mass of the segmentation
                - Finding the center of the image domain
                - Defining the translation vector
                - Applying the translation by antialiasing, translating, and binarizing. (Because this step 
                involves interpolation, we must antialias before and binarize after as we did when resampling.)
            """
            print('Center of mass alignment: ' + shapeName)
            # compute the center of mass of this segmentation
            shapeCenter = shapeSeg.centerOfMass() 
            # get the center of the image domain
            imageCenter = shapeSeg.center() 
            # define the translation to move the shape to its center
            translationVector =  imageCenter - shapeCenter
            # perform antialias-translate-binarize 
            shapeSeg.antialias(antialias_iterations).translate(translationVector).binarize()

        """
        Step 3: Select a reference
        This step requires breaking the loop to load all of the segmentations at once so the shape
        closest to the mean can be found and selected as the reference. 
        """
        refIndex = NewUtils.findReferenceImageIndex(shapeSegList)
        # Make a copy of the reference segmentation
        refSeg = sw.Image(shapeSegList[refIndex].write(groomDir + 'reference.nrrd'))
        refName = shapeNames[refIndex]
        print("Reference found: " + refName)

        """
        Step 4: Rigid alignment
        This step rigidly aligns each shape to the selected references. Rigid alignment involves interpolation, 
        hence we need to convert binary segmentations to continuous-valued images again. There are two steps:
            - computing the rigid transformation parameters that would align a segmentation to the reference shape
            - applying the rigid transformation to the segmentation
            - save the aligned images for the next step
        """

        # First antialias the reference segmentation
        refSeg.antialias(antialias_iterations)

        # Set the alignment parameters
        isoValue       = 1e-20
        icp_iterations = 200

        # Now loop through all the segmentations and apply rigid alignment
        for shapeSeg, shapeName in zip(shapeSegList, shapeNames):
            print('Aligning ' + shapeName + ' to ' + refName) 
            # compute rigid transformation
            rigidTransform = sw.ImageUtils.createRigidRegistrationTransform(shapeSeg.antialias(antialias_iterations), 
                                                                                refSeg, isoValue, icp_iterations)
            # second we apply the computed transformation, note that shapeSeg has 
            # already been antialiased, so we can directly apply the transformation 
            shapeSeg.applyTransform(rigidTransform, 
                                    refSeg.origin(),  refSeg.dims(), 
                                    refSeg.spacing(), refSeg.coordsys(), 
                                    sw.InterpolationType.Linear)
            # then turn antialized-tranformed segmentation to a binary segmentation
            shapeSeg.binarize()

        """
        Step 5: Finding the largest bounding box
        We want to crop all of the segmentations to be the same size, so we need to find the largest bounding box
        as this will contain all the segmentations. This requires loading all segmentation files at once.
        """
        # Compute bounding box - aligned segmentations are binary images, so an good isovalue is 0.5
        isoValue = 0.5
        segsBoundingBox = sw.ImageUtils.boundingBox(shapeSegList, isoValue)

        """
        Step 6: Apply cropping and padding
        Now we need to loop over the segmentations and crop them to the size of the bounding box.
        To avoid cropped segmentations to touch the image boundaries, we will crop then pad the segmentations.
            - Crop to bounding box size
            - Pad segmentations
        """

        # parameters for padding 
        paddingSize  = 10 # number of voxels to pad for each dimension
        paddingValue = 0  # the constant value used to pad the segmentations
        # loop over segs to apply cropping and padding
        for shapeSeg, shapeName in zip(shapeSegList, shapeNames):
            print('Cropping & padding segmentation: ' + shapeName)    
            shapeSeg.crop(segsBoundingBox).pad(paddingSize, paddingValue)

        """
        Step 7: Converting segmentations to smooth signed distance transforms.
        The computeDT API needs an isovalue that defines the foreground-background interface, to create a smoother 
        interface we first antialiasing the segmentation then compute the distance transform at the zero-level set.
        We then need to smooth the DT as it will have some remaining aliasing effect of binarization. 
        So the steps are:
            - Antialias 
            - Compute distance transform
            - Apply smoothing
            - Save the distance transform
        """

        # Define distance transform parameters
        isoValue = 0
        sigma    = 1.3
        # Loop over segs and compute smooth DT
        for shapeSeg, shapeName in zip(shapeSegList, shapeNames):
            print('Compute DT for segmentation: ' + shapeName)    
            shapeSeg.antialias(antialias_iterations).computeDT(isoValue).gaussianBlur(sigma)
        # Save distance transforms
        dtFiles = NewUtils.saveImages(groomDir + 'distance_transforms/', shapeSegList, shapeNames, extension='nrrd', compressed=False, verbose=True)

    """
    ## OPTIMIZE : Particle Based Optimization

    Now that we have the distance transform representation of data we create 
    the parameter files for the shapeworks particle optimization routine.
    For more details on the plethora of parameters for shapeworks please refer 
    to docs/workflow/optimze.md
    First we need to create a dictionary for all the parameters required by this
    optimization routine
    """

    print("\nStep 4. Optimize - Particle Based Optimization\n")
    if int(args.interactive) != 0:
        input("Press Enter to continue")

    pointDir = outputDirectory + 'shape_models/'
    if not os.path.exists(pointDir):
        os.makedirs(pointDir)

    parameterDictionary = {
        "number_of_particles": 128,
        "use_normals": 1,
        "normal_weight": 15.0,
        "checkpointing_interval": 200,
        "keep_checkpoints": 0,
        "iterations_per_split": 2000,
        "optimization_iterations": 1000,
        "starting_regularization": 100,
        "ending_regularization": 10,
        "recompute_regularization_interval": 2,
        "domains_per_shape": 1,
        "domain_type": 'image',
        "relative_weighting": 15,
        "initial_relative_weighting": 0.05,
        "procrustes_interval": 0,
        "procrustes_scaling": 0,
        "save_init_splits": 0,
        "verbosity": 2
    }

    if args.tiny_test:
        parameterDictionary["number_of_particles"] = 32
        parameterDictionary["optimization_iterations"] = 25

    if not args.use_single_scale:
        parameterDictionary["use_shape_statistics_after"] = 32

    """
    Now we execute a single scale particle optimization function.
    """
    [localPointFiles, worldPointFiles] = runShapeWorksOptimize(pointDir, dtFiles, parameterDictionary)

    # if args.tiny_test:
    #     print("Done with tiny test")
    #     exit()
          
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
    """
    
    print("\nStep 5. Analysis - Launch ShapeWorksStudio - sparse correspondence model.\n")
    if args.interactive != 0:
        input("Press Enter to continue")

    launchShapeWorksStudio(pointDir, dtFiles, localPointFiles, worldPointFiles)
