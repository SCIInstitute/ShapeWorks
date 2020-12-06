import os
import math
import trimesh
import numpy as np
import matplotlib.tri as mtri
import subprocess
from ShapeCohortGen.CohortGenUtils import *

'''
Generates super shapes and saves mesh form
'''
def generate(num_samples, out_dir, m, start_id, size):
    # make folders
    meshDir= out_dir + "meshes/"
    make_dir(meshDir)
    # for implicit resolution
    unitCubeSize = 2 
    for i in range(num_samples):
        print("Generating shape " + str(i+1) + " out of " + str(num_samples))
        name = "id" + get_id_str(i+start_id) + "_ss" + str(m)
        # shape params
        n1 = np.random.uniform(0.5,1.5)
        n2 = np.random.uniform(0.5,1.5)
        n3 = n2
        a = 1
        b = 1
        X, Y, Z, triIndices = super_formula_3D(m, n1, n2, n3, a, b)
       	verts = np.column_stack((X,Y,Z))
        shapeMesh = trimesh.Trimesh(vertices=verts, faces=triIndices)
        matrix = np.array([[size,0,0,0], [0,size,0,0], [0,0,size,0],[0,0,0,1]])
        shapeMesh = shapeMesh.apply_transform(matrix)
        shapeMesh.export(meshDir + name + ".stl")
        execCommand = ["stl2ply", meshDir + name + ".stl", meshDir + name + ".ply"]
        subprocess.check_call(execCommand)
        os.remove(meshDir + name + ".stl")
    return get_files(meshDir)

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

def get_id_str(num):
	string = str(num)
	while len(string) < 4:
		string = '0' + string
	return(string)

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

def super_formula_2D(m, n1, n2, n3, a, b, theta):
	r = abs((1 / a) * np.cos(m * theta / 4.0))**n2  +  abs((1 / b) * np.sin(m * theta / 4.0))**n3
	return r**(-1 / n1)
