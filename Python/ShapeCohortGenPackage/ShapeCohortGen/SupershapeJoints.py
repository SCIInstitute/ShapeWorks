 
import vtk
import numpy as np
from operator import add
from ShapeCohortGen.CohortGenUtils import *
from ShapeCohortGen.Ellipsoids import *
import os
import math
import trimesh
import numpy as np
import matplotlib.tri as mtri
import shapeworks as sw

'''
Generates super shapes and saves mesh form
'''

class Domain:
    def __init__(self, m, n1, n2, n3, a, b, size, center):
        self.m = m
        self.n1 = n1
        self.n2 = n2
        self.n3 = n3
        self.a = a
        self.b = b
        self.size = size
        self.center = center

def generate(out_dir):
    meshDir= out_dir + "/super-shapes-joint-meshes/"
    make_dir(meshDir)
    num_domain = 5
    num_Samples = 10
    # x_pos = [-200, -100, 0.1, 100, 200] # num_domain
    # x_pos = [-100, -50, 25, 75, 100] # num_domain
    x_pos = [-150, -75, 25, 100, 250]
    coeff_ar = []
    a = 0.001
    for i in range(num_Samples):
        a += 0.001
        coeff_ar.append(a)
    
    n_ar = []
    n = 3
    for i in range(num_Samples):
        n += 2
        n_ar.append(n)
    
    sz = 30
    for i in range(num_Samples):
        d1 = Domain(3, 30, n_ar[i], n_ar[i], 1, 1, sz, (x_pos[0], x_pos[0] * x_pos[0]* coeff_ar[i], 0))
        d2 = Domain(3, 30, n_ar[i], n_ar[i], 1, 1, sz, (x_pos[1], x_pos[1] * x_pos[1]* coeff_ar[i], 0))
        d3 = Domain(3, 30, n_ar[i], n_ar[i], 1, 1, sz, (x_pos[2], x_pos[2] * x_pos[2]* coeff_ar[i], 0))
        d4 = Domain(3, 30, n_ar[i], n_ar[i], 1, 1, sz, (x_pos[3], x_pos[3] * x_pos[3]* coeff_ar[i], 0))
        d5 = Domain(3, 30, n_ar[i], n_ar[i], 1, 1, sz, (x_pos[4], x_pos[4] * x_pos[4]* coeff_ar[i], 0))

        
        # d2 = Domain(4, 15, n_ar[i], n_ar[i], 1, 1, sz, (x_pos[1], x_pos[1] * x_pos[1]* coeff_ar[i], 0))
        # d3 = Domain(2, 20, n_ar[i], n_ar[i], 1, 1, sz, (x_pos[2], x_pos[2] * x_pos[2]* coeff_ar[i], 0))
        # d4 = Domain(4, 30, n_ar[i], n_ar[num_Samples-1-i], 1, 1, sz, (x_pos[3], x_pos[3] * x_pos[3]* coeff_ar[i], 0))
        # d5 = Domain(3, 15, n_ar[num_Samples-1-i], n_ar[i], 1, 1, sz, (x_pos[4], x_pos[4] * x_pos[4]* coeff_ar[i], 0))
        generate_supershape(meshDir, f"sample_{i+1}_d1", False, d1)
        generate_supershape(meshDir, f"sample_{i+1}_d2", False, d2)
        generate_supershape(meshDir, f"sample_{i+1}_d3", False, d3)
        generate_supershape(meshDir, f"sample_{i+1}_d4", False, d4)
        generate_supershape(meshDir, f"sample_{i+1}_d5", False, d5)
        print(f"Generated sample {i+1}")
    print("DONE")

def generate_supershape(meshDir, shape_name, randomize_rotation, domain_shape):
    # Define shape params
    X, Y, Z, triIndices = super_formula_3D(domain_shape.m, domain_shape.n1, domain_shape.n2, domain_shape.n3, domain_shape.a, domain_shape.b)
    verts = np.column_stack((X,Y,Z))
    # Generate shape
    shapeMesh = trimesh.Trimesh(vertices=verts, faces=triIndices)
    # Apply transform
    if randomize_rotation:
        rotation = np.random.rand(3)
    else:
        rotation = np.zeros(3)
    S = trimesh.transformations.scale_matrix(domain_shape.size, [0,0,0])
    T = trimesh.transformations.translation_matrix(domain_shape.center)
    R = trimesh.transformations.random_rotation_matrix(rotation)
    transform_matrix = trimesh.transformations.concatenate_matrices(T, R, S)
    shapeMesh = shapeMesh.apply_transform(transform_matrix)
    shapeMesh.export(f"{meshDir}/{shape_name}.stl")

# Shape generation helper
def super_formula_3D(m, n1, n2, n3, a, b, numPoints=15000):
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

generate('/home/sci/nawazish.khan/Desktop/papers/meshes/')