"""Generate the correspondence-quality cutting-plane dataset (issue #2673).

Five ellipsoids that vary smoothly above z = 0 and wildly below it.  Each has a cutting plane at
z = 0 that keeps the top half, so the optimizer only places particles there.  A correspondence
quality evaluation that ignores the plane scores the bottom halves too, which no particle describes.

Run from this directory, then optimize:
    python make_dataset.py
    shapeworks optimize --name project.swproj
"""
import json
import os

import numpy as np
import vtk

# (rx, ry, rz) of the top half, then the bottom half's sideways shear (x, y) and vertical stretch
SUBJECTS = [
    ("ellipsoid_0", (10.0, 10.0, 10.0), (0.00, 0.00), 1.0),
    ("ellipsoid_1", (11.0, 9.5, 10.5), (0.10, -0.05), 1.6),
    ("ellipsoid_2", (9.5, 10.5, 9.0), (-0.08, 0.09), 0.6),
    ("ellipsoid_3", (10.5, 10.0, 11.0), (0.04, 0.12), 2.0),
    ("ellipsoid_4", (9.0, 9.0, 10.0), (-0.12, -0.10), 1.3),
]

# three counter-clockwise points in z = 0, so the normal points to +z and the top half is kept
PLANE = [[10.0, 10.0, 0.0], [-10.0, 10.0, 0.0], [-10.0, -10.0, 0.0]]

PROJECT_OPTIMIZE = {
    "checkpointing_interval": "0",
    "ending_regularization": "10.000000",
    "initial_relative_weighting": "0.050000",
    "iterations_per_split": "300",
    "multiscale": "false",
    "number_of_particles": "64",
    "optimization_iterations": "300",
    "procrustes": "false",
    "relative_weighting": "1.000000",
    "starting_regularization": "500.000000",
    "use_geodesic_distance": "false",
    "use_normals": "0",
    "verbosity": "0",
}


def make_mesh(radii, shear, stretch):
    sphere = vtk.vtkSphereSource()
    sphere.SetRadius(1.0)
    sphere.SetThetaResolution(32)
    sphere.SetPhiResolution(32)
    sphere.Update()

    # the source duplicates the seam vertices; merge them so the surface is closed
    clean = vtk.vtkCleanPolyData()
    clean.SetInputData(sphere.GetOutput())
    clean.Update()

    triangles = vtk.vtkTriangleFilter()
    triangles.SetInputData(clean.GetOutput())
    triangles.Update()
    poly = triangles.GetOutput()

    points = poly.GetPoints()
    for i in range(points.GetNumberOfPoints()):
        x, y, z = np.array(points.GetPoint(i)) * np.array(radii)
        if z < 0:
            # zero slope at z = 0, so the surface stays smooth across the plane
            x += shear[0] * z * z
            y += shear[1] * z * z
            z *= stretch
        points.SetPoint(i, x, y, z)
    poly.Modified()
    return poly


def write_mesh(poly, filename):
    writer = vtk.vtkPolyDataWriter()
    writer.SetFileName(filename)
    writer.SetInputData(poly)
    writer.Write()


def main():
    os.makedirs("meshes", exist_ok=True)
    os.makedirs("constraints", exist_ok=True)

    data = []
    for name, radii, shear, stretch in SUBJECTS:
        mesh_file = f"meshes/{name}.vtk"
        constraint_file = f"constraints/{name}_constraints.json"
        write_mesh(make_mesh(radii, shear, stretch), mesh_file)
        with open(constraint_file, "w") as f:
            json.dump({"planes": [{"points": PLANE}]}, f, indent=4)
        data.append({
            "name": name,
            "shape_1": mesh_file,
            "groomed_1": mesh_file,
            "constraints_1": constraint_file,
            "local_particles_1": f"project_particles/{name}_local.particles",
            "world_particles_1": f"project_particles/{name}_world.particles",
        })

    project = {
        "data": data,
        "optimize": PROJECT_OPTIMIZE,
        "project": {"version": "2"},
    }
    with open("project.swproj", "w") as f:
        json.dump(project, f, indent=1)


if __name__ == "__main__":
    main()
