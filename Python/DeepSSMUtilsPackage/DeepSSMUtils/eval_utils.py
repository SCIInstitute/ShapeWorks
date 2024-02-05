# Jadie Adams
import os
import re
import shutil
import subprocess
import numpy as np
import shapeworks as sw


def get_distance_meshes(out_dir, DT_dir, prediction_dir, mean_prefix):
    # Step 1: Get list of test names
    test_names = []
    for file in os.listdir(prediction_dir):
        test_names.append(get_prefix(file))
    # Step 2: Get original meshes from test distance transforms
    print("\n\nGetting original meshes from distance transforms...")
    DT_files = []
    for file in os.listdir(DT_dir):
        prefix = get_prefix(file)
        if prefix in test_names:
            DT_files.append(DT_dir + '/' + file)
    orig_mesh_list = sorted(get_mesh_from_DT(DT_files, out_dir + "OriginalMeshes/"))
    # Step 3: Get predicted meshes from predicted particles
    print("\n\nGetting meshes from predicted particles...")
    particle_files = []
    for file in os.listdir(prediction_dir):
        particle_files.append(prediction_dir + file)
    template_mesh = mean_prefix + "_dense.vtk"
    template_points = mean_prefix + "_sparse.particles"
    pred_mesh_list = sorted(
        get_mesh_from_particles(particle_files, out_dir + "PredictedMeshes/", template_points, template_mesh))
    # Step 4: Get distance between original and predicted mesh
    print("\n\nGetting distance between original and predicted meshes...")
    orig_dist_mesh_list, pred_dist_mesh_list, avg_distance = surface_to_surface_distance(orig_mesh_list, pred_mesh_list,
                                                                                         out_dir)
    print("Done.\n")
    return avg_distance


def get_prefix(path):
    file_name = os.path.basename(path)
    file_name = file_name.replace("predicted_ft_", "")
    prefix = "_".join(file_name.split("_")[0:2])
    prefix = prefix.split(".")[0]
    return prefix


def get_mesh_from_DT(DT_list, mesh_dir):
    if not os.path.exists(mesh_dir):
        os.makedirs(mesh_dir)
    mesh_files = []
    xml_filename = mesh_dir + "temp.xml"
    for input_file in DT_list:
        print('	' + get_prefix(input_file))
        output_vtk = mesh_dir + "original_" + get_prefix(input_file) + ".vtk"
        image = sw.Image(input_file)
        image.toMesh(isovalue=0).write(output_vtk)
        mesh_files.append(output_vtk)
    return sorted(mesh_files)


def get_mesh_from_particles(particle_list, mesh_dir, template_particles, template_mesh, planes=None):
    if not os.path.exists(mesh_dir):
        os.makedirs(mesh_dir)

    warp = sw.MeshWarper()
    sw_mesh = sw.Mesh(template_mesh)
    sw_particles = np.loadtxt(template_particles)
    warp.generateWarp(sw_mesh, sw_particles)

    particle_dir = os.path.dirname(particle_list[0]) + '/'

    out_mesh_filenames = []
    for i in range(len(particle_list)):
        out_filename = particle_list[i].replace('.particles', '.vtk')
        out_filename = out_filename.replace(particle_dir, mesh_dir)
        out_mesh_filenames.append(out_filename)
        sw_particles = np.loadtxt(particle_list[i])
        out_mesh = warp.buildMesh(sw_particles)
        out_mesh.write(out_filename)

    if planes is not None:
        for index in range(len(out_mesh_filenames)):
            sw.Mesh(out_mesh_filenames[index]).clip(planes[index][0], planes[index][1], planes[index][2]).write(
                out_mesh_filenames[index])
    return out_mesh_filenames


def surface_to_surface_distance(orig_list, pred_list, out_dir):
    orig_out_dir = out_dir + "OriginalMeshesWithDistance/"
    if not os.path.exists(orig_out_dir):
        os.makedirs(orig_out_dir)
    pred_out_dir = out_dir + "PredictedMeshesWithDistance/"
    if not os.path.exists(pred_out_dir):
        os.makedirs(pred_out_dir)
    orig_out_list = []
    pred_out_list = []
    mean_distances = []
    for index in range(len(orig_list)):
        orig = orig_list[index]
        pred = pred_list[index]
        name = os.path.basename(orig).replace("original_", "")
        orig_out = orig_out_dir + name
        pred_out = pred_out_dir + name
        orig_out_list.append(orig_out)
        pred_out_list.append(pred_out)
        orig_mesh = sw.Mesh(orig)
        pred_mesh = sw.Mesh(pred)
        dist1 = sw.mean(orig_mesh.distance(pred_mesh).write(orig_out).getField("distance", Mesh.Point))
        dist2 = sw.mean(pred_mesh.distance(orig_mesh).write(pred_out).getField("distance", Mesh.Point))
        mean_distances.append(dist1)
        mean_distances.append(dist2)
    return orig_out_list, pred_out_list, np.mean(np.array(mean_distances))


def get_MSE(pred_particle_files, true_particle_files):
    MSEs = []
    for i in range(len(pred_particle_files)):
        pred_particles = np.loadtxt(pred_particle_files[i])
        true_particles = np.loadtxt(true_particle_files[i])
        MSEs.append(np.mean((pred_particles - true_particles) ** 2))
    MSEs = np.array(MSEs)
    return np.mean(MSEs), np.std(MSEs)


def get_mesh_distance(pred_particle_files, mesh_list, template_particles, template_mesh, out_dir, planes=None):
    # Step 1: Get predicted meshes from predicted particles
    pred_mesh_list = get_mesh_from_particles(pred_particle_files, out_dir + "/predicted_meshes/", template_particles,
                                             template_mesh, planes)
    print(f"pred_mesh_list: {pred_mesh_list}")
    # Step 2: Get distance between original and predicted mesh
    mean_distances = []
    for index in range(len(mesh_list)):
        print(f"Getting distance between {mesh_list[index]} and {pred_mesh_list[index]}")
        orig_mesh = sw.Mesh(mesh_list[index])
        if planes is not None:
            orig_mesh.clip(planes[index][0], planes[index][1], planes[index][2])
        pred_mesh = sw.Mesh(pred_mesh_list[index])
        orig_mesh.write("/tmp/orig_mesh.vtk")
        pred_mesh.write("/tmp/pred_mesh.vtk")
        val1 = np.mean(orig_mesh.distance(pred_mesh)[0])
        val2 = np.mean(pred_mesh.distance(orig_mesh)[0])
        print(f"val1: {val1}")
        print(f"val2: {val2}")

        mean_distances.append(np.mean(orig_mesh.distance(pred_mesh)[0]))
        mean_distances.append(np.mean(pred_mesh.distance(orig_mesh)[0]))
    return np.mean(np.array(mean_distances))
