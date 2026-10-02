# Mesh Scalars as Correspondence Attributes in Studio

Meshes often carry a per-vertex scalar field: thickness, curvature, a parametric coordinate, a measurement of your own.  ShapeWorks Studio can use such a field as a correspondence attribute, so that particles line up on the field as well as on the geometry.  This helps when the shape alone does not say where a feature is.  See also [Mesh Scalar Attributes](../workflow/optimize.md#mesh-scalar-attributes).

## Example Data

The example data for this tutorial is provided in the ``Examples/Studio/MeshScalars`` directory.

To open the project, start ShapeWorks Studio, and open the project file ``Examples/Studio/MeshScalars/mesh_scalars.swproj``.

The data is ten ellipsoids of nearly the same shape.  Each carries a field named `activation`: a patch near the top of the ellipsoid whose position along the long axis differs from subject to subject.  Nothing in the geometry marks where the patch is.  The script `make_example.py` in the same directory regenerates the data.

## Grooming

The meshes are already clean and aligned, so the grooming steps are turned off.  Run Groom to pass them through.  When grooming steps such as remeshing are used, scalar fields are carried onto the groomed meshes, which is where optimization reads them.

## Optimization

The Optimize panel shows a Mesh Scalars row for each field found on the meshes.  In this project `activation` is checked, with a Mesh Scalar Weight of 100.  The weight scales the field relative to XYZ: the field here ranges from 0 to 1 while the ellipsoids are tens of units across, so it needs a large weight to count.

Run Optimize.  To see what the field contributes, uncheck `activation`, run Optimize again, and compare.

## Analysis

Choose `activation` as the feature map in the Samples view to see the patch on each subject.

| | Without the field | With the field |
| --- | --- | --- |
| Particle at the centre of the patch | a different particle in most subjects | the same particle in every subject |
| Spread of `activation` at a particle, across subjects | 0.065 | 0.009 |
| First PCA mode | the small differences in size | the patch moving along the ellipsoid |

With the field in use, each particle sits at the same place relative to the patch in every subject, and the first mode of variation is the movement of the patch, which a model built from geometry alone cannot see.
