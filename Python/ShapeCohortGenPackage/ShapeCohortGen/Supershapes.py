import os
import math
import trimesh
import numpy as np
import matplotlib.tri as mtri
import shapeworks as sw
from ShapeCohortGen.CohortGenUtils import *

'''
Generates super shapes and saves PLY mesh form
'''
def generate(num_samples, out_dir, randomize_center=False, randomize_rotation=False, m=-1, start_id=0, size=12, numPoints=2048, n_distribution="chisquare"):
    meshDir = out_dir + "meshes/"
    make_dir(meshDir)
    if m == -1:
        randomize_m = True
    else:
        randomize_m = False
    i = 0
    meshes = []
    while i < num_samples:
        print("Generating shape " + str(i+1) + " out of " + str(num_samples))
        # Define shape params
        if randomize_m:
            m = np.random.randint(3,8)
        name = "id" + get_id_str(i+start_id) + "_ss" + str(m)
        if n_distribution == "chisquare":
            n1 = np.random.chisquare(4)
            n2 = np.random.chisquare(4)
            n3 = np.random.chisquare(4)
        elif n_distribution == "uniform":
            n1 = np.random.uniform(0.75,1.5)
            n2 = np.random.uniform(0.75,1.5)
            n3 = n2
        else:
            print("Error: n_distribution unimplemented.")
        a = 1
        b = 1
        X, Y, Z, triIndices = super_formula_3D(m, n1, n2, n3, a, b, numPoints)
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
            rotation = np.zeros(3)
        S = trimesh.transformations.scale_matrix(size, [0,0,0])
        shapeMesh = shapeMesh.apply_transform(S)
        # check size
        if max(max(shapeMesh.bounds[1]), max(abs(shapeMesh.bounds[0]))) > size*3:
            print(name + " too large.")
        else:
            # Save mesh as ply
            shapeMesh.export(meshDir + name + ".stl")
            sw.Mesh(meshDir + name + ".stl").write(meshDir + name + ".ply")
            os.remove(meshDir + name + ".stl")
            meshes.append(meshDir + name + ".ply")
            i += 1
    return meshes

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

# Sample points for a 2D supershape and return as np.ndarray
def sample_super_formula_2D(n_points, m, n1, n2, n3):
    pts = np.ndarray((n_points, 3))
    for i in range(n_points):
        theta = 2.0 * np.pi * i / n_points
        r = super_formula_2D(m, n1, n2, n3, 1.0, 1.0, theta)

        x = r * np.cos(theta)
        y = r * np.sin(theta)
        pts[i] = [x, y, 0.0]
    return pts

def generate_2D(n_samples, n_points, out_dir,
                m=6, n1_degree=None, n2_degree=None, n3_degree=None,
                default_n=5.0, seed=None, size=20):
    """
    Generate a set of 2D supershapes sampled with chi-square distribution

    n_samples: number of shapes to generate
    n_points:  number of points per shape
    out_dir:   directory to save supershapes (in .vtp format)
    m:         number of lobes
    n1_degree: degree for chi-square distribution. If set to None, uses a constant value of default_n
    n2_degree: degree for chi-square distribution. If set to None, uses a constant value of default_n
    n3_degree: degree for chi-square distribution. If set to None, uses a constant value of default_n
    default_n: constant value for n if not using chi-square distribution
    seed:      random number seed to obtain reproducible samples
    """

    if seed is not None:
        np.random.seed(seed)

    contour_dir = os.path.join(out_dir, 'contours')
    if not os.path.exists(contour_dir):
        os.makedirs(contour_dir)

    filenames = []
    for i in range(n_samples):
        n1 = default_n if n1_degree is None else np.random.chisquare(n1_degree)
        n2 = default_n if n2_degree is None else np.random.chisquare(n2_degree)
        n3 = default_n if n3_degree is None else np.random.chisquare(n3_degree)
        pts = sample_super_formula_2D(n_points, m, n1, n2, n3)
        pts = pts*size
        lines = sw.utils.compute_line_indices(n_points, is_closed=True)
        out_fname = os.path.join(contour_dir, f'{i:02d}.vtp')
        sw.utils.save_contour_as_vtp(pts, lines, out_fname)
        filenames.append(out_fname)
    return filenames
