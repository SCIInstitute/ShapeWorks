# -*- coding: utf-8 -*-
"""
====================================================================
Example ShapeWorks Pipeline for mesh optimization
====================================================================
"""
import os
import glob
import shapeworks as sw
import OptimizeUtils
import AnalyzeUtils
import subprocess
import shutil
def Run_Pipeline(args):
    print("\nStep 1. Extract Data\n")
    """
    Step 1: EXTRACT DATA
    We define dataset_name which determines which dataset to download from 
    the portal and the directory to save output from the use case in. 
    """
    dataset_name = "ellipsoid_1mode"
    output_directory = "Output/ellipsoid_mesh_proj/"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    # If running a tiny_test, then download subset of the data
    if args.tiny_test:
        args.use_single_scale = 1
        sw.data.download_subset(args.use_case, dataset_name, output_directory)
        mesh_files = sorted(glob.glob(output_directory +
                            dataset_name + "/meshes/*.vtk"))[:3]
    # else download the entire dataset
    else:
        sw.data.download_and_unzip_dataset(dataset_name, output_directory)
        mesh_files = sorted(glob.glob(output_directory +
                            dataset_name + "/meshes/*.vtk"))

        # Select data if using subsample
        if args.use_subsample:
            inputMeshes =[sw.Mesh(filename) for filename in mesh_files]
            sample_idx = sw.data.sample_meshes(inputMeshes, int(args.num_subsample))
            mesh_files = [mesh_files[i] for i in sample_idx]

    # If skipping grooming, use the pregroomed meshes from the portal
    if args.skip_grooming:
        print("Skipping grooming.")
        out_directory = output_directory + dataset_name + '/groomed/meshes/'
        indices = []
        if args.tiny_test:
            indices = [0, 1, 2]
        elif args.use_subsample:
            indices = sample_idx
        mesh_files = sw.data.get_file_list(
            out_directory, ending=".vtk", indices=indices)

    # Else groom the meshes for optimization
    else:
        print("\nStep 2. Groom - Data Pre-processing\n")
        """
        Step 2: GROOMING 
        The required grooming steps are: 
        1. Remesh 
        2. Reference selection
        3. Rigid Alignment
        For more information on grooming see docs/workflow/groom.md
        http://sciinstitute.github.io/ShapeWorks/workflow/groom.html
        """

        # Create a directory for groomed output
        groom_dir = output_directory + 'groomed/'
        if not os.path.exists(groom_dir):
            os.makedirs(groom_dir)

        """
        First, we need to loop over the mesh files and load them
        """
        mesh_list = []
        mesh_names = []
        for mesh_file in mesh_files:
            print('Loading: ' + mesh_file)
            # get current name
            mesh_names.append(mesh_file.split('/')[-1].replace('.vtk', ''))
            """
            Grooming Step 1: load mesh and remesh
            """
            mesh = sw.Mesh(mesh_file).remeshPercent(percentage=99, adaptivity=1.0)
            # append to the list
            mesh_list.append(mesh)

        """
        Grooming Step 2: Select a reference
        This step requires loading all of the meshes at once so the shape
        closest to the mean can be found and selected as the reference. 
        """
        ref_index = sw.find_reference_mesh_index(mesh_list)
        # Make a copy of the reference segmentation 
        ref_mesh = mesh_list[ref_index]
        # Center the reference mesh at 0,0,0
        # ref_mesh.translate(-ref_mesh.center()).write(groom_dir + 'reference.vtk')
        ref_mesh.write(groom_dir + 'reference.vtk')
        ref_name = mesh_names[ref_index]
        print("Reference found: " + ref_name)

        """
        Grooming Step 3: Rigid alignment
        This step rigidly aligns each shape to the selected reference. 
        """
        Rigid_transforms = []
        for mesh, name in zip(mesh_list, mesh_names):
            print('Aligning ' + name + ' to ' + ref_name)
            # compute rigid transformation
            rigid_transform = mesh.createTransform(ref_mesh, sw.Mesh.AlignmentType.Rigid, 100)
            # apply rigid transform
            # mesh.applyTransform(rigid_transform)
            Rigid_transforms.append(rigid_transform)

        # Save groomed meshes
        mesh_files = sw.utils.save_meshes(groom_dir + 'meshes/', mesh_list, mesh_names, extension='vtk')


    print("\nStep 3. Optimize - Particle Based Optimization\n")
    """
    Step 3: OPTIMIZE - Particle Based Optimization
    Now we can run optimization directly on the meshes.
    For more details on the plethora of parameters for shapeworks please refer 
    to docs/workflow/optimze.md
    http://sciinstitute.github.io/ShapeWorks/workflow/optimize.html
    """

    # Make directory to save optimization output
    point_dir = output_directory + 'shape_models/' + args.option_set
    if not os.path.exists(point_dir):
        os.makedirs(point_dir)



    subjects = []
    number_domains = 1
    mesh_dir = output_directory+"/ellipsoid_1mode/meshes/"
    for i in range(len(mesh_names)):
        # print(shape_names[i])
        subject = sw.Subject()
        subject.set_number_of_domains(number_domains)
        # subject.set_segmentation_filenames([groomed_seg_files[i]])
        subject.set_segmentation_filenames([mesh_dir+mesh_names[i]+".vtk"])
        subject.set_groomed_filenames([mesh_files[i]])
        transform = Rigid_transforms[i]
        transforms = [ transform.flatten() ]
        subject.set_groomed_transforms(transforms)
        print(subject.get_groomed_transforms())
        subjects.append(subject)

    project = sw.Project()
    project.set_subjects(subjects)
    parameters = sw.Parameters()
    # Create a dictionary for all the parameters required by optimization
    parameter_dictionary = {
        "number_of_particles": 128,
        "use_normals": 0,
        "normal_weight": 10.0,
        "checkpointing_interval": 1000,
        "keep_checkpoints": 0,
        "iterations_per_split": 1000,
        "optimization_iterations": 1000,
        "starting_regularization": 10,
        "ending_regularization": 1,
        "recompute_regularization_interval": 1,
        "domains_per_shape": 1,
        
        "relative_weighting": 1,
        "initial_relative_weighting": 0.05,
        "procrustes_interval": 0,
        "procrustes_scaling": 0,
        "save_init_splits": 0,
        "verbosity": 0
    }

    for key in parameter_dictionary:
            parameters.set(key,sw.Variant([parameter_dictionary[key]]))
    parameters.set("domain_type",sw.Variant('mesh'))
    project.set_parameters("optimze",parameters)
    project.save("ellipsoid_mesh.xlsx")
    

    opt = sw.Optimize()
    opt.SetUpOptimize(project)
    opt.Run()
    project.save("ellipsoid_mesh.xlsx")

    AnalysisCmd = 'ShapeWorksStudio ellipsoid_mesh.xlsx'.split()
    subprocess.check_call(AnalysisCmd)
