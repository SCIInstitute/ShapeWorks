import os
import math
import trimesh
import numpy as np
import matplotlib.tri as mtri
from ShapeCohortGen.CohortGenUtils import *

'''
Generates super shapes and saves mesh form
'''
def generate(num_samples, out_dir, m, start_id, size):
    # make folders
    implicitDir = outDir + "implicit/"
    make_dir(implicitDir)
    meshDir= outDir + "mesh/"
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
        X, Y, Z, triIndices = super_formula_3D(m, n1, n2, n3, a, b, size)
       	verts = np.column_stack((X,Y,Z))
        shapeMesh = trimesh.Trimesh(vertices=verts, faces=triIndices)
        ply_mesh = trimesh.exchange.ply.export_ply(shapeMesh, encoding='ascii')
        ply_mesh_file = open(meshDir + name + '.ply', 'wb')
        ply_mesh_file.write(ply_mesh)
        ply_mesh_file.close()
    return meshDir

def get_id_str(num):
	string = str(num)
	while len(string) < 4:
		string = '0' + string
	return(string)

def super_formula_3D(m, n1, n2, n3, a, b, numPoints):
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
