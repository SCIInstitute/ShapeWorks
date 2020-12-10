import os
import math
import trimesh
import numpy as np
import matplotlib.tri as mtri
import subprocess
from ShapeCohortGen.CohortGenUtils import *

'''
Generates super shapes and saves PLY mesh form
'''
def generate(num_samples, out_dir, randomize_center, randomize_rotation, m, start_id, size):
    meshDir= out_dir + "meshes/"
    make_dir(meshDir)
    for i in range(num_samples):
        print("Generating shape " + str(i+1) + " out of " + str(num_samples))
        name = "id" + get_id_str(i+start_id) + "_ss" + str(m)
        # Define shape params
        n1 = np.random.uniform(0.5,1.5)
        n2 = np.random.uniform(0.5,1.5)
        n3 = n2
        a = 1
        b = 1
        X, Y, Z, triIndices = super_formula_3D(m, n1, n2, n3, a, b)
       	verts = np.column_stack((X,Y,Z))
        # Generate shape
        shapeMesh = trimesh.Trimesh(vertices=verts, faces=triIndices)
        # Apply transform
        if randomize_center:
            center_loc = list(np.random.randint(low = 0,high=30,size=3))
        else:
            center_loc = [0,0,0]
        if randomize_rotation:
            rotation = np.random.rand(3)
        else:
            rotation = 0
        S = trimesh.transformations.scale_matrix(size, [0,0,0])
        T = trimesh.transformations.translation_matrix(center_loc)
        R = trimesh.transformations.random_rotation_matrix(rotation)
        transform_matrix = trimesh.transformations.concatenate_matrices(T, R, S)
        shapeMesh = shapeMesh.apply_transform(transform_matrix)
        # Save mesh as ply
        shapeMesh.export(meshDir + name + ".stl")
        execCommand = ["stl2ply", meshDir + name + ".stl", meshDir + name + ".ply"]
        subprocess.check_call(execCommand)
        os.remove(meshDir + name + ".stl")
        VTKmesh = name + ".vtk"
        PLYmesh = name + ".ply"
        subprocess.check_call(["ply2vtk", PLYmesh, VTKmesh])
    return get_files(meshDir)

# Name helper
def get_id_str(num):
	string = str(num)
	while len(string) < 4:
		string = '0' + string
	return(string)

# Shape generation helper
def super_formula_3D(m, n1, n2, n3, a, b, numPoints=100000):
	numPointsRoot = round(math.sqrt(numPoints))
	theta = np.linspace(-math.pi, math.pi, endpoint=True, num=numPointsRoot)
	phi = np.linspace(-math.pi / 2.0, math.pi/2.0, endpoint=True, num=numPointsRoot)
	theta, phi = np.meshgrid(theta, phi)
	theta, phi = theta.flatten(), phi.flatten()
	r1 = super_formula_2D(m, n1, n2, n3, a, b, theta)
	r2 = super_formula_2D(m, n1, n2, n3, a, b, phi)
	x = r1 * r2 * np.cos(theta) * np.cos(phi)
	y = r1 * r2 * np.sin(theta) * np.cos(phi)
	z = r2 * np.sin(phi)
	tri = mtri.Triangulation(theta, phi)
	return x, y, z, tri.triangles

# Shape generation helper
def super_formula_2D(m, n1, n2, n3, a, b, theta):
	r = abs((1 / a) * np.cos(m * theta / 4.0))**n2  +  abs((1 / b) * np.sin(m * theta / 4.0))**n3
	return r**(-1 / n1)