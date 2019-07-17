 
-------------------------------
ComputeGeodesicDistanceToCurve 
-------------------------------

Compute geodesic distances on given triangular meshes (ply format) to a set of given curves 

It uses a parameter file with the following tags
 - with_fids: a flag to indicate whether to use fids to generate the geodesics, if 0, kdtree will be used instead (filenames will be affected), if true, we need to provide
              the spacing, size and origin of their distance transforms that generated those fids files
 - mesh:      a list of ply file names for meshes to be processed
 - fids:      a list of corresponding fids files (in case with_fids = 1)
 - origin_x, origin_t, origin_z: the origin in physical units of the resulting distance transform
 - spacing_x, spacing_y, spacing_z: voxel spacing of the resulting distance transform
 - size_x, size_y, size_z: the size (rows,cols,slices) of the resulting distance transform

 - curves_prefix: where we can find the point files of the curves, this should be the common part of the filename for all given mesh files
 - curves_suffix: string (optional) that will suffix all curve files
 - curves_ext:    the extension of the curve files
 - curve_labels:  the name of each curve according to the suffix given for each curve pts file, this will also be used in the naming of the output feature files (fea)
 
 
-------------------------------
TODO: CLEARED
-------------------------------
This tool uses fids to project a given landmark onto the closest VERTEX on the mesh then use this vertex as a seed for geodesics computation. It will be more accurate to project the point onto 
a triangle and use its vertices as seeds to initiate geodesics to the rest of the mesh then use geodesic approximation scheme to obtain the geodesic from the projected point to all the mesh vertices.
