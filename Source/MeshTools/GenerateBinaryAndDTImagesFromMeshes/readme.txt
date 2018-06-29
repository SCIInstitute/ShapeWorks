 

-------------------------------
GenerateBinaryAndDTImagesFromMeshes 
-------------------------------

Compute binary images (i.e. segmentation) and their distance transforms of a given set of meshes (ply format)

It uses a parameter file with the following tags"
  - mesh: a list of ply file names for meshes to be processed
  - origin_x, origin_y, origin_z: the origin in physical units of the resulting distance transform
  - spacing_x, spacing_y, spacing_z: voxel spacing of the resulting distance transform
  - size_x, size_y, size_z: the size (rows,cols,slices) of the resulting distance transform
  
