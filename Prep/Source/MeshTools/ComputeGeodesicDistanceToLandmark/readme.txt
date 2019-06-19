 

-------------------------------
ComputeGeodesicDistanceToLandmark 
-------------------------------

Compute geodesic distances on given triangular meshes (ply format) to a set of given landmarks 

It uses a parameter file with the following tags"
  - with_fids: a flag to indicate whether to use fids to generate the geodesics, if 0, kdtree will be used instead (filenames will be affected), if true, we need to provide
	       the spacing, size and origin of their distance transforms that generated those fids files
  - origin_x, origin_t, origin_z: the origin in physical units of the resulting distance transform
  - spacing_x, spacing_y, spacing_z: voxel spacing of the resulting distance transform
  - size_x, size_y, size_z: the size (rows,cols,slices) of the resulting distance transform
  - mesh:      a list of ply file names for meshes to be processed
  - fids:      a list of corresponding fids files (in case with_fids = 1)
  - landmark_files: a list of text files containing the 3d point locations of the landmarks associated with each mesh (a single file for each mesh with each row a different landmark xyz)
  - landmark_labels: the name of each landmark specified in landmark files, this will be used in the naming of the output feature files (fea)
 
 
-------------------------------
TODO: CLEARED
-------------------------------
This tool uses fids to project a given landmark onto the closest VERTEX on the mesh then use this vertex as a seed for geodesics computation. It will be more accurate to project the point onto 
a triangle and use its vertices as seeds to initiate geodesics to the rest of the mesh then use geodesic approximation scheme to obtain the geodesic from the projected point to all the mesh vertices.
