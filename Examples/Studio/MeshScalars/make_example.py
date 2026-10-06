#!/usr/bin/env python
"""
Build the Mesh Scalars example: ellipsoids that are nearly the same shape, each carrying a
scalar field ("scalar_value") with a patch whose position differs from subject to subject.

Geometry alone cannot tell where the patch is, so a model built from position only leaves the
patch smeared across particles.  Using the field as a correspondence attribute makes the particles
follow it: untick "scalar_value" under Mesh Scalars in the Optimize panel and run again to compare.

    python make_example.py [output_dir]
"""

import json
import os
import sys

import numpy as np
import vtk
from vtk.util.numpy_support import numpy_to_vtk, vtk_to_numpy

NUM_SUBJECTS = 10
RADII = np.array([20.0, 14.0, 10.0])
PATCH_WIDTH = np.radians(25.0)  # standard deviation of the patch, as an angle
PATCH_RANGE = np.radians(35.0)  # the patch center moves this far either side of the top


def ellipsoid(radii, subdivisions=4):
    source = vtk.vtkPlatonicSolidSource()
    source.SetSolidTypeToIcosahedron()
    tri = vtk.vtkTriangleFilter()
    tri.SetInputConnection(source.GetOutputPort())
    sub = vtk.vtkLinearSubdivisionFilter()
    sub.SetInputConnection(tri.GetOutputPort())
    sub.SetNumberOfSubdivisions(subdivisions)
    sub.Update()
    out = vtk.vtkPolyData()
    out.SetPoints(vtk.vtkPoints())
    out.SetPolys(sub.GetOutput().GetPolys())
    unit = vtk_to_numpy(sub.GetOutput().GetPoints().GetData()).astype(float)
    unit /= np.linalg.norm(unit, axis=1, keepdims=True)
    out.GetPoints().SetData(numpy_to_vtk(unit * radii, deep=True))
    return out, unit


def main(out_dir):
    os.makedirs(out_dir, exist_ok=True)
    rng = np.random.default_rng(7)
    data = []
    angles = np.linspace(-PATCH_RANGE, PATCH_RANGE, NUM_SUBJECTS)
    for i, angle in enumerate(angles):
        radii = RADII * (1.0 + rng.uniform(-0.06, 0.06, 3))
        mesh, unit = ellipsoid(radii)
        # the patch sits on the top of the ellipsoid, shifted along its long axis
        center = np.array([np.sin(angle), 0.0, np.cos(angle)])
        separation = np.arccos(np.clip(unit @ center, -1.0, 1.0))
        field = np.exp(-0.5 * (separation / PATCH_WIDTH) ** 2).astype(np.float32)
        array = numpy_to_vtk(field, deep=True)
        array.SetName("scalar_value")
        mesh.GetPointData().SetScalars(array)
        name = "ellipsoid_%02d.vtk" % (i + 1)
        writer = vtk.vtkPolyDataWriter()
        writer.SetFileName(os.path.join(out_dir, name))
        writer.SetInputData(mesh)
        writer.SetFileVersion(42)
        writer.SetFileTypeToBinary()
        writer.Write()
        data.append({"name": "ellipsoid_%02d" % (i + 1), "shape_1": name})

    optimize = {
        "number_of_particles": "128",
        "initialization_mode": "split",
        "iterations_per_split": "1000",
        "optimization_iterations": "1000",
        "relative_weighting": "10",
        "initial_relative_weighting": "0.05",
        "starting_regularization": "1000",
        "ending_regularization": "10",
        "procrustes": "false",
        "field_attributes": "scalar_value",
        "field_attribute_weights": "100",
    }
    # the meshes are already clean and aligned; grooming only has to pass them (and the field) through
    groom = {"1": {"remesh": "false", "fill_holes": "false", "alignment_enabled": "false"}}
    project = {"data": data, "groom": groom, "optimize": optimize}
    json.dump(project, open(os.path.join(out_dir, "mesh_scalars.swproj"), "w"), indent=1)
    print("wrote %d meshes and mesh_scalars.swproj to %s" % (len(data), out_dir))


if __name__ == "__main__":
    main(sys.argv[1] if len(sys.argv) > 1 else ".")
